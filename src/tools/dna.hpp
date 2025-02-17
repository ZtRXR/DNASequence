#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "tools.hpp" // 自己写的库，在src/tools/tools.hpp当中，注意要使用C++23标准编译
#include <cstring>
#include <stdio.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <filesystem>


// 这两个宏用来申请读入和读出流，实现反射并输出日志，获取申请流的变量名字

#define OPEN_IFS_AND_CHECK(file_path,value_name)std::ifstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open input file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open input file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

#define OPEN_OFS_AND_CHECK(file_path,value_name)std::ofstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open output file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open output file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

namespace dna {
    //最大DNA序列长度
    // const size_t MAX_SIZE_PER_DNA = 5e4+5;
    //


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

    template<size_t BUF_SIZE = (size_t)4 * 1024 * 1024 *1024 , size_t MAX_SIZE_PER_DNA = (size_t)5e4+5>
    inline void open_file_and_calculate(std::filesystem::path input_path,std::filesystem::path output_path, std::function<void(char *begin, char *end)> reverseComplement){
        //std::ios_base::sync_with_stdio(false); //加了没效果 //这里直接关掉就行了，不会影响读入，因为目前是一次性读入。开了反而会让日志输出变成全缓冲，不友好
        // using namespace std; // 别加，刚被坑了
        
        // std::array<char,MAX_SIZE> buf;

        bool lines = 0; //使用布尔值加速
        const auto get_lines_add = [&lines]() {
            bool old_value = lines;  // 保存旧值
            lines = !lines;          // 改变布尔值
            return old_value;        // 返回旧值
        };


        // std::filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        OPEN_IFS_AND_CHECK(input_path, input_file_stream) //创建输入和输出流
        OPEN_OFS_AND_CHECK(output_path, output_file_stream)

        // const size_t BUF_SIZE  = (size_t)4 * 1024 * 1024 *1024; //4GB///////////////////////////设置区块大小
        // const size_t BUF_SIZE  = (size_t)400*1024*1024; //4GB + 一点冗余 // 测试用
        std::vector<char> buf(BUF_SIZE); // 堆上分配可以大一点
        std::array<char, MAX_SIZE_PER_DNA> tmp_buf;//用于处理截断的DNA，直接在栈上申请
        zt::print("Chunk size :",BUF_SIZE," bytes\n");
        
        Spent all_spent("All spent"); //自动计时器，给主函数计时
        unsigned int chunk_id = 0;
        size_t last_buf_size = 0;
        
        while (input_file_stream.eof()==false)
        {
            size_t recovered_size = 0;
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

            if( last_buf_size > 0 ) [[likely]] {
                Spent recovery_interrupt_spent(zt::fmt("recovery_interrupt [",chunk_id,"]"));
                if( (end_pos=buf_str_v.find('\n',start_pos)) != std::string_view::npos ) [[likely]] {

                    std::memcpy(tmp_buf.data()+last_buf_size,buf.data(),end_pos+1); //+1加的是空格位置

                    if(get_lines_add()){
                        reverseComplement(tmp_buf.data(), tmp_buf.data()+last_buf_size+end_pos); //注意传入函数时不要传入空格
                    }
                    // lines=!lines;
                    output_file_stream.write(tmp_buf.data(), last_buf_size+end_pos+1); //+1加的是空格位置
                    start_pos=end_pos+1;
                    recovered_size=end_pos+1;
                }else{
                    THROW_RT_ERROR("DNA incompleteness, chunk Size is too small !!")
                }
                last_buf_size=0;
            }

            {
                Spent calculate_spent(zt::fmt("calculate_chunk_id:[",chunk_id,"]"));

                #pragma omp parallel
                #pragma omp single
                {
                    while ((end_pos = buf_str_v.find('\n', start_pos)) != std::string_view::npos) {
                        char *start = buf.data() + start_pos;
                        char *end = buf.data() + end_pos;

                        // 确保每个任务只操作属于它的独立数据
                        if (get_lines_add()) {
                            // 为每一块数据创建一个任务
                            #pragma omp task
                            // #pragma omp task firstprivate(start, end)
                            {
                                reverseComplement(start, end); // 任务内处理反转互补
                            }
                        }

                        start_pos = end_pos + 1;
                    }
                }


                // while((end_pos=buf_str_v.find('\n',start_pos)) != std::string_view::npos){
                //     if(get_lines_add()){
                //         reverseComplement(buf.data()+start_pos, buf.data()+end_pos); //这个函数并行优化
                //     }
                //     // lines=!lines;
                //     start_pos=end_pos+1;
                // }
            }
            
            if(start_pos!=buf_len){
                zt::print("Saving interrupt chunk_id[",chunk_id,"]\n");
                std::memcpy(tmp_buf.data(),buf.data()+start_pos,(last_buf_size = buf_len-start_pos)); //这里边界处理脑子要炸了
            }

            {
                Spent chunk_write_spent(zt::fmt("write_chunk_id:[",chunk_id,"] , ","[Wrote bytes] ",NAME_VALUE(start_pos)));
                output_file_stream.write(buf.data()+recovered_size, start_pos-recovered_size);
            }
            
            
        }
    }
}