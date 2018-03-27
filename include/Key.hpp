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
 * @n @b Note: Errors might occur if using a locale other than the default "C" locale.
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

    /**
     * @brief       Returns keyword as given in constructor
     * 
     * @details     The keyword given in constructor is returned, rather than the keyword
     *                  as it appears in the square. (e.g. applecake,  not APLECK)
     * 
     * @return keyword
     */
    std::string getKeyword();

    /**
     * @brief       Encrypts plainText with key
     * @details     Call sanitizeText() on plainText before using this function to ensure no
     *                  errors occur.
     * 
     * @param plainText     Must only contain the 25 valid uppercase characters. There will
     *                          be errors otherwise. (Likely out_of_range exception).
     * @return  Encrypted cipherText
     */
    std::vector<char> encrypt(const std::vector<char> &plainText);

    /**
     * @brief       Decrypts cipherText with key
     * @details     Call sanitizeText() on cipherText before using this function to ensure no
     *                  errors occur. Letters added during encryption, due to double letters
     *                  or odd length text, are still included in decrypted text.
     * 
     * @param cipherText    Must only contain the 25 valid uppercase characters. There will
     *                          be errors otherwise. (Likely out_of_range exception).
     * @return Decrypted plainText
     */
    std::vector<char> decrypt(const std::vector<char> &cipherText);

    /**
     * @brief   Prepare text for encrypt() and decrypt()
     * @details This function is used to sanitize:
     * @li          plainText  for encrypt()
     * @li          cipherText for decrypt()
     * @li          keyword    for generate() *(called internally)
     *
     * Valid characters are standard Latin letters A to Z, minus letterOmit.
     * All non-valid characters are removed from text, while all lowercase equivalents
     *      to valid characters are converted to uppercase.
     * 
     * @param text  Reference to text to be sanitized
     * @return  Reference to text
     */
    std::vector<char> &sanitizeText(std::vector<char> &text);
private:
    ///  The keyword to the cipher
    std::string keyword;
    ///  This is the square that the en/decryption is done with
    char key[5][5];
    /**  
    *  @brief A letter's place in the square is held as a number 0-24.
    *    As if writing the numbers sequentially left to right, top down.
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

    /**
     * @brief   Called during construction to set up private variables
     * 
     * @return  0 on completion
     */
    int generate();

    /**
     * @brief   Encrypts digram 
     * 
     * @return  Encrypted digram as char[2]
     */
    char* encryptDigram(char a, char b);

    /**
     * @brief   Decrypts digram 
     * 
     * @return  Decrypted digram as char[2]
     */
    char* decryptDigram(char a, char b);  

    //  Helper functions to determing letter positioning from the int stored in letterPlace
    int getRow(int place);
    int getColumn(int place);
};

#endif // KEY_HPP
