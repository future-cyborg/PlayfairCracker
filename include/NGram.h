#ifndef NGRAM_H
#define NGRAM_H

#include <unordered_map>

class NGram {
public:
	NGram(int n);
	~NGram();

	int getN();
	std::unordered_map<std::string, int>

private:
	int N;
	std::unordered_map<std::string, int> frequencies;
	int totalCount;

}


#endif // NGRAM_H