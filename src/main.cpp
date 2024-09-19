#include "dna.hpp"

void reverseComplement(char *begin, char *end) 
{
    //注意end是开区间，不能访问end
    static const std::unordered_map<char, char> complement = { //这里使用查表的方式大大提高CPU速度，因为if分支CPU不容易命中缓存，需要使用查表加速
        {'A', 'T'}, {'a', 'T'},
        {'T', 'A'}, {'t', 'A'},
        {'C', 'G'}, {'c', 'G'},
        {'G', 'C'}, {'g', 'C'}
    };

    // std::reverse(begin, end); //翻转DNA序列
    // 并行翻转序列 //似乎没用
    #pragma omp parallel for
    for (ptrdiff_t i = 0; i < (end - begin) / 2; ++i) {
        std::swap(begin[i], begin[(end - begin) - i - 1]);
    }

    // 并行查表替换
    #pragma omp parallel for
    for (ptrdiff_t i = 0; i < (end - begin); ++i) {
        static int _ = (zt::print(NAME_VALUE(omp_get_num_threads()),"\n"),0); // 打印线程数量
        auto it = complement.find(begin[i]);
        if (it != complement.end()) {
            begin[i] = it->second;
        }
    }
}

int main()
{
	try{
        //原理是这里定义处理函数，将函数传入open_file_and_calculate，在open_file_and_calculate中会调用传入的函数

        //参数列表 <文件分块大小，单个DNA序列最长大小>("输入文件名","输出文件名",序列处理函数);
        //这个函数在src/tools/dna里面
        dna::open_file_and_calculate<(size_t)4 * 1024 * 1024 *1024,(size_t)5e4+5>("filteredReads.txt", "reversedSequence.txt",reverseComplement);

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
