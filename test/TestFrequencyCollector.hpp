#include "cxxtest/TestSuite.h"
#include "FrequencyCollector.hpp"
#include "PfHelpers.hpp"
#include <iostream>

using std::vector;
using std::string;

class TestFreq : public CxxTest::TestSuite {
public:
	void testConstructor(void) {
		TS_ASSERT_THROWS(FrequencyCollector fc(0), InvalidParameters);
		for(unsigned n = 1; n < 14; n++) {
			TS_ASSERT_THROWS_NOTHING(FrequencyCollector fc(n));
		}
		TS_ASSERT_THROWS(FrequencyCollector fc(14), InvalidParameters);
	}
	void testValidFile(void) {
		char fileNamePass[] = "test/frequencies/freq_pass_1_1";
		for(unsigned i = 1; i <= 5; i++) {
			fileNamePass[27] = '0' + i;
			FrequencyCollector fc(i);
			TS_ASSERT(fc.validNgramFile(fileNamePass));
		}

		char fileNameFail[] = "test/frequencies/freq_fail_2_1";
		FrequencyCollector fc2(2);
		for(unsigned i = 1; i <= 6; i++) {
			fileNameFail[29] = '0' + i;
			TS_ASSERT(!fc2.validNgramFile(fileNameFail));
		}
	}
};