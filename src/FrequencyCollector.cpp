#include "FrequencyCollector.hpp"
#include "PfHelpers.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <string.h>
#include <regex>

using std::string;

FrequencyCollector::FrequencyCollector(int N) :
	n{N}, totalCount{0}, freqs{} {}

FrequencyCollector::~FrequencyCollector() {}

bool FrequencyCollector::validNgramFile(char fileName[]) {
	try {
		numFileLines(fileName);
	} catch (const std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return false;
	}

	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		std::cerr << "Failed to open: " << fileName << '\n';
		return false;
	}

	std::unordered_map<string, count_t> NGrams;
	long lineNum = 1;
	while(!fileReader.eof()) {
		string line, ngramString, countString;
		std::getline(fileReader, line);

		std::regex format("^[[:alpha:]]{" + std::to_string(n) + "} [[:digit:]]+$");
		if(!std::regex_match(line, format)) {
			std::cerr << "Line " << lineNum << " of " << fileName << "is of wrong format";
			return false;
		}

		string ngram = line.substr(0,n);
		if(NGrams.find(ngram) != NGrams.end()) {
			std::cerr << "Line " << lineNum << " of " <<fileName << "has duplicate: " <<
				ngram << '\n';
			return false;
		}
		NGrams.insert(std::make_pair(ngram, 0));

		++ lineNum;
	}
	return true;
}

int FrequencyCollector::readNgramCount(char fileName[]) {
	totalCount = 0;
	unsigned long numLines;
	try {
		numLines = numFileLines(fileName);
	} catch (const std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		throw;
	}
	freqs.clear();
	freqs.reserve(numLines);

	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}
	
	while(!fileReader.eof()) {
		//	Read each line, split line into - n-gram and count
		string line, ngramString, countString;
		std::getline(fileReader, line);
		// 	Split the line
		std::stringstream ss(line);
		ss >> ngramString;
		ss >> countString;
		count_t count = std::stoi(countString);

		totalCount += count;

		// 	If the n-gram read was of the wrong length.
		if((int)ngramString.size() != n) {
			std::cerr << "Ngram length contradiction!" << '\n';
			std::cerr << "  Ngram read from " << fileName << " was of length: " << ngramString.size() << '\n';
			std::cerr << "  Ngram length expected: " << n << '\n';
			throw Exception("Error: Ngram length contradiction");
		}
		// 	Convert string to char[]
		char* ngram = new char[n];
		for(int i = 0; i < n; i ++) {
			ngram[i] = ngramString.at(i);
		}
		// 	Add ngram:count to hashmap nGrams
		freqs.insert(std::make_pair(ngram, count));
		delete[] ngram;
	}
	return 0;
}
// Throws exception
int FrequencyCollector::writeNGramCount(char fileName[]) {
	std::ofstream fileWriter(fileName);
	if(fileWriter.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}

	for(auto it = freqs.begin(); it != freqs.end(); ++it) {
		fileWriter << it->first << " " << it->second << '\n';
	}
	return 0;
}

// Throws exception
int FrequencyCollector::setNGramCount(char fileRead[], char fileWrite[]) {
	collectNGramsFile(fileRead);
	writeNGramCount(fileWrite);
	return 0;
}

// Throws exception
int FrequencyCollector::collectNGramsFile(char fileName[]) {
	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}

	std::stringstream buffer;
	buffer << fileReader.rdbuf();

	return collectNGrams(buffer);
}

int FrequencyCollector::collectNGrams(std::stringstream &buffer) {
	totalCount = 0;
	// 	Read stream 1 letter at a time and add them to this queue
	// 	Queue will "rotate" as letters get added
	char *queue = new char[n];
	//	Keep track of the current start of the queue
	int curPos = 0;
	// 	This first while-loop is just to fill up the queue
	//	Collect the first N letters before adding the ngram to map
	char ch;
	while(buffer.get(ch)){
		if(!isalpha(ch)) continue;
		queue[curPos++] = toupper(ch);
		// 	Fill up the queue before adding ngrams to map
		if(curPos >= n-1) break;
	}
	// 	Read buffer one character at a time
	while(buffer.get(ch)) {
		//  Only select the letters
		if(!isalpha(ch)) continue;
		++totalCount;
		// 	Make sure it is uppercase
		queue[curPos++] = toupper(ch);
		curPos = curPos % n;
		// 	Reorder the chars starting with curPos and wrapping
		string ngram;
		for(int i = 0; i < n; i++) {
			ngram.push_back(queue[(curPos + i) % n]);
		}
		// 	Count how many times that n-gram occurs
		freqs[ngram] ++;
	}
	delete[] queue;
	return 0;
}

// Throws exception
unsigned long FrequencyCollector::numFileLines(char* fileName) {
	unsigned long numLines = 0;
	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}
	
	string buffer;
	while(!fileReader.eof()) {
		std::getline(fileReader, buffer);
		++ numLines;
	}
	return numLines;
}
int FrequencyCollector::printNGrams() {
    for(auto it = freqs.begin(); it != freqs.end(); ++it) {
        std::cout << it->first << " " << it->second << '\n';
    }
    return 0;
}

double FrequencyCollector::frequency(std::string ngram) const {
	auto tF = freqs.find(ngram);
		if(tF != freqs.end())
			return double(tF->second) / totalCount;
		else
			return 0;
}

int FrequencyCollector::getN() const {
	return n;
}

int FrequencyCollector::clear() {
	totalCount = 0;
	freqs.clear();
	return 0;
}

bool FrequencyCollector::isEmpty() const {
	if(freqs.empty()) return true;
	return false;
}
