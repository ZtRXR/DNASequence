#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include<fstream>
#include <sstream>
#include <stdexcept>
#include<algorithm>
#include"tools.hpp" // 自己写的库，在src/tools/tools.hpp当中，注意要使用C++23标准编译
#include <cstring>
#include <stdio.h>
#include <string_view>
#include <type_traits>
#include <unordered_map>


// 这两个宏用来申请读入和读出流，实现反射并输出日志，获取申请流的变量名字

#define OPEN_IFS_AND_CHECK(file_path,value_name)std::ifstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open input file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open input file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

#define OPEN_OFS_AND_CHECK(file_path,value_name)std::ofstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open output file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open output file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}
//最大DNA序列长度
const size_t MAX_SIZE = 5e4+5;

void reverseComplement(auto &DNAsequence, const size_t buf_size) //注意这里使用引用DNA sequence，避免拷贝开销
{
    static const std::unordered_map<char, char> complement = { //这里使用查表的方式大大提高CPU速度，因为if分支CPU不容易命中缓存，需要使用查表加速
        {'A', 'T'}, {'a', 'T'},
        {'T', 'A'}, {'t', 'A'},
        {'C', 'G'}, {'c', 'G'},
        {'G', 'C'}, {'g', 'C'}
    };

    std::reverse(DNAsequence.begin(), DNAsequence.begin() + buf_size); //翻转DNA序列
    
    for (std::remove_const_t<decltype(buf_size)> i = 0; i < buf_size; ++i) { //std::remove_const_t<decltype(buf_size)>意思是和buf_size相同的类型并去掉const
        auto it = complement.find(DNAsequence[i]);//查表并替换
        if (it != complement.end()) {
            DNAsequence[i] = it->second;
        }
    }
}


class Spent{ // 使用RAII原理的自动计时器，计算主函数运行时间，析构时自动输出
private:
    const decltype(std::chrono::system_clock::now()) start;
    const std::string_view name;
public:
    Spent(std::string_view name)noexcept:start(std::chrono::system_clock::now()),name(name){
        zt::print("[Timer: ",name,"]"," Start timing","\n");
    }
    ~Spent()noexcept{
        const auto end = std::chrono::system_clock::now();
        const auto dur = std::chrono::duration_cast<std::chrono::milliseconds> (end-start);
        zt::print("[Timer: ",name,"]"," Stop timing , using ", dur.count(),"ms\n");
    }
};

int main()
{
	try{
        std::ios_base::sync_with_stdio(false); //加了没效果 
        // using namespace std; // 别加，刚被坑了

        Spent all_spent("All spent"); //自动计时器，给主函数计时
        
        std::array<char,MAX_SIZE> buf;

        unsigned int lines = 0;

        std::filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        OPEN_IFS_AND_CHECK(input_path, input_file_stream) //创建输入和输出流
        OPEN_OFS_AND_CHECK(output_path, output_file_stream)

        while (input_file_stream.getline(buf.data(),MAX_SIZE,'\n'))
        {
            auto m = lines%=2; //防止溢出
            const auto buf_len = strlen(buf.data());
            const std::string_view suffix("\n"); //设置一个每个DNA序列结尾的字符，这里是以\n换行来结尾
            if (m == 1){
                // output_file_stream << reverseComplement(buf) << endl;
                reverseComplement(buf,buf_len);
            }
            // buf+=suffix;
            for(std::remove_const_t<decltype(suffix.size())> i=0;i<suffix.size();i++){
                buf[buf_len+i] = suffix[i];
            }
            output_file_stream.write(buf.data(), buf_len+suffix.size()); // 写入文件
            lines++;
        }
        
        return 0;
    }catch(const std::exception &e){
        zt::eprint(
            "Caught an error because:\n",
            "\t",NAME_VALUE(e.what()),"\n"
            "Closing\n"
        );
    }catch(...){
        zt::eprint(
            "Caught an unknown error :\n",
            "Closing\n"
        );
    }
    return -1;
}
