#ifndef FREQUENCYCOLLECTOR_H
#define FREQUENCYCOLLECTOR_H

#include <unordered_map>

/*  Return codes shared amongst functions:
        0 - Normal
        1 - File can not be opened
        2 - Length of ngrams found in file differ from arguement n
*/
typedef unsigned long long count_t;
struct NGrams {
    int n;
    count_t count;
    std::unordered_map<std::string, count_t> *freqs;
};

class FrequencyCollector {
public:
     FrequencyCollector();
    ~ FrequencyCollector();
    
    /*  Read file that contains n-gram counts for English frequency and set standardFreq
        The file read must be formatted so that every line contains one ngram, a single space,
            then the number of occurances of that ngram. Like so -
        ABCD 17
        ABFT 12
        ABCA 19      */
    // Throws exception
    int readNgramCount(char fileName[], NGrams &nGrams);

    // Throws exception
    int writeNGramCount(char fileName[], NGrams &nGrams);

    //  This will collect ngrams from fileRead and write them to fileWrite
    // Throws exception
    int setNGramCount(int n, char fileRead[], char fileWrite[]);

    //  Puts file into stream and calls collectFrequencies
    //  
    //  Return codes:
    //  0 - Normal
    //  1 - File can not be opened
    // Throws exception
    int collectNGramsFile(char fileName[], NGrams &nGrams);

    //  Reads from stream and compiles map of ngram frequencies
    int collectNGrams(std::stringstream &buffer, NGrams &nGrams);

    int printNGrams(NGrams &nGrams);
private:
    //  Returns the number of lines in a file
    // Throws exception
    unsigned long numFileLines(char* fileName);
};

#endif // FREQUENCYCOLLECTOR_H
