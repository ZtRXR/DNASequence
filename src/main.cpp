#include "dna.hpp"

int main()
{
	try{
        //参数列表 <文件分块大小，单个DNA序列最长大小>("输入文件名",输出文件名);
        dna::open_file_and_calculate<(size_t)4 * 1024 * 1024 *1024,(size_t)5e4+5>("filteredReads.txt", "reversedSequence.txt");

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
