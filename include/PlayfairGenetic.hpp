#ifndef PLAYFAIRGENETIC_HPP
#define PLAYFAIRGENETIC_HPP

#include "EnglishFitness.hpp"
#include "Key.hpp"
#include "pcg_random.hpp"
#include <list>

using std::vector;
using std::string;

/** Random number generator type */
typedef pcg32 rng_t;
/** Population type */
typedef vector<string> pop_t;

/**
 * The mutation function to be used. Each child is guaranteed to be mutated.
 */
enum MutationType {
	/**
	 * The swap mutation chooses select letters in the key and shuffles them.
	 * The mutationRate affects the chance that a letter will be one that is shuffled.
	 */
	SWAP,
	/**
	 * A random substring of the key is inversed. mutationRate has no effect.
	 */
	INVERSION
};

/**
 * @brief Parameters applied to each generation.
 * 
 * GenParams holds the parameters for this genetic algorithm. These values are applied
 * 	to each generation.
 */
struct GenParams {
	/** The number of children produced */
	unsigned numChildren;
	/** The number of randomized keys added to population */
	unsigned newRandom;
	/** see mutationType */
	unsigned mutationType;
	/**
	 * Mutation is applied to each individual.
	 * The effect of this rate depends on mutationType
	 * @attention Must be in range (0.0 - 1.0)
	 */
	double mutationRate;
	/**
	 * The number of worst population members to be killed before the parent
	 * 	selection step. Members have a proportional chance of being selected as
	 * 	a parent. The ones we kill off have no chance.
	 */
	unsigned killWorst;
	/**
	 * The number of best population members to continue to the next generation.
	 * 	Aside from these best, only the parents and children continue.
	 */
	unsigned keepBest;
};

/**
 * @namespace PlayfairGenetic
 * @brief A genetic algorithm to crack English Playfair encryptions.
 * 
 * A genetic algorithm to crack English Playfair encryptions. Uses the GenParams
 * 	struct for options.
 * 	
 * This class provides functionality for initializing population and progressing a
 * 	generation, along with some helpers. This class does not manage the conditions
 * 	determining when the algorithm will stop.
 * 
 * @b Example:
 * @include example_genetic.cpp
 */
namespace PlayfairGenetic {
	/**
	 * @brief Initialize a population randomly
	 * 
	 * Initializes a population of random keys
	 * 
	 * @param popSize 		Size of population to be built
	 * @param population 	Reference to population, will be cleared and resized
	 * @param rng 			Reference to random number generator
	 * @return 				Reference to population
	 */
	pop_t& initializePopulationRandom(unsigned popSize, pop_t &population, rng_t &rng);

	/**
	 * @brief Initialize a population from a seed
	 * 
	 * Initialize a population from a seed. All members will start with the given seed,
	 * 	with the remaining letters randomized.
	 * 	
	 * For example: If seed is 'Apple', each member will start with 'APLE'.
	 * 
	 * @param popSize 		Size of population to be built
	 * @param population 	Reference to population, will be cleared and resized
	 * @param rng 			Reference to random number generator
	 * @param seed 			Seed for each member
	 * @return 				Reference to population
	 */
	pop_t& initializePopulationSeed(unsigned popSize, pop_t &population, rng_t &rng, string seed);

	/**
	 * @brief Produce the next generation
	 * 
	 * Produce the next generation. Uses GenParams as options to do the following steps:
	 * 
	 * 1. Get fitness scores for each member, using decrypted cipherText
	 * 2. Kill off worst members
	 * 3. Copy and save best members
	 * 4. Select two different members to be the parents, kill off everyone else
	 * 5. Produce children
	 * 6. Mutate parents and children
	 * 7. Add random keys to population
	 * 8. Add best members from step 3 to population
	 * 
	 * @param englishFit 	Reference to EnglishFitness class to be used for fitness function 
	 * @param cipherText 	Reference to the cipherText
	 * @param genParams 	Reference to GenParams
	 * @param population 	Reference to population
	 * @param rng 			Reference to random number generator
	 * @return 				Reference to population
	 */
	pop_t& nextGeneration(const EnglishFitness &englishFit, const vector<char> &cipherText,
			const GenParams &genParams,	pop_t &population, rng_t &rng);

	/**
	 * @brief Get the key and score for the most fit member
	 * 
	 * Get the key and score for the most fit member of the population
	 * 
	 * @param population 	Reference to population
	 * @param scores 		Reference to fitness scores
	 * 
	 * @return 				Reference to population
	 */
	std::pair<string, score_t> bestMember(const pop_t &population, 
			const vector<score_t> &scores);

	/**
	 * @brief Calculate the fitness scores for a population
	 * 
	 * Calculate the fitness scores for a population with the given cipherText
	 * 
	 * @param englishFit 	Reference to EnglishFitness class to be used for fitness function
	 * @param population 	Reference to population
	 * @param cipherText 	Reference to the cipherText
	 * @return 				Reference to population
	 */
	vector<score_t> fitScores(const EnglishFitness &englishFit, const pop_t &population, 
			const vector<char> &cipherText);
}	

#endif // PLAYFAIRGENETIC_HPP