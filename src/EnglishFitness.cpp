#include "EnglishFitness.hpp"
#include "PfHelpers.hpp"
#include <algorithm>

EnglishFitness::EnglishFitness(const FrequencyCollector &standardFreq): sFreq{standardFreq}, n{standardFreq.getN()} {}
EnglishFitness::~EnglishFitness() {}

score_t EnglishFitness::fitness(const FrequencyCollector &testFreq) const {
	// S = frequency of ngram for standard
	// T = frequency of ngram for test
	
	// frequency = 1 / sum{|S - T|^2}

	unsigned n = sFreq.getN();
	score_t fitness = 0;
	if(sFreq.isEmpty() || testFreq.isEmpty()) {
		throw Exception("Error: A frequency map is empty");
	}
	if(testFreq.getN() != n) {
		throw Exception("Error: Frequencies have different n values");
	}

	//  Iterate through all permutations of n letters
	//  perms[] is used like an odometer
	int *perms = new int[n] ();
	int max = std::pow(26,n);
	for(int i = 0; i < max; i++) {
		string perm;
		for(unsigned index = 0; index < n; index++) {
			perm.push_back(65 + perms[index]);
		}
		//  Increment perms
		perms[0] = (perms[0] + 1) % 26;
		//  Check if 'odometer' rolls over
		for(unsigned index = 1; index < n; index++) {
			if(perms[index - 1] == 0) {
				perms[index] = (perms[index] + 1) % 26;
			} else {
				break;
			}
		}
		
		//  Collect the standard and test rate of permutation
		double standardF = sFreq.frequency(perm);
		double testF = testFreq.frequency(perm);

		//  Add to fitness score
		fitness += std::pow(std::abs(standardF - testF), 2);
	}
	delete[] perms;
	//  Right now, lower fitness is better. Let's take the inverse. Now higher is better.
	return 1 / fitness;
}

unsigned EnglishFitness::getN() const {
	return n;
}

score_t EnglishFitness::maxFitness(const FrequencyCollector &testFreq) const {
	count_t testCount = testFreq.getCount();
	count_t standardCount = sFreq.getCount();

	count_t result = testCount > standardCount ? testCount : standardCount;
	return (result * result) / 2.0; 
}