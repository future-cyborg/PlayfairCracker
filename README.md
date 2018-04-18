# Playfair Cracker

This project spawned while trying to solve Stage 6 of Simon Singh's *The Code Book* cipher challenge. (Check it out! https://simonsingh.net/cryptography/cipher-challenge/)

This includes 3 command line programs
- playfair - For encrypting and decrypting text with the Playfair cipher
- ngramFrequency - For collecting and managing English n-gram frequencies
- playfairCracker - To help crack text encrypted with the Playfair cipher

These only work with English, sorry! I did consider allowing these to easily include or be rewritten for other languages, but due to the unique nature of this cipher, that was too difficult.

#### In progress - check back in a couple weeks for completion.
Writing tests and finish documentation right now.
Tests 25% done. Documentation 50% done. 4/17/18

## Installation
Use Makefile to install. Run `make` to build all or specify individual program. e.g. `make playfair`
Documentation uses Doxygen and will be generated with make if Doxygen is installed.

## Programs

### playfair
A simple program to encrypt/decrypt text with the Playfair cipher.
Has options to specify which letter is being replaced in the message, and the replacing letter. (Default: 'J' to 'I').
Also has options to specify which letter will be inserted between doubles and/or which letter pads an odd-length message.

### ngramFrequency
This is a helper program for the playfairCracker to collect and manage frequencies of n-grams for [frequency analysis](http://www.practicalcryptography.com/cryptanalysis/letter-frequencies-various-languages/english-letter-frequencies/). 
The frequencies directory contains English n-gram frequencies already, but use this if you want to collect frequency analysis from any sample data. Feed it your own corpus, or selection of texts!
#### TODO: Add functionality to add frequencies to a past frequency file.

### playfairCracker
Use this program to *help* crack a message encoded with the Playfair cipher. Frequency analysis is used to determine how likely a given decrypted text is to English, which is its fitness score. The Playfair cipher's handling of double letters is likely to cause an incorrect key to produce a higher fitness score than the correct key, especially with a small message and/or many cases of double letters.

'The cat fell off the wall' becomes 'th ec at fe lx lo fx ft he wa lx lx'. The won't sentence will not produce a high fitness score!


## Acknowledgments 
Helpful tools, thank you!
[The Lean Mean C++ Option Parser](http://optionparser.sourceforge.net/)
[PCG Random Number Generation for C++](http://www.pcg-random.org/)
[Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
[CxxTest](http://cxxtest.com/)
