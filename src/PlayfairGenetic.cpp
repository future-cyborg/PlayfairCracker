#include "FrequencyCollector.hpp"
#include "Key.hpp"
#include "PlayfairGenetic.hpp"
#include "PfHelpers.hpp"

#define ALPHABET "ABCDEFGHIKLMNOPQRSTUVWXYZ"
#define INEFFECTIVE 100

using std::vector;
using std::unordered_map;
using std::string;
using std::stringstream;
using std::list;


PlayfairGenetic::PlayfairGenetic() {}
PlayfairGenetic::~PlayfairGenetic() {}


int PlayfairGenetic::initializePopulationRandom(int popSize, vector<string> &population, rng_t &rng) {
	population.clear();
	population.reserve(popSize);
	for(int i = 0; i < popSize; i++) {
		population.push_back(randomKey(rng));
	}
	return 0;
}


int PlayfairGenetic::initializePopulationSeed(int popSize, vector<string> &population, rng_t &rng, string seed) {
	population.clear();
	population.reserve(popSize);
	for(int i = 0; i < popSize; i++) {
		population.push_back(seedKey(rng, seed));
	}
	return 0;
}




int PlayfairGenetic::printPopulation(vector<string> &population) {
	for(int i = 0; i < (int)population.size(); i++) {
		std::cout << population[i] << '\n';
		vector<char> child(population[i].begin(), population[i].end());
		validKey(child);
	}
	return 0;
}

list<string> PlayfairGenetic::keepBest(const vector<string> &population, const vector<score_t> scores, const GenParams genParams) {
	if(population.size() != scores.size()) 
		throw InvalidParameters("Vector sizes do not match: population & scores");

	list<string> bestPop;
	list<std::pair<string, score_t>> pop;
	for(int index = 0; index < (int)scores.size(); index++) {
		pop.push_back(std::pair<string, score_t> (population.at(index), scores.at(index)));
	}


	for(int index = 0; index < genParams.keepBest; index++) {
		auto max = std::max_element(pop.begin(), pop.end(),
            [](const std::pair<string, score_t>& left, const std::pair<string, score_t>& right){
	            return left.second <  right.second;
	        });
		bestPop.push_back(max->first);
		pop.erase(max);
	}	

	return bestPop;
}

int PlayfairGenetic::nextGeneration(const FrequencyCollector &standardFreq, vector<string> &population, const vector<char> &cipherText, const GenParams &genParams, rng_t &rng) {
	//	get fitness scores for the population
	vector<score_t> scores = fitnessPopulation(standardFreq, population, cipherText);
	//	Kill off the worst
	for(int index = 0; index < genParams.killWorst; index++) {
		int worst = std::distance(scores.begin(), std::min_element(scores.begin(), scores.end()));
		population.erase(population.begin()+worst);
		scores.erase(scores.begin()+worst);
	}
	std::pair<int, int> parents = selectParents(scores, rng);
	
	string p1 = population.at(parents.first);
	string p2 = population.at(parents.second);

	list<string> bestPop = keepBest(population, scores, genParams);
	population.clear();
	population.push_back(p1);
	population.push_back(p2);

	try{
		crossover(population, genParams, rng);
	} catch(InvalidKeyException e) {
		std::cerr << "Crossover step produced an invalid key." << '\n';
		throw;
	}
	if(genParams.newRandom > 0) {
		vector<string> newPop;
		initializePopulationRandom(genParams.newRandom, newPop, rng);
		population.reserve(population.size() + newPop.size());
		population.insert(population.end(), newPop.begin(), newPop.end());
	}
	try{
		mutation(population, genParams, rng);
	} catch(InvalidKeyException e) {
		std::cerr << "Mutation step produced an invalid key." << '\n';
		throw;
	} catch(InvalidParameters e) {
		std::cerr << e.what() << '\n';
	}

	//	Add the best elements that we kept earlier
	auto bestMember = bestPop.begin();
	for(int index = 0; index < (int)bestPop.size(); index++) {
		population.push_back(*bestMember++);
	}
	return 0;
}

vector<score_t> PlayfairGenetic::scores(const FrequencyCollector &standardFreq, const vector<string> &population, const vector<char> &cipherText) {
	return fitnessPopulation(standardFreq, population, cipherText);
}

std::pair<string, score_t> PlayfairGenetic::bestMember(const FrequencyCollector &standardFreq, const vector<string> &population,
		const vector<char> &cipherText) {

	vector<score_t> scores = fitnessPopulation(standardFreq, population, cipherText);
	int best = std::distance(scores.begin(), std::max_element(scores.begin(), scores.end()));

	return std::pair<string, score_t> (population.at(best), scores.at(best));
}

string PlayfairGenetic::randomKey(rng_t &rng) {
	string alphabet = ALPHABET;
	std::shuffle(alphabet.begin(), alphabet.end(), rng);
	return alphabet;
}

string PlayfairGenetic::seedKey(rng_t &rng, string seed) {
	unordered_map<char, bool> letterUsed;
	string alphabet = ALPHABET;
	string key;
	for(int i = 0; i < (int)alphabet.length(); i++) {
		letterUsed[toupper(alphabet[i])] = false;
	}
	for(int i = 0; i < (int)seed.length(); i++) {
		char letter = toupper(seed[i]);
		if(isalpha(letter)) {
			if(!letterUsed[letter])
				key.push_back(letter);
		}
	}
	//  Might be different than seed.length() because doubles and non-letters are removed!
	int seedLength = key.length();
	for(auto it = letterUsed.begin(); it != letterUsed.end(); ++it) {
		if(!it->second)
			key.push_back(it->first);
	}

	string::iterator it = key.begin();
	std::advance(it, seedLength);   
	std::shuffle(it, key.end(), rng);
	return key;
}

std::pair<int, int> PlayfairGenetic::selectParents(const vector<score_t> scores, rng_t &rng) {
	// 	Update to parent selection. Subtracting lowestFitnessValue from all fitness scores,
	//		then we use their proportions
	vector<score_t> cpyScores = scores;
	score_t worst = cpyScores.at(std::distance(cpyScores.begin(), std::min_element(cpyScores.begin(), cpyScores.end())));
	for(int index = 0; index < (int)cpyScores.size(); index++) {
		cpyScores[index] = cpyScores[index] - worst;
	}

	score_t scoreSum = sumVector(cpyScores);

	//  Get a random double in range [0-scoreSum]
	std::uniform_real_distribution<score_t> uid(0,scoreSum);
	score_t pSelect = uid(rng);
	//  Select first parent
	int selection = -1;
	while(pSelect < scoreSum) {
		pSelect += cpyScores.at(++selection);
	}

	//  Select second parent
	scoreSum -= cpyScores.at(selection);
	int selection2 = 0;
	std::uniform_real_distribution<score_t> uid2(0,scoreSum);
	pSelect = uid2(rng);
	while(pSelect < scoreSum) {
		//  Don't pick the first parent twice
		if(selection2 != selection)
			pSelect += cpyScores.at(selection2);
		++selection2;
	}
	--selection2;

	std::pair<int, int> p(selection, selection2);
	return p;
}

vector<score_t> PlayfairGenetic::fitnessPopulation(const FrequencyCollector &standardFreq, const vector<string> &population, const vector<char> &cipherText) {
	int n = standardFreq.getN();
	vector<score_t> scores;
	FrequencyCollector fCollector(n);
	for(auto it = population.begin(); it != population.end(); ++it) {
		fCollector.clear();

		Key key(*it);
		vector<char> pText = key.decrypt(cipherText);
		stringstream pTextStream(string(pText.begin(), pText.end()));

		fCollector.collectNGrams(pTextStream);
		try {
			scores.push_back(fitness(standardFreq, fCollector));
		} catch(Exception e) {
			std::cerr << e.what() << '\n';
			throw;
		}
	}
	return scores;
}

score_t PlayfairGenetic::fitness(const FrequencyCollector &standardFreq, const FrequencyCollector testFreq) {
	// S = frequency of ngram for standard
	// T = frequency of ngram for test
	
	// frequency = 1 / sum{|S - T|^2}

	int n = standardFreq.getN();
	score_t fitness = 0;
	if(standardFreq.isEmpty() || testFreq.isEmpty()) {
		throw Exception("Error: A frequency map is empty");
	}
	if(testFreq.getN() != n) {
		throw Exception("Error: frequencies have different n value");
	}

	//  Iterate through all permutations of n letters
	//  perms[] is used like an odometer
	int *perms = new int[n];
	int max = std::pow(26,n);
	for(int i = 0; i < max; i++) {
		string perm;
		for(int index = 0; index < n; index++) {
			perm.push_back(65 + perms[index]);
		}
		//  Increment perms
		perms[0] = (perms[0] + 1) % 26;
		//  Check if 'odometer' rolls over
		for(int index = 1; index < n; index++) {
			if(perms[index - 1] == 0) {
				perms[index] = (perms[index] + 1) % 26;
			} else {
				break;
			}
		}
		
		//  Collect the standard and test rate of permutation
		double standardF = standardFreq.frequency(perm);
		double testF = testFreq.frequency(perm);

		//  Add to fitness score
		fitness += std::pow(std::abs(standardF - testF), 2);
	}
	delete[] perms;
	//  Right now, lower fitness is better. Let's take the inverse. Now higher is better.
	return 1 / fitness;
}

int PlayfairGenetic::crossover(vector<string> &population, const GenParams &genParams, rng_t &rng) {
	string p1 = population.at(0);
	string p2 = population.at(1);
	std::uniform_int_distribution<int> uid(0, 1);
	//	Child becomes a copy of p1
	//	Go through every letter and 50% chance to keep it or not
	//	Letters that aren't being used are added in order from p2
	for(int i = 0; i < genParams.numChildren; i++) {
		// 	If a letter exists in this map, it is being used
		unordered_map<char, bool> letterUsed;
		vector<char> key(p1.begin(), p1.end());
		vector<bool> letterKeep(key.size(), false);
		for(int index = 0; index < (int)key.size(); index++) {            
			if(uid(rng)) {
				letterKeep[index] = true;
				letterUsed.insert(std::make_pair(key[index], true));
			}
		}
		auto p2gene = p2.begin();
		for(int index = 0; index < (int)key.size(); index++) {
			if(!letterKeep[index]) {
				while(letterUsed.find(*p2gene) != letterUsed.end()) {
					++p2gene;
				}
				key[index] = *p2gene++;
			}
		}
		if(!validKey(key)) {
			throw InvalidKeyException("InvalidKeyException in crossover()");
		}
		string keyString(key.begin(), key.end());
		population.push_back(keyString);
	}
	return 0;
}

int PlayfairGenetic::mutation(vector<string> &population, const GenParams &genParams, rng_t &rng) {
	int index = 0;
	while(index < (int)population.size()) {
		string keyBefore, key = population[index];
		// Count how many times the mutation doesn't change the key
		int count = -1;
		while(keyBefore.compare(key) == 0) {
			// If mutation is ineffective this many times in a row, throw
			if(count++ > INEFFECTIVE) {
				string msg = "Mutation rate ineffective. INEFFECTIVE attempts to mutate key '";
				msg = msg + key + "' were unsuccessful.";
				throw Exception(msg.c_str());
			}

			switch(genParams.mutationType) {
				case SWAP: {
					swapMutation(key, genParams, rng);
					break;
				}
				case INVERSION: {
					inversionMutation(key, genParams, rng);					
					break;
				}
				default: {
					std::cerr << "Invalid mutationType: " << genParams.mutationType << '\n';
					throw InvalidParameters("Invalid Parameters: mutationType");
				}
			}
		}
		population[index] = key;
		++index;
	}
	return 0;
}

int PlayfairGenetic::swapMutation(string &key, const GenParams &genParams, rng_t &rng) {
	vector<char> swapLetters;
	vector<char> swapIndicies;
	std::uniform_real_distribution<double> urd(0.0, 1.0);
	for(int index = 0; index < (int)key.size(); index++) {
		double rand = urd(rng);
		if(rand < genParams.mutationRate) {
			swapLetters.push_back(key[index]);
			swapIndicies.push_back(index);
		}
	}
	std::shuffle(swapLetters.begin(), swapLetters.end(), rng);
	for(int i = 0; i < (int)swapIndicies.size(); i++) {
		key[swapIndicies[i]] = swapLetters[i];
	}
	if(!validKey(key)) {
		throw InvalidKeyException("InvalidKeyException in swapMutation()");
	}
	return 0;
}

int PlayfairGenetic::inversionMutation(string &key, const GenParams &genParams, rng_t &rng) {
	std::uniform_int_distribution<int> uid(0,key.size() - 1);
	int start = uid(rng);
	int end = uid(rng);
	if(start > end) {
		int temp = start;
		start = end;
		end = temp;
	}
	std::reverse(key.begin()+start, key.begin()+end);

	if(!validKey(key)) {
		throw InvalidKeyException("InvalidKeyException in inversionMutation())");
	}
	return 0;
}


template <typename Iterable>
bool PlayfairGenetic::validKey(Iterable key) {
	if(key.size() != 25) return false;

	unordered_map<char, int> letterUsed;
	for(int index = 0; index < (int)key.size(); index++) {
		//	If the letter has alreay been put in
		if(letterUsed.find(key[index]) != letterUsed.end()) {
			return false;
		}
		letterUsed.insert(std::make_pair(key[index], 0));
	}
	return true;
}

template <typename Number>
Number PlayfairGenetic::sumVector(vector<Number> vec) {
	Number total = 0;
	for(auto it = vec.begin(); it != vec.end(); ++it)
		total += *it;
	return total;
}