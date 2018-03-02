#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "Key.hpp"
#include "optionparser.h"


struct Arg: public option::Arg
{
	static void printError(const char* msg1, const option::Option& opt, const char* msg2)
	{
		fprintf(stderr, "%s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	static option::ArgStatus Unknown(const option::Option& option, bool msg)
	{
		if (msg) printError("Unknown option '", option, "'\n");
		return option::ARG_ILLEGAL;
	}

	static option::ArgStatus Required(const option::Option& option, bool msg)
	{
		if (option.arg != 0)
			return option::ARG_OK;

		if (msg) printError("Option '", option, "' requires an argument\n");
		return option::ARG_ILLEGAL;
	}

    static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  	{
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
{ UNKNOWN,   0,"",  "",       Arg::Unknown, "USAGE: playfairCipher [OPTION]... TEXT\n"
											"       playfairCipher [OPTION]... -f FILE\n"
											"\nOPTIONS:" },
{ HELP,      0,"",  "help",   Arg::None,    "            \t--help\n"
                                            "            \t  Print usage and exit.\n"},
{ METHOD,    0,"e", "encrypt",Arg::None,    "  -e,       \t--encrypt (default mode)"},
{ METHOD,    1,"d", "decrypt",Arg::None,    "  -d,       \t--decrypt"},
{ KEY,       0,"k", "key",    Arg::NonEmpty,"  -k,       \t--key=KEY"},
{ KEY,	     0,"",  "keyword",Arg::NonEmpty,"            \t--keyword=KEY"},
{ INPUTFILE, 0,"f", "file",   Arg::NonEmpty,"  -f,       \t--file=FILE"},
{ OUTPUTFILE,0,"o", "output", Arg::NonEmpty,"  -o,       \t--output=FILE"},
{ BIGRAM,    0,"b", "bigram", Arg::None,"  -b,       \t--bigram\n"
											"            \t  Print output as bigrams (e.g. BI GR AM)\n"
											"\nADVANCED OPTIONS:"},
{ INSERT,    0,"i", "insert", Arg::NonEmpty,"  -i,       \t--insert=LETTER\n"
											"            \t  Letter to insert between double letters (default Q)\n"},
{ PAD,       0,"p", "pad",    Arg::NonEmpty,"  -p,       \t--pad=LETTER\n"
											"            \t  Letter to pad odd length message (default X)\n"},
{ SKIP,      0,"s", "skip",   Arg::NonEmpty,"  -s,       \t--skip=LETTER\n"
											"            \t  Letter that is skipped/replaced in message (default J)\n"},
{ REPLACE,   0,"r", "replace",Arg::NonEmpty,"  -r,       \t--replace=LETTER\n"
											"            \t  Letter that replaces the skipped letter (default I)\n"},
{ UNKNOWN,   0,"",  "",       Arg::None,
 "EXAMPLES:\n"
 "  playfairCipher -e -k keyword -o cipher.txt -f plain.txt \n"
 "  playfairCipher -e -k keywork \"Encrypt this text\"\n"
 "  playfairCipher -d cipher.txt \n"
 "  playfairCipher -d -k keyword -o plain.txt cipher.txt\n"
 "\nIf no key is provided, that's a boring Playfair square!"
},
{ 0, 0, 0, 0, 0, 0 } };

int main(int argc, char* argv[])
{
	argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
	option::Stats stats(usage, argc, argv);

	std::vector<option::Option> options(stats.options_max);
	std::vector<option::Option> buffer(stats.buffer_max);
	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

	if (parse.error())
		return 1;

	if (options[HELP] || argc == 0)
	{
		int columns = getenv("COLUMNS")? atoi(getenv("COLUMNS")) : 80;
		option::printUsage(fwrite, stdout, usage, columns);
		return 0;
	}

	if(!options[INPUTFILE] && (parse.nonOptionsCount() == 0) ) {
		fprintf(stderr, "Usage requires either input file or text.\n");
		fprintf(stderr, "Try 'playfairCipher --help' for more information.\n");
	}

	//	Open before reading input text, so can exit more quickly if error opening file
	std::ofstream outFile;
	if(options[OUTPUTFILE]) {
		outFile.open(options[OUTPUTFILE].last()->arg);
		if(!outFile) {
			fprintf(stderr, "%s cannot be opened for writing", options[OUTPUTFILE].last()->arg);
			return 3;
		}
	}


	//	Collect text
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
	//	Else grab text from command line arg
	} else {
		std::string textString(parse.nonOption(0));
		for(int index = 0; index < int(textString.size()); index++) {
			if(!isalpha(textString.at(index))) continue;
			text.push_back(toupper(textString.at(index)));
		}		
	}

	//	Process the advanced options
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

	//	Get key
	std::string keyWord;
	if(options[KEY]) {
		keyWord = std::string(options[KEY].last()->arg);
	}
	Key key(keyWord, doubleFill, extraFill, omitLetter, replaceLetter);
	key.sanitizeText(text);

	std::vector<char> result;
	// 	If last METHOD flag is decrypt
	if(options[METHOD] && (options[METHOD].last()->type() == DECRYPT) ) {
		result = key.decrypt(text);
	} else {
		result = key.encrypt(text);
	}
	
	// 
	if(options[BIGRAM]) {

	}
	//	Print to stdout or to file
	if(options[OUTPUTFILE]) {
		int index = 0;
		while(index < int(result.size())) {
			outFile.put(result.at(index++));
			outFile.put(result.at(index++));
			if(options[BIGRAM])
				outFile.put(' ');
		}
	} else {
		int index = 0;
		while(index < int(result.size())) {
			std::cout << result.at(index++);
			std::cout << result.at(index++);
			if(options[BIGRAM])
				std::cout << ' ';
		}
	}
	return 0;
}
