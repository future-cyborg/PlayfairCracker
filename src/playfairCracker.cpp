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

#include "PlayfairGenetic.hpp"
#include "FrequencyCollector.hpp"
#include "PfHelpers.hpp"
#include "optionparser.h"
#include <random>
#include <sys/ioctl.h>

using std::vector;
using std::string;

struct Arg: public option::Arg {
    static void printError(const char* msg1, const option::Option& opt, const char* msg2) {
        fprintf(stderr, "%s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
    }

    static option::ArgStatus Unknown(const option::Option& option, bool msg) {
        if (msg) printError("Unknown option '", option, "'\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Numeric(const option::Option& option, bool msg) {
        char* endptr = 0;
        long num;
        if (option.arg != 0) {
        	num = strtol(option.arg, &endptr, 10);
        }
        if (endptr != option.arg && *endptr == 0) {
        	if(num >= 0) {
        		return option::ARG_OK;
        	}
          if(msg) printError("Option '", option, "'requires a non-negative number\n");
        } else if(msg) printError("Option '", option, "' requires a numeric argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Rate(const option::Option& option, bool msg) {
    	char* endptr = 0;
    	double num;
    	if (option.arg != 0) {
    		num = strtod(option.arg, &endptr);
    	}
    	if (endptr != option.arg && *endptr == 0) {
    		if(num >= 0 && num <= 1) {
    			return option::ARG_OK;
    		}
    		if(msg) printError("Option '", option, "' requires a value between 0.0 and 1.0\n");
    	} else if(msg) printError("Option '", option, "' requires a numeric argument\n");
    	return option::ARG_ILLEGAL;
    }

    static option::ArgStatus NonEmpty(const option::Option& option, bool msg) {
        if (option.arg != 0 && option.arg[0] != 0)
          return option::ARG_OK;

        if (msg) printError("Option '", option, "' requires a non-empty argument\n");
        return option::ARG_ILLEGAL;
    }
};

enum  optionIndex { UNKNOWN, HELP, METHOD, OUTFILE, VERBOSE, PARAMS, SEED, RNG,
	CHILDS, RANDOM, MUTATION, MUTRATE, KILL, BEST };
enum  method { GENS, DORM };
const option::Descriptor usage[] = {
{ UNKNOWN,  0,"",  "",       Arg::Unknown,  "USAGE: playfairCracker -g NUM [OPTION]... CIPHER FREQ\n"
                                            "       playfairCracker -s NUM [OPTION]... CIPHER FREQ\n"
                                        	"\nDESCRIPTION: See full documentation for more details.\n"
                                        	"	CIPHER is the file containing the cipherText\n"
                                        	"	FREQ is the file containing the standard n-gram frequencies"},
{ METHOD,   0,"g", "g",      Arg::Numeric,  "\nMETHODS: (See documentation for details)\n"
										    "  -g <NUM>, \t--g=<NUM>"
										    "\tAlgorithm runs for NUM generations"},
{ METHOD,   1,"d", "d",      Arg::Numeric,  "  -d <NUM>, \t--d=<NUM>"
										    "\tAlgorithm runs until dormant for NUM generations"},
{ HELP,     0,"",  "help",   Arg::None,     "\nOPTIONS:\n"
										    "			 \t--help"
                                            "\tPrint usage and exit"},
{ PARAMS,	0,"p", "params", Arg::NonEmpty, "  -p <FILE>,\t--params=<FILE>"
											"\tFile with parameters. See documentation"},
{ SEED,     0,"s",  "seed",  Arg::NonEmpty,  "  -s <ARG>, \t--seed=<ARG>"
											"\tSeed to initialize population with" },
{ VERBOSE,	0,"v", "verbose",Arg::None,     "  -v,       \t--verbose"},
{ CHILDS,	0,"c", "children",Arg::Numeric, "\nPARAMETERS: These take precedence over params file\n"
											"  -c <NUM>, \t--children=<NUM>"
											"\tNUM children produced each generation"},
{ RANDOM,	0,"a", "add",    Arg::Numeric,  "  -a <NUM>, \t--add=<NUM>"
											"\tNUM random members added each generation"},
{ MUTATION,	0,"m", "mutation",Arg::Numeric, "  -m <NUM>, \t--mutation=<NUM>"
											"\tMutation type, see documentation"},
{ MUTRATE,	0,"r", "rate",	 Arg::Rate,     "  -r <NUM>, \t--rate=<NUM>"
											"\tMutation rate, see documentation"},
{ KILL,  	0,"k", "kill",	 Arg::Numeric,  "  -k <NUM>, \t--kill=<NUM>"
											"\tNUM worst members of population killed before parent selection"},
{ BEST, 	0,"b", "best",	 Arg::Numeric,  "  -b <NUM>, \t--best=<NUM>"
											"\tNUM best members that will continue next generation"},
{ RNG,      0,"",  "rng",    Arg::Numeric, 0 },
{ 0, 0, 0, 0, 0, 0 } };

bool setParam(std::unordered_map<string, string> &paramMap, unsigned &param, string s) {
	if(paramMap.find(s) != paramMap.end()) {
		string value = (*paramMap.find(s)).second;
		if(!PfHelpers::isInteger(value)) {
			fprintf(stderr, "'%s' in parameters file requires an integer value.\n", s.c_str());
			fprintf(stderr, "See documentation for more details.\n");
			return false;
		}
		param = strtoul(value.c_str(), NULL, 10);
	} else {
		fprintf(stderr, "No parameter '%s' found in parameters file.\n", s.c_str());
		fprintf(stderr, "See documentation for more details.\n");
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
    argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
    option::Stats stats(usage, argc, argv);

    std::vector<option::Option> options(stats.options_max);
    std::vector<option::Option> buffer(stats.buffer_max);
    option::Parser parse(usage, argc, argv, &options[0], &buffer[0], 3);

    if(parse.error())
        return 1;

    if(options[HELP] || argc == 0) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        int columns = w.ws_col ? w.ws_col : 80;
        option::printUsage(fwrite, stdout, usage, columns);
        return 0;
    }

    unsigned verbose = options[VERBOSE].count();

    if(!options[METHOD]) {
        fprintf(stderr, "Usage requires either -g or -s flag flag.\n");
        fprintf(stderr, "See documentation for more details.\n");
        return 1;
    }

    if(parse.nonOptionsCount() != 2) {
    	fprintf(stderr, "Usage requires exactly 2 arguments.\n");
    	return 1;
    }

    // Parameters
	unsigned children;
	unsigned addRandom;
	unsigned mutationType;
	double mutationRate;
	unsigned killWorst;
	unsigned keepBest;
	std::unordered_map<string, string> paramMap;

    if(!options[PARAMS]) {
    	if(!options[CHILDS] && !options[RANDOM] && !options[MUTATION] &&
    			!options[MUTRATE] && !options[KILL] && !options[BEST]) {

    		fprintf(stderr, "Usage requires parameters file or all parameter flags.\n");
    		fprintf(stderr, "See documentation for more details.\n");
    		return 1;
    	}
    } else {
    	//	Get params from parameters file
    	const char *fileName = options[PARAMS].last()->arg;
    	std::ifstream fileReader(fileName);
        if(!fileReader) {
            fprintf(stderr, "%s can not be opened.\n", fileName);
            return 2;
        }
        while(fileReader) {
        	string line, param, value;
        	getline(fileReader, line);
        	std::size_t pos = line.find('=');
        	if(pos != string::npos) {
	        	param = line.substr(0, pos);
	        	value = line.substr(pos + 1, string::npos);
	        	paramMap.insert(std::make_pair(param, value));
	        }
        }
    }

    if(!options[CHILDS]) {
	    if(!setParam(paramMap, children, "children"))
	    	return 3;
	} else children = strtoul(options[CHILDS].last()->arg, NULL, 10);

	if(!options[RANDOM]) {
	    if(!setParam(paramMap, addRandom, "addRandom"))
	    	return 3;
	} else addRandom = strtoul(options[RANDOM].last()->arg, NULL, 10);

	if(!options[MUTATION]) {
	    if(!setParam(paramMap, mutationType, "mutationType"))
	    	return 3;
	} else mutationType = strtoul(options[MUTATION].last()->arg, NULL, 10);

	if(!options[KILL]) {
	    if(!setParam(paramMap, killWorst, "killWorst"))
	    	return 3;
	} else killWorst = strtoul(options[KILL].last()->arg, NULL, 10);

	if(!options[BEST]) {
	    if(!setParam(paramMap, keepBest, "keepBest"))
	    	return 3;
	} else keepBest = strtoul(options[BEST].last()->arg, NULL, 10);

	if(!options[MUTRATE]) {
		if(paramMap.find("mutationRate") != paramMap.end()) {
			string value = (*paramMap.find("mutationRate")).second;
			if(!PfHelpers::isRate(value)) {
				fprintf(stderr, "'mutationRate' in parameters file requires a value between 0.0 and 1.0\n");
				fprintf(stderr, "See documentation for more details.\n");
				return 3;
			}
			mutationRate = strtod(value.c_str(), NULL);
		} else {
			fprintf(stderr, "No parameter 'mutationRate' found in parameters file.\n");
			fprintf(stderr, "See documentation for more details.\n");
			return 3;
		}
	} else mutationRate = strtod(options[MUTRATE].last()->arg, NULL);

	GenParams params { children, addRandom, mutationType, mutationRate, killWorst, keepBest };


	unsigned n;
	try {
		std::ifstream fileReader(parse.nonOption(1));
		if(!fileReader) {
            fprintf(stderr, "%s can not be opened.\n", parse.nonOption(1));
            return 2;
        }
       	fileReader.ignore(50, ' ');
       	n = fileReader.gcount() - 1;
	} catch(std::ios_base::failure e) {
		fprintf(stderr, "Error with frequency file '%s'", parse.nonOption(1));
		return 2;
	}

	// Initialize standardFreq
	// Get standardFrequencies
	FrequencyCollector standardFreq(n);
	char fileName[] = "frequencies/english_bigrams.txt";
	try {
		standardFreq.readNgramCount(fileName);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return 2;
	}
	EnglishFitness englishFit(standardFreq);

	//	Read cipher text
	vector<char> cipherText;
	try {
		PfHelpers::readFile(parse.nonOption(0), cipherText);
	} catch (std::ifstream::failure e) {
		std::cerr << e.what() << '\n';
		return 2;
	}

	// RNG
	pcg_extras::seed_seq_from<std::random_device> seed_source;
    rng_t rng(seed_source);
    if(options[RNG]) {
    	rng.seed(strtol(options[RNG].last()->arg, NULL, 10));
    }

	// Initialize population
	pop_t population;
	unsigned initialSize = 2 + children + addRandom;
	if(options[SEED]) {
		PlayfairGenetic::initializePopulationSeed(initialSize, population, rng, options[SEED].last()->arg);
	} else {
		PlayfairGenetic::initializePopulationRandom(initialSize, population, rng);
	}
	
	unsigned generation = 0;
	unsigned numGens = 0;
	unsigned dormant = 0;
	unsigned numDorm = 0;
	score_t lastBest = 0;

	if(options[METHOD].last()->type() == GENS)
		numGens = strtol(options[METHOD].last()->arg, NULL, 0);
	else
		numDorm = strtol(options[METHOD].last()->arg, NULL, 0);
	
	PfHelpers::Timer timer;
	while(true) {
		++generation;
		if(numGens && generation > numGens) {
			break;
		}
		if(numDorm && dormant > numDorm) {
			break;
		}

		PlayfairGenetic::nextGeneration(englishFit, cipherText, params, population, rng);
		if(verbose || numDorm) {
			// Print each member and scores
			vector<score_t> scores = PlayfairGenetic::fitScores(standardFreq, population, cipherText);
			std::pair<string, score_t> bestIndex = PlayfairGenetic::bestMember(population, scores);
			if(verbose) {
				std::cout << "Generation " << generation << '\n';
				if(verbose > 1) {
					for(unsigned index = 0; index < population.size(); index++) {
						std::cout << population.at(index) << "  " << scores.at(index) << '\n';
					}
				}
				std::cout << "Best member: " << bestIndex.first << "  " << bestIndex.second << "\n\n";
			}

			if(lastBest == bestIndex.second) {
				++dormant;
			} else {
				dormant = 0;
				lastBest = bestIndex.second;
			}
		}
	}

	vector<score_t> scores = PlayfairGenetic::fitScores(standardFreq, population, cipherText);
	std::pair<string, double> bestIndex = PlayfairGenetic::bestMember(population, scores);
	std::cout << "Finished after " << generation << " generations\n";
	std::cout << "Best member: " << bestIndex.first << "  " << bestIndex.second << "\n";

	std::cout << "Timer: " << timer.elapsed() << " seconds" << '\n';
	return 0;
}