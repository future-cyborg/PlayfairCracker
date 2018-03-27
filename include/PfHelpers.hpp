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

	bool isInteger(const std::string& s);

	bool isDouble(const std::string& s);
	
	bool isRate(const std::string& s);
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