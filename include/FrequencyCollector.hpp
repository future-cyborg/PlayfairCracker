#ifndef FREQUENCYCOLLECTOR_HPP
#define FREQUENCYCOLLECTOR_HPP

#include <unordered_map>

typedef unsigned long long count_t;
typedef std::string ngram_t;

/**
 * @brief Collect and manage frequencies of English n-grams
 * 
 * Collect and manage frequencies of English n-grams.
 *  Contains methods for collecting, reading, and writing n-grams from text files or
 *  stringstream.
 * @n @b Note: This class is built to work on English. Errors might occur if using a
 *      locale other than the default "C" locale.
 *      
 * @param N The number of characters in this sized n-gram
 */
class FrequencyCollector {
public:
    /** 
     * @param N The number of characters in this sized n-gram.
     *          Require: N > 0, N < 14
     *          Suggest: N < 6
     *          
     * @throw InvalidParameters     If (N <= 0 || N > 13)
     */
     FrequencyCollector(unsigned N);
    ~ FrequencyCollector();
    
    /**
     * @brief Checks if file is a valid list of n-grams
     * 
     * Checks if a file contains a valid list of n-grams.
     * 
     * A file is valid if
     * @li All n-grams are of length n, as determined by object
     * @li File contains no blank lines
     * @li No duplicate n-grams
     * @li Each line is formatted: "[n-gram] [number]"
     * 
     * @b Example:
     * @code{.unparsed}
     * TH 11699
     * HE 10068
     * IN 8767
     * ER 7713
     * @endcode
     * 
     * Explanation will be printed to stderr if false.
     * 
     * @param fileName  The file to be checked
     * @return  bool
     */    
    bool validNgramFile(const char* fileName);

    /**
     * @brief Read a file that contains n-gram counts
     * 
     * Read from fileName that contains counts of n-gram occurrences. Counts are saved in
     *  freqs and totalCount is set. See validNgramFile() for details on valid file format.
     * 
     * @note Current n-gram frequencies stored in this object are overwritten.
     * 
     * @throw std::ios_base::failure    fileName cannot be opened
     * @throw Exception                 n-gram length contradiction
     * 
     * @b Basic @b guarantee: Will not throw exceptions if validNgramFile(fileName)
     * returns true.
     * 
     * @param fileName  The file to be read
     * @return  0 on completion
     */
    int readNgramCount(const char* fileName);

    /**
     * @brief Writes n-gram counts to file
     * 
     * Write current n-gram counts to fileName.
     * 
     * @throw std::ios_base::failure    fileName cannot be opened for write
     * 
     * @param fileName  The file to be read
     * @return  0 on completion
     */
    int writeNGramCount(const char* fileName);

    /**
     * @brief Collects n-gram frequencies and writes to file
     * 
     * Wrapper that calls collectNGramsFile(fileRead) then writeNGramsCount(fireWrite).
     * 
     * @note Current n-gram frequencies stored in this object are overwritten.
     * 
     * @throw std::ios_base::failure    fileRead or fileWrite cannot be opened
     * 
     * @param fileRead  The file to be read
     * @param fileWrite The file to write to
     * @return  0 on completion
     */
    int setNGramCount(const char* fileRead, const char* fileWrite);

    /**
     * @brief Parses file to collect n-gram frequencies
     * 
     * Read and parse text from fileName. All non Latin characters are ignored and n-gram
     * frequencies are collected. All letters are read as uppercase. See collectNGrams()
     * for example.
     * 
     * @note Any n-grams collected are added to the current counts. Call clear() prior to
     *          this function if necessary.
     * 
     * @throw std::ios_base::failure    fileName cannot be opened
     * 
     * @param fileName  The file to be read
     * @return  0 on completion
     */
    int collectNGramsFile(const char* fileName);

    /**
     * @brief Parse stringstream to collect n-gram frequencies
     * 
     * Read and parse text from buffer. All non Latin characters are ignored and n-gram
     * frequencies are collected. All letters are read as uppercase.
     * 
     * @note Any n-grams collected are added to the current counts. Call clear() prior to
     *          this function if necessary.
     *           
     * @b Example: The following stream is read:
     * @code{.unparsed}
     * The 5 c@ts \n ate that l!ver.
     * @endcode
     * 
     * With n = 2, the frequencies collected are:
     * @code
     * TH 2
     * HE 1
     * EC 1
     * CT 1
     * TS 1
     * SA 1
     * AT 2
     * TE 1
     * ET 1
     * HA 1
     * TL 1
     * LV 1
     * VE 1
     * ER 1
     * @endcode
    
     * @param buffer The stringstream to be read
     * @return  0 on completion
     */
    int collectNGrams(std::stringstream &buffer);

    /**
     * @brief Print n-gram counts
     * 
     * Writes n-gram counts to buffer. Pass std::cout to print to screen.
     * 
     * @param buffer  Output stream to be written to
     * @return  0 on completion
     */
    int printNGrams(std::ostream &buffer);

    /**
     * @brief Returns N, the type of n-gram for this object
     * 
     * Returns N, determining the size of n-grams this object is associated with.
     *  (i.e. 2 = bigram, 3 = trigram)
     * 
     * @return  integer
     */
    unsigned getN() const;

    /**
     * @brief Returns the total count of all n-grams
     * 
     * Returns the total count of all n-grams stored in this object
     * 
     * @return unsigned long long
     */
    count_t getCount() const;

    /**
     * @brief Get frequency of ngram
     * 
     * Get frequency of ngram for n-grams stored in this object. Call after setting
     *  frequencies via readNGramCount(), setNGramCount(), collectNGramsFile(),
     *  collectNGrams().
     *  
     * It is possible to call this function with ngram.size() != n. Result will always
     *  be 0.0 in this case.
     * 
     * 
     * @param ngram n-gram to get frequency for
     * @return double in range (0.0, 1.0)
     */
    double frequency(ngram_t ngram) const;

    /**
     * @brief Checks if object has collected frequencies
     * 
     * Returns the result of freqs.empty().
     * 
     * @return bool
     */
    bool isEmpty() const;

    /**
     * @brief Clears collected frequencies
     * 
     * Calls freqs.clear() and resets totalCount.
     * 
     * @return  0 on completion
     */
    int clear();

private:
    /// N for the type of n-gram.
    const unsigned n;

    /// Stores the totalCount of collected frequencies.
    unsigned long long totalCount;

    /**
     * @brief Stores counts of each ngram
     * 
     * Stores the counts of each ngram, if any. Divide value by totalCount to get
     *  the frequency.
     */
    std::unordered_map<ngram_t, count_t> freqs;

    /**
     * @brief Returns the number of lines in a file
     * 
     * Returns the number of lines in a file.
     * 
     * @throw std::ios_base::failure    fileName cannot be opened
     * 
     * @param fileName  The file to be read
     * @return unsigned long
     */
    unsigned long numFileLines(const char* fileName);
};

#endif // FREQUENCYCOLLECTOR_HPP
