/* PlayfairCracker - Crack Playfair Encryptions
 * Copyright (C) 2018 Yesha Maggi
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
	if(!fitness) {
		// Can't divide by 0
		return 0;
	}
	//  Right now, lower fitness is better. Let's take the inverse. Now higher is better.
	return 1 / fitness;
}

unsigned EnglishFitness::getN() const {
	return n;
}

score_t EnglishFitness::maxFitness(const FrequencyCollector &testFreq) const {
	count_t testCount = testFreq.getCount();
	count_t standardCount = sFreq.getCount();

	return 1/(2*std::pow(2*std::abs((1.0/testCount)-(1.0/standardCount)), 2));
}