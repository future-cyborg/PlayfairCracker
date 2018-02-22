#ifndef PF_HELPERS_H
#define PF_HELPERS_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

using std::vector;
using std::string;

class InvalidKeyException {
public:
   InvalidKeyException(const char* msg = "InvalidKeyException");
  ~InvalidKeyException( );

   const char* getMessage( ) const;
private:
   const char* e_msg;
};

class InvalidParameters {
public:
	InvalidParameters(const char* msg = "Invalid Parameters");
	~InvalidParameters( );

	const char* getMessage( ) const;
private:
	const char* e_msg;
};

namespace PfHelpers {
	std::ifstream::pos_type fileSize(const string fileName);

	// Throws exception
	int readFile(const char* fileName, vector<char> &text);


	class Timer	{
	public:
		Timer();		
		int reset();		
		double elapsed() const;
	private:
		// Type aliases to make accessing nested type easier
		using clock_t = std::chrono::high_resolution_clock;
		using second_t = std::chrono::duration<double, std::ratio<1> >;
		
		std::chrono::time_point<clock_t> m_beg;	
	};
}


#endif // PF_HELPERS_H_