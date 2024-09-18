#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include<fstream>
#include <ratio>
#include <sstream>
#include <stdexcept>
#include<algorithm>
#include"tools.hpp"
#include <cstring>
#include <string_view>


// std::string reversedComplement(std::string DNAsequence);

#define OPEN_IFS_AND_CHECK(file_path,value_name)std::ifstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open input file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open input file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

#define OPEN_OFS_AND_CHECK(file_path,value_name)std::ofstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open output file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open output file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

const size_t MAX_SIZE = 5e4+5;

void reverseComplement(std::array<char, MAX_SIZE> &DNAsequence,const size_t buf_size) // 注意这里要用'&'取引用，这样不用拷贝，大大减少耗时
{
	using namespace std;
	
	// string l = "";
	int i;
	int q = strlen(DNAsequence.data());
	reverse(DNAsequence.begin(), DNAsequence.begin()+q); //将字符串反过来 
	for (i = 0; i < q; i++)
	{
        char &l = DNAsequence[i];
		if (DNAsequence[i] == 'A'){
			// l = l + 'T';
            l = 'T';
        }
		if (DNAsequence[i] == 'a'){
			// l = l + 'T';
            l = 'T';
        }
		if (DNAsequence[i] == 'c'){
			// l = l + 'G';
            l = 'G';
        }
		if (DNAsequence[i] == 'C'){
			// l = l + 'G';
            l = 'G';
        }
		if (DNAsequence[i] == 'g'){
			// l = l + 'C';
            l = 'C';
        }
		if (DNAsequence[i] == 'G'){
			// l = l + 'C';
            l = 'C';
        }
		if (DNAsequence[i] == 't'){
			// l = l + 'A';
            l = 'A';
        }
		if (DNAsequence[i] == 'T'){
			// l = l + 'A';
            l = 'A';
        }
	}
}

class Spent{
    const decltype(std::chrono::system_clock::now()) start;
    const std::string_view name;
public:
    Spent(std::string_view name)noexcept:start(std::chrono::system_clock::now()),name(name){
        zt::print("[",name,"]"," Start timing","\n");
    }
    ~Spent(){
        const auto end = std::chrono::system_clock::now();
        const auto dur = std::chrono::duration_cast<std::chrono::milliseconds> (end-start);
        zt::print("[",name,"]"," Stop timing , using ", dur);
    }
};

int main()
{
	try{
        // std::ios_base::sync_with_stdio(false); //加了没效果 
        using namespace std;

        Spent all_spent("All spent");

        const auto start = chrono::system_clock::now();
        
        std::array<char,MAX_SIZE> buf;
        int lines = 0;

        filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        // ifstream input_file_stream(input_path);
        // ofstream output_file_stream(output_path);
        OPEN_IFS_AND_CHECK(input_path, input_file_stream)
        OPEN_OFS_AND_CHECK(output_path, output_file_stream)

        // string l = "";
        zt::print("Undergoing transformation\n");

        
        
        while (input_file_stream.getline(buf.data(),MAX_SIZE,'\n'))
        {
            int m = lines%2;
            const auto buf_len = strlen(buf.data());
            const std::string_view suffix("\n");
            if (m == 1){
                // output_file_stream << reverseComplement(buf) << endl;
                reverseComplement(buf,buf_len);
            }
            for(int i=0;i<suffix.size();i++){
                buf[buf_len+i]=suffix[i];
            }
            output_file_stream.write(buf.data(), buf_len+suffix.size());
            lines++;
        }
        
        return 0;
    }catch(const std::exception &e){
        zt::print(
            "Caught an error because:\n",
            "\t",NAME_VALUE(e.what()),"\n"
            "closing\n"
        );
    }catch(...){
        zt::print(
            "Caught an unknown error because:\n",
            "Closing\n"
        );
    }
    return -1;
}
