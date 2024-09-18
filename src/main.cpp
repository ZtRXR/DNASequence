#include <exception>
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


template <class ...Args>
void check_fstream_isopen(const Args&...args)noexcept(false){
    bool is_open=true;
    (((!args.is_open())?is_open=false:true),...);
    if(is_open==false){
        throw std::runtime_error("cannot open file stream ");
    }
}


int main()
{
	try{
        std::ios_base::sync_with_stdio(false); //加了没效果 
        using namespace std;
        
        char buf[50000];
        int lines = 0;

        
        
        ifstream inputFile("filteredReads.txt");
        ofstream outputFile("reversedSequence.txt");
        
        check_fstream_isopen(inputFile,outputFile);

        zt::print("open file ok!");

        string l = "";
        
        while (inputFile.getline(buf,50000,'\n'))
        {
            int m = lines%2;
            
            if (m == 1)
            outputFile << reverseComplement(buf) << endl;
            else
            outputFile << buf << endl;
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
