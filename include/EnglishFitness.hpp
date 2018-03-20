#ifndef ENGLISHFITNESS_HPP
#define ENGLISHFITNESS_HPP

#include "FrequencyCollector.hpp"

typedef double score_t;

class EnglishFitness {
public:
	EnglishFitness(const FrequencyCollector &standardFreq);
	~EnglishFitness();

	score_t fitness(const FrequencyCollector &testFreq) const;
	unsigned getN() const;
	score_t maxFitness(const FrequencyCollector &testFreq) const;

private:
	FrequencyCollector sFreq;
	unsigned n;

};

#endif // ENGLISHFITNESS_HPP