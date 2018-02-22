#include "PfHelpers.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string.h>

using std::vector;
using std::string;

InvalidKeyException::InvalidKeyException(const char* msg) : e_msg(msg) {}
InvalidKeyException::~InvalidKeyException( ) {}
const char* InvalidKeyException::getMessage( ) const {return(e_msg);}


InvalidParameters::InvalidParameters(const char* msg) : e_msg{msg} {}
InvalidParameters::~InvalidParameters( ) {}
const char* InvalidParameters::getMessage( ) const {return(e_msg);}

namespace PfHelpers {
	std::ifstream::pos_type fileSize(const string fileName) {
		std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
		return in.tellg(); 
	}

	// Throws exception
	int readFile(const char* fileName, vector<char> &text) {
		text.clear();
		text.reserve(fileSize(fileName));

		std::ifstream fileReader(fileName);
		if(fileReader.fail()) {
			char e[] = "Failed to open: ";
			strncat(e, fileName, 20);
			throw std::ios_base::failure(e);
		}
		
		char ch;
		while(fileReader.get(ch)) {
			text.push_back(ch);
		}
		return 0;

	}



	Timer::Timer() : m_beg(clock_t::now()) {}
	
	int Timer::reset() {
		m_beg = clock_t::now();
		return 0;
	}
	
	double Timer::elapsed() const {
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
	
}