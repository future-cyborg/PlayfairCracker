/* PlayfairCipher - Encrypt/Decrypt using Playfair Cipher
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

#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include "Key.hpp"
#include "optionparser.h"

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

enum  optionIndex { UNKNOWN, HELP, METHOD, KEY, INPUTFILE, OUTPUTFILE, BIGRAM, INSERT, PAD, SKIP, REPLACE };
enum  encrypt { ENCRYPT, DECRYPT };
const option::Descriptor usage[] = {
{ UNKNOWN,   0,"",  "",       Arg::Unknown, "USAGE: playfair [OPTION]... TEXT\n"
                                            "       playfair [OPTION]... -f FILE"},
{ HELP,      0,"",  "help",   Arg::None,    "\nOPTIONS:\n"
                                            "            \t--help"
                                            "\tPrint usage and exit"},
{ METHOD,    0,"e", "encrypt",Arg::None,    "  -e,          \t--encrypt (default mode)"},
{ METHOD,    1,"d", "decrypt",Arg::None,    "  -d,          \t--decrypt"},
{ KEY,       0,"k", "key",    Arg::NonEmpty,"  -k <KEY>,    \t--key=<KEY>"},
{ KEY,       0,"",  "keyword",Arg::NonEmpty,"               \t--keyword=<KEY>"},
{ INPUTFILE, 0,"f", "file",   Arg::NonEmpty,"  -f <KEY>,    \t--file=<FILE>"},
{ OUTPUTFILE,0,"o", "output", Arg::NonEmpty,"  -o <FILE>,   \t--output=<FILE>"},
{ BIGRAM,    0,"b", "bigram", Arg::None,    "  -b,          \t--bigram"
                                            "\tPrint output as bigrams (e.g. BI GR AM)"},
{ INSERT,    0,"i", "insert", Arg::NonEmpty,"\nADVANCED OPTIONS:\n"
                                            "  -i <LETTER>, \t--insert=<LETTER>"
                                            "\tLetter to insert between double letters (default Q)"},
{ PAD,       0,"p", "pad",    Arg::NonEmpty,"  -p <LETTER>, \t--pad=<LETTER>"
                                            "\tLetter to pad odd length message (default X)"},
{ SKIP,      0,"s", "skip",   Arg::NonEmpty,"  -s <LETTER>, \t--skip=<LETTER>"
                                            "\tLetter that is skipped/replaced in message (default J)"},
{ REPLACE,   0,"r", "replace",Arg::NonEmpty,"  -r <LETTER>, \t--replace=<LETTER>"
                                            "\tLetter that replaces the skipped letter (default I)"},
{ UNKNOWN,   0,"",  "",       Arg::None,
 "\nEXAMPLES:\n"
 "  playfair -e -k keyword -o cipher.txt -f plain.txt \n"
 "  playfair -e -k keywork \"Encrypt this text\"\n"
 "  playfair -d cipher.txt \n"
 "  playfair -d -k keyword -o plain.txt cipher.txt\n"
 "\nIf no key is provided, that's a boring Playfair square!"
},
{ 0, 0, 0, 0, 0, 0 } };

int main(int argc, char* argv[]) {
    argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
    option::Stats stats(usage, argc, argv);

    std::vector<option::Option> options(stats.options_max);
    std::vector<option::Option> buffer(stats.buffer_max);
    option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

    if(parse.error())
        return 1;

    if(options[HELP] || argc == 0)  {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        int columns = w.ws_col ? w.ws_col : 80;
        option::printUsage(fwrite, stdout, usage, columns);
        return 0;
    }

    if(!options[INPUTFILE] && (parse.nonOptionsCount() == 0) ) {
        fprintf(stderr, "Usage requires either input file or text.\n");
        fprintf(stderr, "Try 'playfair --help' for more information.\n");
        return 1;
    }

    //  Open before reading input text, so can exit more quickly if error opening file
    std::ofstream outFile;
    std::ostream* output = &std::cout;
    if(options[OUTPUTFILE]) {
        outFile.open(options[OUTPUTFILE].last()->arg);
        if(!outFile) {
            fprintf(stderr, "%s cannot be opened for writing", options[OUTPUTFILE].last()->arg);
            return 3;
        }
        output = &outFile;
    }

    //  Collect text
    std::vector<char> text;
    if(options[INPUTFILE]) {
        const char *fileName = options[INPUTFILE].last()->arg;
        text.reserve(fileSize(fileName));
        std::ifstream fileReader(fileName);
        if(!fileReader) {
            fprintf(stderr, "%s can not be opened.\n", fileName);
            return 2;
        }
        char ch;
        while(fileReader.get(ch)) {
            if(!isalpha(ch)) continue;
            text.push_back(toupper(ch));
        }
    //  Else grab text from command line arg
    } else {
        std::string textString(parse.nonOption(0));
        for(int index = 0; index < int(textString.size()); index++) {
            if(!isalpha(textString.at(index))) continue;
            text.push_back(toupper(textString.at(index)));
        }       
    }

    //  Process the advanced options
    char doubleFill = 'Q';
    char extraFill = 'X';
    char omitLetter = 'J';
    char replaceLetter = 'I';
    if(options[INSERT]) {
        doubleFill = *(options[INSERT].last()->arg);
    }
    if(options[PAD]) {
        extraFill = *(options[PAD].last()->arg);
    }
    if(options[SKIP]) {
        omitLetter = *(options[SKIP].last()->arg);
    }
    if(options[REPLACE]) {
        char tmp = *(options[REPLACE].last()->arg);
        if(tmp == omitLetter) {
            fprintf(stderr, "Error: Replacement letter can not be the same as the replaced.");
            return 5;
        }
        replaceLetter = tmp;
    }

    //  Get key
    std::string keyWord;
    if(options[KEY]) {
        keyWord = std::string(options[KEY].last()->arg);
    }
    Key key(keyWord, doubleFill, extraFill, omitLetter, replaceLetter);
    key.sanitizeText(text);

    std::vector<char> result;
    //  If last METHOD flag is decrypt
    if(options[METHOD] && (options[METHOD].last()->type() == DECRYPT) ) {
        result = key.decrypt(text);
    } else {
        result = key.encrypt(text);
    }
    
    //  Print to stdout or to file
    int index = 0;
    while(index < int(result.size())) {
        output->put(result.at(index++));
        output->put(result.at(index++));
        if(options[BIGRAM])
            output->put(' ');
    }
    output->put('\n');

    return 0;
}
