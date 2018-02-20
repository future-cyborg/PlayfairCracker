#include "PlayfairGenetic.h"
#include "FrequencyCollector.h"
#include "Key.h"
#include "helpers.h"

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
	int numChildren		= 8;
	int newRandom 		= 2;
	double mutationRate = 0.1;
	int mutationType 	= 0; 
	int killWorst		= 2;

	int initialSize = 10;
	int numGens = 100;

	
	NGrams *standardFreq = new NGrams;
	standardFreq->n = 4;
	standardFreq->count = 0;
	standardFreq->freqs = new std::unordered_map<std::string, int>;

	//	Read cipher text
	vector<char> cipherText;
	char cFileName[] = "cText.txt";
	PfHelper::readFile(cFileName, cipherText);
	

	FrequencyCollector fC;
	char fileName[] = "frequencies/english_quadgrams.txt";
	fC.readNgramCount(fileName, *standardFreq);


	vector<string> population;
	std::mt19937 rng;

	PlayfairGenetic pG;
	pG.initializePopulationRandom(initialSize, population, rng);
	GenerationParams params { numChildren, newRandom, mutationRate, mutationType, killWorst };

	// Update it for a hundred generations, recording best for each generation
	// We need sample rates for what a good score might be.
	//	Use English text and make a bunch of random keys and compare the fitness scores.
	vector<std::pair<string, double>> bestScores;
	bestScores.reserve(numGens);
	for(int generation = 0; generation < numGens; ++generation) {
		pG.nextGeneration(*standardFreq, population, cipherText, params, rng);
		std::pair<string, double> bestIndex = pG.bestMember(*standardFreq, population, cipherText);
		bestScores.push_back(bestIndex);
		std::cout << "!" << std::endl;
	}
	for(int index = 0; index < (int)bestScores.size(); index++) {
		std::cout << bestScores.at(index).first << " " << bestScores.at(index).second << '\n';
	}

	return 0;
}