#include <string>
#include <random>
#include "PlayfairGenetic.hpp"
#include "PfHelpers.hpp"
#include "EnglishFitness.hpp"

using namespace PlayfairGenetic;

int main() {
   	FrequencyCollector standardFreq(2);
	char fileName[] = "frequencies/english_bigrams.txt";
	try {
		standardFreq.readNgramCount(fileName);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return -1;
	}

    EnglishFitness englishFit(standardFreq);

	vector<char> cipherText;
	char cFileName[] = "cipherText.txt";
	try {
		PfHelpers::readFile(cFileName, cipherText);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return -1;
	}
	
	GenParams genParams { 12, 4, 0, 0.2, 2, 1 };

	std::string seed = "Apple";
	pop_t population;
    rng_t rng(time(0));

	initializePopulationSeed(12, population, rng, seed);
	for(unsigned generation = 0; generation < 1000; generation++) {
		nextGeneration(englishFit, cipherText, genParams, population, rng);
	}
	vector<score_t> scores = fitScores(englishFit, population, cipherText);
	std::pair<string, score_t> best = bestMember(population, scores);
	std::cout << "After 1000 generations the best Key produced was: " <<
		best.first << ", with a fitness score: " << best.second << '\n';

	return 0;
}