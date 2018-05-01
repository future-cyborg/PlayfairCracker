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

/**
 * @brief Rate how likely a text is English
 * 
 * Rate the fitness of how likely a text is English, or given frequencies.
 * 
 * @param standardFreq The standard n-gram frequencies the fitnessfunction is compared against
 */
class EnglishFitness {
public:
	/**
	 * @param standardFreq  The standard frequencies the fitnessfunction is compared against
	 */
	EnglishFitness(const FrequencyCollector &standardFreq);
	~EnglishFitness();

	/**
	 * @brief Collect fitness score for given n-gram frequencies
	 * 
	 * Collect fitness score for given n-gram frequencies.
	 * 
	 * @param testFreq  The test frequencies fintess is collected for
	 * @return score_t
	 */
	score_t fitness(const FrequencyCollector &testFreq) const;
	/**
	 * @brief Return n-gram size associated with this object
	 * 
	 * Return n-gram size associated with this object.
	 * 
	 * @return unsigned int
	 */
	unsigned getN() const;
	/**
	 * @brief Return the maximum fitness score possible
	 * 
	 * Return the maximum fitness score possible for this set of frequencies.
	 * 	Defined by
	 * 	
	 * 	( max(standardFreq, testFreq)^2 ) / 2
	 * 
	 * @param testFreq The test frequency maxFitness is collected for
	 * @return score_t
	 */
	score_t maxFitness(const FrequencyCollector &testFreq) const;

private:
	FrequencyCollector sFreq;
	unsigned n;

};

#endif // ENGLISHFITNESS_HPP