#include "cxxtest/TestSuite.h"
#include "Key.hpp"

using std::vector;
using std::string;
typedef vector<char> text_t;

class TestKey : public CxxTest::TestSuite {
public:
	string key = "APLE";
	void testKeywords(void) {
		string plainText = "The dog jumped over the moon";
		string cipherText = "UGPGNHOQKLPGIZPTUGLNMZIO";
		string newPlain   = "THEDOGIUMPEDOVERTHEMOXON";
		text_t pText;
		text_t cTextCorrect(cipherText.begin(), cipherText.end());
		text_t newPlainText(newPlain.begin(), newPlain.end());
		vector<string> keywords = {"apple", "APple", "aP3ple",
									"A\n*pllllelll", "a@\"pap(lE"};
		for(unsigned i = 0; i < keywords.size(); i++) {
			Key k(keywords.at(i));
			pText.assign(plainText.begin(), plainText.end());
			k.sanitizeText(pText);

			text_t cipher = k.encrypt(pText);
			TS_ASSERT(cipher == cTextCorrect);
			TS_ASSERT(k.decrypt(cipher) == newPlainText);

		}
	}

	void testFillLetters(void) {
		string plainText = "QQZZZ";
		vector<string> cipherTexts   = {"SVUVVYVU", "RSUVVUVU", "UVUVVBVU"};
		vector<string> newPlainTexts = {"QXQZZXZQ", "QRQZZQZQ", "QZQZZAZQ"};
		vector<text_t> cTexts;
		vector<text_t> newPTexts;
		for(unsigned i = 0; i < cipherTexts.size(); i++) {
			cTexts.emplace_back(cipherTexts.at(i).begin(), cipherTexts.at(i).end());
			newPTexts.emplace_back(newPlainTexts.at(i).begin(), newPlainTexts.at(i).end());
		}
		text_t pText;
		text_t fillLetters = {'X', 'x', 'Q', 'q', 'Z', 'z'};

		for(unsigned i = 0; i < fillLetters.size(); i++) {
			Key k(key, fillLetters.at(i));
			pText.assign(plainText.begin(), plainText.end());
			text_t cipher = k.encrypt(pText);
			TS_ASSERT(cipher == cTexts.at(i/2));
			TS_ASSERT(k.decrypt(cipher) == newPTexts.at(i/2));
		}
	}

	void testExtraLetters(void) {
		text_t extraLetters = {'Q', 'X', 'Z'};
		vector<text_t> plainTexts = {{'X'}, {'X'}, {'Z'}};
		vector<text_t> cTexts = {{'V', 'S'},
								 {'Y', 'Z'},
								 {'V', 'B'}};
		vector<text_t> pTexts = {{'X', 'Q'},
						 		 {'X', 'Y'},
						 		 {'Z', 'A'}};
		text_t pText;
		for(unsigned i = 0; i < extraLetters.size(); i++) {
			Key k(key, 'X', extraLetters.at(i));
			pText = plainTexts.at(i);;
			text_t cipher = k.encrypt(pText);
			TS_ASSERT(cipher == cTexts.at(i));
			TS_ASSERT(k.decrypt(cipher) == pTexts.at(i));
		}
	}

	void testOmitLetters(void) {
		text_t plainText = {'I','X','Q','J'};
		text_t omitLetters    = {'J', 'J', 'I', 'I', 'Q'};
		text_t replaceLetters = {'I', 'J', 'I', 'J', 'A'};
		
		vector<text_t> cTexts = {{'M', 'V', 'V', 'Q'},
								 {'M', 'V', 'V', 'Q'},
								 {'M', 'V', 'V', 'Q'},
								 {'M', 'V', 'V', 'Q'},
								 {'K', 'V', 'P', 'I'}};

		vector<text_t> pTexts = {{'I', 'X', 'Q', 'I'},
								 {'I', 'X', 'Q', 'I'},
								 {'J', 'X', 'Q', 'J'},
								 {'J', 'X', 'Q', 'J'},
								 {'I', 'X', 'A', 'J'}};
		for(unsigned i = 0; i < omitLetters.size(); i++) {
			Key k(key, 'X', 'Q', omitLetters.at(i), replaceLetters.at(i));
			plainText = {'I','X','Q','J'};
			k.sanitizeText(plainText);
			text_t cipher = k.encrypt(plainText);
			TS_ASSERT(cipher == cTexts.at(i));
			TS_ASSERT(k.decrypt(cipher) == pTexts.at(i));
		}
	}

	void testSanitize(void) {
		text_t text;
		for(unsigned i = 0; i < 256; i++) {
			text.push_back(i);
		}
		Key k("");
		k.sanitizeText(text);
		string s("ABCDEFGHIIKLMNOPQRSTUVWXYZABCDEFGHIIKLMNOPQRSTUVWXYZ");
		text_t correct(s.begin(), s.end());
		TS_ASSERT(correct == text);
	}

};