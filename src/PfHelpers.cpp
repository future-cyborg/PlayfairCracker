#include "PfHelpers.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string.h>
#include <unordered_map>

using std::vector;
using std::string;
using std::unordered_map;

Exception::Exception(const char* msg) : e_msg{msg} {}
Exception::~Exception( ) {}
const char* Exception::what( ) const throw() {return(e_msg);}

InvalidKeyException::InvalidKeyException(const char* msg) : Exception{msg} {}
InvalidKeyException::~InvalidKeyException( ) {}

InvalidParameters::InvalidParameters(const char* msg) : Exception{msg} {}
InvalidParameters::~InvalidParameters( ) {}

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
			string e = "Failed to open: ";
			e += fileName;
			throw std::ios_base::failure(e.c_str());
		}
		
		char ch;
		while(fileReader.get(ch)) {
			text.push_back(ch);
		}
		return 0;

	}

	int printPopulation(vector<string> &population) {
		for(unsigned i = 0; i < population.size(); i++) {
			std::cout << population[i] << '\n';
		}
		return 0;
	}

	template <typename Iterable>
	bool validKey(Iterable key) {
		if(key.size() != 25) return false;

		unordered_map<char, int> letterUsed;
		for(unsigned index = 0; index < key.size(); index++) {
			//	If the letter has alreay been put in
			if(letterUsed.find(key[index]) != letterUsed.end()) {
				return false;
			}
			letterUsed.insert(std::make_pair(key[index], 0));
		}
		return true;
	}

	template <typename Number>
	Number sumVector(vector<Number> vec) {
		Number total = 0;
		for(auto it = vec.begin(); it != vec.end(); ++it)
			total += *it;
		return total;
	}

	template bool validKey(string);
	template bool validKey(vector<char>);
	template double sumVector(vector<double>);

	Timer::Timer() : m_beg(clock_t::now()) {}
	
	int Timer::reset() {
		m_beg = clock_t::now();
		return 0;
	}
	
	double Timer::elapsed() const {
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
	
}