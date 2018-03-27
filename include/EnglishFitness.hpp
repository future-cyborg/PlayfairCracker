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