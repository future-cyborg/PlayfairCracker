#ifndef KEY_HPP
#define KEY_HPP

#include <unordered_map>
#include <vector>

#define LFILL 'X'
#define LEXTR 'Q'
#define LOMIT 'J'
#define LREPL 'I'

/**
 * @brief %Key to encrypt & decrypt messages with the Playfair cipher
 * 
 * Contains the Playfair square associated with the keyword given during construction.
 * This class is built to work on English, and due to the nature of this cipher, might
 * not easily be changed to other languages.
 * @b Note: Errors might occur if using a locale other than the default "C" locale.
 * 
 * All enrcyptions and decryptions, as well as the keyword are returned as Latin 
 * uppercase letters. All non-letter characters are removed from text.
 * 
 * @attention Call member function sanitizeText() on text before encrypt() or
 * decrypt().
 * 
 * The following parameters add some options to how the Playfair cipher can be used:
 * 
 * @param keyWord              The keyword for the cipher. If none is given, you have
 *                              a boring cipher.
 * @param doubleFill        The letter that will be inserted between occurances of
 *                              double letters (e.g. wall -> walxl)
 * @param extraFill         The letter to add to the end of encrypted text if plain
 *                              text has odd length
 * @param omitLetter        The letter that is omitted from the text (usually J).
 * @param replaceLetter     The letter that replaces all occurances of omitLetter
 *                              (usually I).
 */

class Key {
public:
    Key(char doubleFill = LFILL, char extraFill = LEXTR, char omitLetter = LOMIT, char replaceLetter = LREPL);
    Key(std::string keyWord, char doubleFill = LFILL, char extraFill = LEXTR, char omitLetter = LOMIT, char replaceLetter = LREPL);
    ~Key();

    std::string getKeyword();

    std::vector<char> encrypt(const std::vector<char> &plainText);
    std::vector<char> decrypt(const std::vector<char> &cipherText);

    std::vector<char> &sanitizeText(std::vector<char> &text);
private:
    ///  The keyword to the cipher
    std::string keyword;
    ///  This is the square that the en/decryption is done with
    char key[5][5];
    /**  
    *        @brief A letter's place in the square is held as a number 0-24.
    *           As if writing the numbers sequentially left to right, top down
    */
    std::unordered_map<char, int> letterPlace;

    ///  The letter that will be inserted between double letters
    char bufferDouble = LFILL;
    ///  The letter that will be inserted at the end, in case of odd amount of letters
    char bufferExtra = LEXTR;
    ///  The letter that will be absent from the key. Most often this is 'J'
    char letterOmit = LOMIT;
    ///  The letter that will replace the absent letter in the text. Most often this is 'I'
    char letterReplace = LREPL;

    int generate();

    char* encryptDigram(char a, char b);
    char* decryptDigram(char a, char b);  

    //  Helper functions to determing letter positioning from the int stored in letterPlace
    int getRow(int place);
    int getColumn(int place);
};

#endif // KEY_HPP
