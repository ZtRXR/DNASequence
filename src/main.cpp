#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include<fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include<algorithm>
#include"tools.hpp" // 自己写的库，在src/tools/tools.hpp当中，注意要使用C++23标准编译
#include <cstring>
#include <stdio.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>


// 这两个宏用来申请读入和读出流，实现反射并输出日志，获取申请流的变量名字

#define OPEN_IFS_AND_CHECK(file_path,value_name)std::ifstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open input file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open input file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

#define OPEN_OFS_AND_CHECK(file_path,value_name)std::ofstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open output file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open output file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

//最大DNA序列长度
const size_t MAX_SIZE_PER_DNA = 5e4+5;

void reverseComplement(char *begin, char *end) //注意end是开区间，不能访问end
{
    static const std::unordered_map<char, char> complement = { //这里使用查表的方式大大提高CPU速度，因为if分支CPU不容易命中缓存，需要使用查表加速
        {'A', 'T'}, {'a', 'T'},
        {'T', 'A'}, {'t', 'A'},
        {'C', 'G'}, {'c', 'G'},
        {'G', 'C'}, {'g', 'C'}
    };

    std::reverse(begin, end); //翻转DNA序列
    
    for (std::remove_const_t<decltype(begin)> i = begin; i < end; ++i) { //std::remove_const_t<decltype(buf_size)>意思是和buf_size相同的类型并去掉const
        auto it = complement.find(*i);//查表并替换
        if (it != complement.end()) [[likely]] {
            *i = it->second;
        }
    }
}


class Spent{ // 使用RAII原理的自动计时器，计算主函数运行时间，析构时自动输出
private:
    const decltype(std::chrono::system_clock::now()) start;
    const std::string name;
public:
    Spent(const std::string name)noexcept:start(std::chrono::system_clock::now()),name(name){
        zt::print("[Timer: ",name,"]"," Start timing","\n");
    }
    ~Spent()noexcept{
        const auto end = std::chrono::system_clock::now();
        const auto dur = std::chrono::duration_cast<std::chrono::milliseconds> (end-start);
        zt::print("[Timer: ",name,"]"," Stop timing , used ", dur.count(),"ms\n");
    }
};

int main()
{
	try{
        //std::ios_base::sync_with_stdio(false); //加了没效果 //这里直接关掉就行了，不会影响读入，因为目前是一次性读入。开了反而会让日志输出变成全缓冲，不友好
        // using namespace std; // 别加，刚被坑了
        
        // std::array<char,MAX_SIZE> buf;

        bool lines = 0; //使用布尔值加速

        const auto get_lines_add = [&lines]() {
            bool old_value = lines;  // 保存旧值
            lines = !lines;          // 改变布尔值
            return old_value;        // 返回旧值
        };


        std::filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        OPEN_IFS_AND_CHECK(input_path, input_file_stream) //创建输入和输出流
        OPEN_OFS_AND_CHECK(output_path, output_file_stream)

        const size_t BUF_SIZE  = (size_t)4 * 1024 * 1024 *1024; //4GB + 区块大小一点冗余 ///////////////////////////设置区块大小
        // const size_t BUF_SIZE  = (size_t)400*1024*1024; //4GB + 一点冗余 // 测试用
        zt::print("Chunk size :",BUF_SIZE," bytes\n");

        std::vector<char> buf(BUF_SIZE); // 堆上分配可以大一点
        std::array<char, MAX_SIZE_PER_DNA> tmp_buf;//用于处理截断的DNA，直接在栈上申请
        
        Spent all_spent("All spent"); //自动计时器，给主函数计时
        unsigned int chunk_id = 0;
        size_t last_buf_size = 0;
        while (input_file_stream.eof()==false)
        {
            Spent chunk_spent(zt::fmt("chunk_id:[",++chunk_id,"]"));
            {
                Spent chunk_read_spent(zt::fmt("read_chunk_id:[",chunk_id,"]"));
                input_file_stream.read(buf.data(),buf.size());
            }
            // lines=!lines; //防止溢出
            const auto buf_len = input_file_stream.gcount();

            zt::print(NAME_VALUE(buf_len),"\n");
            
            if(buf_len == std::numeric_limits<decltype(buf_len)>::max())[[unlikely]]{
                THROW_RT_ERROR("get input file stream read buf size failed\n")
            }

            if(buf_len == 0)[[unlikely]]{
                break;
            }

            const std::string_view buf_str_v(buf.data(),buf_len); //string_view是零拷贝，但是要注意悬垂引用

            size_t start_pos = 0;
            size_t end_pos = 0;

            if(last_buf_size>0)[[likely]]{
                Spent recovery_interrupt_spent(zt::fmt("recovery_interrupt [",chunk_id,"]"));
                if((end_pos=buf_str_v.find('\n',start_pos)) != std::string_view::npos)[[likely]]{
                    std::memcpy(tmp_buf.data()+last_buf_size,buf.data(),end_pos+1);
                    if(get_lines_add()){
                        reverseComplement(tmp_buf.data(), tmp_buf.data()+last_buf_size+end_pos);
                    }
                    // lines=!lines;
                    output_file_stream.write(tmp_buf.data(), last_buf_size+end_pos+1);
                }else{
                    THROW_RT_ERROR("DNA incompleteness")
                }
                last_buf_size=0;
            }

            {
                Spent calculate_spent(zt::fmt("calculate_chunk_id:[",chunk_id,"]"));

                while((end_pos=buf_str_v.find('\n',start_pos)) != std::string_view::npos){
                    if(get_lines_add()){
                        reverseComplement(buf.data()+start_pos, buf.data()+end_pos);
                    }
                    // lines=!lines;
                    start_pos=end_pos+1;
                }
            }
            
            if(start_pos!=buf_len){
                zt::print("Saving interrupt chunk_id[",chunk_id,"]\n");
                std::memcpy(tmp_buf.data(),buf.data()+start_pos+1,(last_buf_size = buf_len-start_pos-1));
            }
            {
                Spent chunk_write_spent(zt::fmt("write_chunk_id:[",chunk_id,"] , ","[Wrote bytes] ",NAME_VALUE(start_pos)));
                output_file_stream.write(buf.data(), start_pos);
            }
            
            
        }

    }catch(const std::exception &e){
        zt::eprint(
            "Caught an error because:\n",
            "\t",NAME_VALUE(e.what()),"\n"
            "Closing\n"
        );
        throw  e;
    }catch(...){
        zt::eprint(
            "Caught an unknown error :\n",
            "Closing\n"
        );
        throw;
    }
}
