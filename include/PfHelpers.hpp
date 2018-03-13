#ifndef PF_HELPERS_HPP
#define PF_HELPERS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

using std::vector;
using std::string;

class Exception : public std::exception {
public:
   Exception(const char* msg = "Error");
  ~Exception( );

   virtual const char* what( ) const throw ();
protected:
   const char* e_msg;
};

class InvalidKeyException : public Exception {
public:
   InvalidKeyException(const char* msg = "InvalidKeyException");
  ~InvalidKeyException( );
};

class InvalidParameters : public Exception {
public:
	InvalidParameters(const char* msg = "Invalid Parameters");
	~InvalidParameters( );
};

namespace PfHelpers {
	std::ifstream::pos_type fileSize(const string fileName);

	// Throws exception
	int readFile(const char* fileName, vector<char> &text);

	int printPopulation(vector<string> &population);

	//	Iterates using .at()
	//	Used on string, vector<char>, etc.
	template <typename Iterable>
	bool validKey(Iterable key);

	template <typename Number>
	Number sumVector(vector<Number> vec);

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