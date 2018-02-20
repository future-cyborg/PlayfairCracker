#ifndef PF_HELPER_H
#define PF_HELPER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using std::vector;
using std::string;

namespace PfHelper {
	std::ifstream::pos_type fileSize(const string fileName) {
		std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
		return in.tellg(); 
	}

	int readFile(const string fileName, vector<char> &text) {
		text.clear();
		text.reserve(fileSize(fileName));
		std::ifstream fileReader(fileName);
		if(!fileReader) {
			std::cerr << fileName << " can not be opened." << '\n';
			return -1;
		}
		char ch;
		while(fileReader.get(ch)) {
			text.push_back(ch);
		}
		return 0;

	}
}


#endif // PF_HELPER_H_