#ifndef FREQUENCYCOLLECTOR_HPP
#define FREQUENCYCOLLECTOR_HPP

#include <unordered_map>


typedef unsigned long long count_t;

class FrequencyCollector {
public:
     FrequencyCollector(int N);
    ~ FrequencyCollector();
    
    //  Parses Ngram file and checks for proper format, and no duplicates.
    bool validNgramFile(char fileName[]);
    
    /*  Read file that contains n-gram counts for English frequency and set standardFreq
        The file read must be formatted so that every line contains one ngram, a single space,
            then the number of occurances of that ngram. Like so -
        ABCD 17
        ABFT 12
        ABCA 19      */
    // Throws exception
    int readNgramCount(char fileName[]);

    // Throws exception
    int writeNGramCount(char fileName[]);

    //  This will collect ngrams from fileRead and write them to fileWrite
    // Throws exception
    int setNGramCount(char fileRead[], char fileWrite[]);

    //  Puts file into stream and calls collectFrequencies
    //  
    //  Return codes:
    //  0 - Normal
    //  1 - File can not be opened
    // Throws exception
    int collectNGramsFile(char fileName[]);

    //  Reads from stream and compiles map of ngram frequencies
    int collectNGrams(std::stringstream &buffer);

    int printNGrams();

    int getN() const;
    int clear();
    bool isEmpty() const;
    double frequency(std::string ngram) const;
private:
    const int n;
    unsigned long long totalCount;
    std::unordered_map<std::string, count_t> freqs;

    //  Returns the number of lines in a file
    // Throws exception
    unsigned long numFileLines(char* fileName);
};

#endif // FREQUENCYCOLLECTOR_HPP
