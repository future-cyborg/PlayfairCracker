#include "PlayfairGenetic.h"
#include "FrequencyCollector.h"
#include "Key.h"
#include "PfHelpers.h"

#include <iostream>
#include <fstream>

// struct NGrams {
//     const int n;
//     int count;
//     std::unordered_map<std::string, int> freqs;
// };

// enum MutationType { SWAP, INVERSION };

// struct GenerationParams {
// 	int numChildren;
// 	int newRandom;
// 	double mutationRate;
// 	int mutationType;
// 	int killWorst;
// };


using std::vector;
using std::string;

int main() {
	PfHelpers::Timer timer;
	int numChildren		= 8;
	int newRandom 		= 2;
	double mutationRate = 0.1;
	int mutationType 	= 0; 
	int killWorst		= 2;

	int initialSize = 10;
	int numGens = 10;

	// Initialize standardFreq
	NGrams *standardFreq = new NGrams;
	standardFreq->n = 2;
	standardFreq->count = 0;
	standardFreq->freqs = new std::unordered_map<std::string, count_t>;
	// Get standardFrequencies
	FrequencyCollector fC;
	char fileName[] = "frequencies/english_bigrams.txt";
	try {
		fC.readNgramCount(fileName, *standardFreq);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return -1;
	}

	//	Read cipher text
	vector<char> cipherText;
	char cFileName[] = "cText.txt";
	try {
		PfHelpers::readFile(cFileName, cipherText);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return -1;
	}
	// Initialize 
	vector<string> population;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 rng(seed);
	PlayfairGenetic pG;
	pG.initializePopulationRandom(initialSize, population, rng);
	GenerationParams params { numChildren, newRandom, mutationRate, mutationType, killWorst };

	// //	Outfile
	// char scoresFileName[] = "scores.txt";
	// std::ofstream scoresFile(scoresFileName);
	// if(!scoresFile) {
	// 	std::cerr << "Could not open " << scoresFileName << '\n';
	// 	return -1;
	// }
	// char keyFileName[] = "keys.txt";
	// std::ofstream keysFile(keyFileName);
	// if(!keysFile) {
	// 	std::cerr << "Could not open " << keyFileName << '\n';
	// 	return -1;
	// }

	// We need sample rates for what a good score might be.
	//	Use English text and make a bunch of random keys and compare the fitness scores.
	vector<std::pair<string, double>> bestScores;
	bestScores.reserve(numGens);
	for(int generation = 0; generation < numGens; ++generation) {
		pG.nextGeneration(*standardFreq, population, cipherText, params, rng);
		std::pair<string, double> bestIndex = pG.bestMember(*standardFreq, population, cipherText);
		bestScores.push_back(bestIndex);

		std::cout << "#" << std::endl;
	}
	for(int index = 0; index < (int)bestScores.size(); index++) {
		std::cout << bestScores.at(index).first << " " << bestScores.at(index).second << '\n';
	}

	std::cout << "Timer: " << timer.elapsed() << " seconds" << '\n';

	return 0;
}