#include "FrequencyCollector.h"
#include "PfHelpers.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <string.h>

FrequencyCollector::FrequencyCollector() {}

FrequencyCollector::~FrequencyCollector() {}

int FrequencyCollector::readNgramCount(char fileName[], NGrams &nGrams) {
	int n = nGrams.n;
	nGrams.count = 0;
	unsigned long numLines;
	try {
		numLines = numFileLines(fileName);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		throw;
	}
	nGrams.freqs->reserve(numLines);

	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}
	
	while(!fileReader.eof()) {
		//	Read each line, split line into - n-gram and count
		std::string line, ngramString, countString;
		std::getline(fileReader, line);
		// 	Split the line
		std::stringstream ss(line);
		ss >> ngramString;
		ss >> countString;
		count_t count = std::stoi(countString);

		nGrams.count += count;

		// 	If the n-gram read was of the wrong length.
		if((int)ngramString.size() != n) {
			std::cerr << "Ngram length contradiction!" << '\n';
			std::cerr << "  Ngram read from " << fileName << " was of length: " << ngramString.size() << '\n';
			std::cerr << "  Ngram length expected: " << n << '\n';
			return -2;
		}
		// 	Convert string to char[]
		char* ngram = new char[n];
		for(int i = 0; i < n; i ++) {
			ngram[i] = ngramString.at(i);
		}
		// 	Add ngram:count to hashmap nGrams
		nGrams.freqs->insert(std::make_pair(ngram, count));
	}
	return 0;
}
// Throws exception
int FrequencyCollector::writeNGramCount(char fileName[], NGrams &nGrams) {
	std::ofstream fileWriter(fileName);
	if(fileWriter.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}

	for(auto it = nGrams.freqs->begin(); it != nGrams.freqs->end(); ++it) {
		fileWriter << it->first << " " << it->second << '\n';
	}
	return 0;
}

// Throws exception
int FrequencyCollector::setNGramCount(int n, char fileRead[], char fileWrite[]) {
	NGrams nGrams {n};
	collectNGramsFile(fileRead, nGrams);
	writeNGramCount(fileWrite, nGrams);
	return 0;
}

// Throws exception
int FrequencyCollector::collectNGramsFile(char fileName[], NGrams &nGrams) {
	std::ifstream fileReader(fileName);
	if(fileReader.fail()) {
		char e[] = "Failed to open: ";
		strncat(e, fileName, 20);
		throw std::ios_base::failure(e);
	}

	std::stringstream buffer;
	buffer << fileReader.rdbuf();

	return collectNGrams(buffer, nGrams);
}

int FrequencyCollector::collectNGrams(std::stringstream &buffer, NGrams &nGrams) {
	int n = nGrams.n;
	nGrams.count = 0;
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
		++nGrams.count;
		// 	Make sure it is uppercase
		queue[curPos++] = toupper(ch);
		curPos = curPos % n;
		// 	Reorder the chars starting with curPos and wrapping
		std::string ngram;
		for(int i = 0; i < n; i++) {
			ngram.push_back(queue[(curPos + i) % n]);
		}
		// 	Count how many times that n-gram occurs
		(*nGrams.freqs)[ngram] ++;
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
	
	std::string buffer;
	while(!fileReader.eof()) {
		std::getline(fileReader, buffer);
		++ numLines;
	}
	return numLines;
}
int FrequencyCollector::printNGrams(NGrams &nGrams) {
    for(auto it = nGrams.freqs->begin(); it != nGrams.freqs->end(); ++it) {
        std::cout << it->first << " " << it->second << '\n';
    }
    return 0;
}
