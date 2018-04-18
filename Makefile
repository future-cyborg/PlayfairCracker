CXX 	= g++ -std=c++11
CXXFLAGS= -Wall -g -fmessage-length=0
# OPTIMIZE= -Os -fomit-frame-pointer 
OPTIMIZE= -O0 -fomit-frame-pointer

SRCDIR  = src
OBJDIR  = obj
INCDIR  = include
TSTDIR  = test

CMD 	= $(CXX) $(CXXFLAGS) $(OPTIMIZE) -I$(INCDIR)
CMDTEST = $(CMD) -I$(TSTDIR)

HELPER  = PfHelpers

TESTGEN = ~/cplusplus/cxxtest-4.3/bin/cxxtestgen
TEST    = Key FrequencyCollector
TESTH   = $(TEST) $(HELPER)

HELPER  = PfHelpers

NGRAM   = FrequencyCollector $(HELPER)
SCRACK	= Key PlayfairGenetic FrequencyCollector EnglishFitness $(HELPER)

VERSION=1.0
PACKAGEDIR=playfairCracker-$(VERSION)
TARBALL=../$(PACKAGEDIR).tar.gz

all: playfair ngramFrequency playfairCracker

playfair: $(OBJDIR)/playfair.o $(OBJDIR)/Key.o
	$(CMD) $(OBJDIR)/playfair.o $(OBJDIR)/Key.o -o $@

$(OBJDIR)/playfair.o: $(SRCDIR)/playfair.cpp $(INCDIR)/Key.hpp
	$(CMD) -c $< -o $@

ngramFrequency: $(OBJDIR)/ngramFrequency.o $(patsubst %, $(OBJDIR)/%.o, $(NGRAM))
	$(CMD) $^ -o $@

$(OBJDIR)/ngramFrequency.o: $(SRCDIR)/ngramFrequency.cpp $(patsubst %, $(INCDIR)/%.hpp, $(NGRAM))
	$(CMD) -c $< -o $@

playfairCracker: $(OBJDIR)/playfairCracker.o $(patsubst %, $(OBJDIR)/%.o, $(SCRACK))
	$(CMD) $^ -o $@

$(OBJDIR)/playfairCracker.o: $(SRCDIR)/playfairCracker.cpp $(patsubst %, $(INCDIR)/%.hpp, $(SCRACK))
	$(CMD) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp $(patsubst %, $(INCDIR)/%.hpp, $(HELPER))
	$(CMD) -c $< -o $@

doc:
	doxygen

clean:
	rm -f $(OBJDIR)/*.o playfair playfairCracker
	rm -rf source_html/
	rm -f $(TSTDIR)/RunTest $(TSTDIR)/RunTest.cpp

.PHONY: test
test: $(patsubst %, $(TSTDIR)/Test%.hpp, $(TEST)) $(patsubst %, $(OBJDIR)/%.o, $(TESTH))
	$(TESTGEN) --error-printer -o $(TSTDIR)/RunTest.cpp $(patsubst %, $(TSTDIR)/Test%.hpp, $(TEST))
	$(CMDTEST) -o runTest $(patsubst %, $(OBJDIR)/%.o, $(TESTH)) $(TSTDIR)/RunTest.cpp