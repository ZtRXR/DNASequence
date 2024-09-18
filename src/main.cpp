#include <array>
#include <exception>
#include <filesystem>
#include<fstream>
#include <sstream>
#include <stdexcept>
#include<string>
#include<algorithm>
#include<iostream>
#include"tools.hpp"

// std::string reversedComplement(std::string DNAsequence);

#define OPEN_IFS_AND_CHECK(file_path,value_name)std::ifstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open input file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open input file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

#define OPEN_OFS_AND_CHECK(file_path,value_name)std::ofstream value_name(file_path);if(value_name.is_open()==false){std::stringstream ss;ss<<"cannot open output file stream : "<<file_path.filename();throw std::runtime_error(ss.str());}else{zt::print("Open output file stream to value ["#value_name"] ok , from [",file_path.filename(),"]\n");}

const size_t MAX_SIZE = 5e4+5;

std::string reverseComplement(std::array<char, MAX_SIZE> DNAsequence)
{
	using namespace std;
	
	string l = "";
	int i;
	int q = DNAsequence.length();
	reverse(DNAsequence.begin(), DNAsequence.end()); //将字符串反过来 
	for (i = 0; i < q; i++)
	{
		if (DNAsequence[i] == 'A')
			l = l + 'T';
		if (DNAsequence[i] == 'a')
			l = l + 'T';
		if (DNAsequence[i] == 'c')
			l = l + 'G';
		if (DNAsequence[i] == 'C')
			l = l + 'G';
		if (DNAsequence[i] == 'g')
			l = l + 'C';
		if (DNAsequence[i] == 'G')
			l = l + 'C';
		if (DNAsequence[i] == 't')
			l = l + 'A';
		if (DNAsequence[i] == 'T')
			l = l + 'A';
	}
	return l;
}

int main()
{
	try{
        // std::ios_base::sync_with_stdio(false); //加了没效果 
        using namespace std;
        
        std::array<char,MAX_SIZE> buf;
        int lines = 0;

        filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        // ifstream input_file_stream(input_path);
        // ofstream output_file_stream(output_path);
        OPEN_IFS_AND_CHECK(input_path, input_file_stream)
        OPEN_OFS_AND_CHECK(output_path, output_file_stream)

        string l = "";
        
        while (input_file_stream.getline(buf.data(),50000,'\n'))
        {
            int m = lines%2;
            if (m == 1)
            reverseComplement(buf);
            // output_file_stream << reverseComplement(buf) << endl;
            // else
            output_file_stream << buf << endl;
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
