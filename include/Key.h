#ifndef KEY_H
#define KEY_H

#include <unordered_map>
#include <vector>

#define LFILL 'X'
#define LEXTR 'Q'
#define LOMIT 'J'
#define LREPL 'I'

class Key {
public:
    //  No keyword makes for a boring Playfair square
    Key(char doubleFill = LFILL, char extraFill = LEXTR, char omitLetter = LOMIT, char replaceLetter = LREPL);
    Key(std::string keyw, char doubleFill = LFILL, char extraFill = LEXTR, char omitLetter = LOMIT, char replaceLetter = LREPL);
    ~Key();
    
    //  Gets the keyword
    std::string getKeyword();

    //  If text is not sanitized, these will throw an out_of_range exception
    //  Encrypt plain text pText and return the cipher text
    std::vector<char> encrypt(std::vector<char> pText);
    //  Decrypt cipher text cText and return the plain text
    std::vector<char> decrypt(std::vector<char> cText);

    //  Sanitize text to work with this key.
    //  This function is used to sanitize:
    //      plain text for encryption
    //      cipher text for decryption
    //      keyword for generating the Playfair square
    //  
    //  Sanitize is only called automatically on keyword during initialization.
    //  You must sanitize all text before calling encrypt or decrypt on it!!
    std::vector<char> &sanitizeText(std::vector<char> &text);
private:
    //  The keyword to the cipher
    std::string keyword;
    //  This is the square that the en/decryption is done with
    char key[5][5];
    //  A letter's place in the square is held as a number 0-24.
    //      As if writing the numbers sequentially left to right, top down
    std::unordered_map<char, int> letterPlace;

    //  The letter that will be inserted between double letters
    char bufferDouble = LFILL;
    //  The letter that will be inserted at the end, in case of odd amount of letters
    char bufferExtra = LEXTR;
    //  The letter that will be absent from the key. Most often this is 'J'
    char letterOmit = LOMIT;
    //  The letter that will replace the absent letter in the text. Most often this is 'I'
    char letterReplace = LREPL;

    //  generate() produces the Playfair square from the given keyword
    int generate();

    //  Encrypts a digram
    char* encryptDigram(char a, char b);
    //  Decrypts a digram
    char* decryptDigram(char a, char b);  

    //  Helper functions to determing letter positioning from the int stored in letterPlace
    int getRow(int place);
    int getColumn(int place);
};

#endif // KEY_H
