#ifndef PF_HELPERS_HPP
#define PF_HELPERS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

using std::vector;
using std::string;

class Exception {
public:
   Exception(const char* msg = "Error");
  ~Exception( );

   const char* getMessage( ) const;
private:
   const char* e_msg;
};

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


#endif // PF_HELPERS_HPP