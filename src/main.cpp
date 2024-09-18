#include <exception>
#include <filesystem>
#include<fstream>
#include <stdexcept>
#include<string>
#include<algorithm>
#include<iostream>
#include <type_traits>
#include"tools.hpp"

// std::string reversedComplement(std::string DNAsequence);

std::string reverseComplement(std::string DNAsequence)
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
        std::ios_base::sync_with_stdio(false); //加了没效果 
        using namespace std;
        
        char buf[50000];
        int lines = 0;

        filesystem::path input_path("filteredReads.txt"),output_path("reversedSequence.txt");
        
        ifstream input_file_stream(input_path);
        ofstream output_file_stream(output_path);
        
        zt::check_fstream_isopen(input_file_stream, output_file_stream);

        zt::print("open file ok!\n");

        string l = "";
        
        while (input_file_stream.getline(buf,50000,'\n'))
        {
            int m = lines%2;
            if (m == 1)
            output_file_stream << reverseComplement(buf) << endl;
            else
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
