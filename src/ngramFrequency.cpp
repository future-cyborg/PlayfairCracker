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

#include "FrequencyCollector.hpp"
#include "optionparser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/ioctl.h>
#include <vector>

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
        if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
        if (endptr != option.arg && *endptr == 0)
          return option::ARG_OK;
        if (msg) printError("Option '", option, "' requires a numeric argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus NonEmpty(const option::Option& option, bool msg) {
        if (option.arg != 0 && option.arg[0] != 0)
          return option::ARG_OK;

        if (msg) printError("Option '", option, "' requires a non-empty argument\n");
        return option::ARG_ILLEGAL;
    }
};

std::ifstream::pos_type fileSize(const std::string fileName) {
    std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

enum  optionIndex { UNKNOWN, HELP, METHOD, TEXT, OUTPUTFILE, N };
enum  method { COLLECT, VALID };
const option::Descriptor usage[] = {
{ UNKNOWN,   0,"",  "",       Arg::Unknown, "USAGE: ngramFrequency [OPTION]... -n N FILE...\n"
                                            "       ngramFrequency [OPTION]... -n N -t TEXT\n"
                                            "       ngramFrequency [OPTION]... -v -n N FILE..."},
{ N,         0,"n", "n",      Arg::Numeric, "\nPARAMETERS:\n"
                                            "  -n,        \t"
                                            "\tN, the size of n-gram being worked with (i.e. 2 = bigram)"},
{ HELP,      0,"",  "help",   Arg::None,    "\nOPTIONS:\n"
                                            "             \t--help"
                                            "\tPrint usage and exit"},
{ METHOD,    0,"c", "collect",Arg::None,    "  -c,        \t--collect (default mode)"
                                            "\tRead file and collect n-gram frequencies"},
{ METHOD,    1,"v", "valid",  Arg::None,    "  -v,        \t--valid"
                                            "\tChecks if file contains valid n-gram counts" },
{ TEXT,      0,"t", "text",   Arg::NonEmpty,"  -t <FILE>, \t--text=<TEXT>"},
{ OUTPUTFILE,0,"o", "output", Arg::NonEmpty,"  -o <FILE>, \t--output=<FILE>"},
{ UNKNOWN,   0,"",  "",       Arg::None,
 "EXAMPLES:\n"
 "  ngramFrequency -n 2 -t \"collect n-gram frequencies!\"\n"
 "  ngramFrequency -c -n 3 -o trigrams.txt english.txt\n"
 "  ngramFrequency -v -n 4 trigrams.txt\n" },
{ 0, 0, 0, 0, 0, 0 } };

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

    if(!options[N]) {
        fprintf(stderr, "Usage requires -n flag.\n");
        fprintf(stderr, "Try 'ngramFrequency --help' for more information.\n");
        return 1;
    }
    unsigned n = atoi(options[N].last()->arg);

    if(!options[TEXT] && (parse.nonOptionsCount() == 0) ) {
        fprintf(stderr, "Usage requires either -t flag or file argument.\n");
        fprintf(stderr, "Try 'ngramFrequency --help' for more information.\n");
        return 1;
    }

    try {
        FrequencyCollector fC(n);

        if(options[METHOD] && options[METHOD].last()->type() == VALID) {
            if(options[TEXT]) {
                fprintf(stderr, "Validation method does not work with -t flag.\n");
                fprintf(stderr, "Try 'ngramFrequency --help' for more information.\n");
                return 1;
            }
            if(parse.nonOptionsCount() == 0) {
                fprintf(stderr, "Validation method requires file argument.\n");
                fprintf(stderr, "Try 'ngramFrequency --help' for more information.\n");
                return 1;
            }
            for(int i = 0; i < parse.nonOptionsCount(); i++) {
                const char* inputFile = parse.nonOption(i);
                if(fC.validNgramFile(inputFile)) {
                    fprintf(stdout, "%s is a valid n-gram file for n = %d\n", inputFile, n);
                } else {
                    fprintf(stdout, "%s is not a valid n-gram file for n = %d\n", inputFile, n);
                }
            }
            return 0;            
        }

        if(options[TEXT]) {
            std::stringstream ss(options[TEXT].last()->arg);
            fC.collectNGrams(ss);
        }
        
        for(int i = 0; i < parse.nonOptionsCount(); i++) {
            fC.collectNGramsFile(parse.nonOption(i));
        }
            

        if(options[OUTPUTFILE]) {
            fC.writeNGramCount(options[OUTPUTFILE].last()->arg);
        } else {
            fC.printNGrams(std::cout);
        }

    } catch(std::exception e) {
        fprintf(stderr, "%s", e.what());
    }

    return 0;
}
