#ifndef PLAYFAIRGENETIC_H
#define PLAYFAIRGENETIC_H

#include "FrequencyCollector.h"
#include "Key.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <iterator>

using std::vector;
using std::unordered_map;
using std::string;
using std::stringstream;

class InvalidKeyException {
public:
   InvalidKeyException(const char* msg = "InvalidKeyException") : e_msg(msg) {}
  ~InvalidKeyException( ) {}

   const char* getMessage( ) const {return(e_msg);}
private:
   const char* e_msg;
};


struct GenerationParams {
	int numChildren;
	int newRandom;
	int mutationRate;
	int mutationType;
	int killWorst;
};

class PlayfairGenetic {
public:
	PlayfairGenetic();
	~PlayfairGenetic();

	int initializePopulationRandom(int popSize, vector<string> &population,
			std::mt19937 &rng);

	int initializePopulationSeed(int popSize, vector<string> &population,
			std::mt19937 &rng, string seed);

	int nextGeneration(const NGrams &standardFreq, vector<string> &population,
			const vector<char> &cipherText,	const GenerationParams &genParams,
			std::mt19937 rng);

	int printPopulation(vector<string> &population);

private:

	string randomKey(std::mt19937 &rng);

	string seedKey(std::mt19937 &rng, string seed);


	std::pair<int, int> selectParents(const NGrams &standardFreq,
			vector<string> &population,	const vector<char> &cipherText,
			const GenerationParams &genParams, std::mt19937 rng);

	vector<int> fitnessPopulation(const NGrams &standardFreq,
			const vector<string> &population, const vector<char> &cipherText);

	int fitness(const NGrams &standardFreq, const NGrams testFreq);

	
	int crossover(vector<string> &population, const GenerationParams &genParams,
			std::mt19937 rng);

	
	int mutation(vector<string> &population, const GenerationParams &genParams,
			std::mt19937 rng);

	int swapMutation(string &key, const GenerationParams &genParams,
			std::mt19937 rng);

	int inversionMutation(string &key, const GenerationParams &genParams,
			std::mt19937 rng);

	template <typename T>
	bool validKey(T key);
	// bool validKey(string key);


	int sumVectorInt(vector<int> vec);


	std::ifstream::pos_type fileSize(const string fileName);

	int readFile(const string fileName, vector<char> &text);


}


#endif // PLAYFAIRGENETIC_H