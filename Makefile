CXX 	= g++ -std=c++11
CXXFLAGS= -Wall -g -fmessage-length=0
# OPTIMIZE= -Os -fomit-frame-pointer 
OPTIMIZE= -O0 -fomit-frame-pointer 

SRCDIR  = src
OBJDIR  = obj
INCDIR  = include

CMD 	= $(CXX) $(CXXFLAGS) $(OPTIMIZE) -I$(INCDIR)

CMDPRG 	= optionsparser.h


VERSION=1.0
PACKAGEDIR=playfairCracker-$(VERSION)
TARBALL=../$(PACKAGEDIR).tar.gz

HELPER  = PfHelpers.xx
SCRACK	= Key.xx PlayfairGenetic.xx FrequencyCollector.xx EnglishFitness.xx $(HELPER)

all: playfair playfairCracker doc

playfair: $(OBJDIR)/playfair.o $(OBJDIR)/Key.o $(INCDIR)/optionparser.h
	$(CMD) $(OBJDIR)/playfair.o $(OBJDIR)/Key.o -o $@

$(OBJDIR)/playfair.o: $(SRCDIR)/playfair.cpp $(INCDIR)/Key.hpp $(INCDIR)/optionparser.h
	$(CMD) -c $< -o $@

playfairCracker: $(OBJDIR)/playfairCracker.o $(patsubst %.xx, $(OBJDIR)/%.o, $(SCRACK))
	$(CMD) $^ -o $@

$(OBJDIR)/playfairCracker.o: $(SRCDIR)/playfairCracker.cpp $(patsubst %.xx, $(INCDIR)/%.hpp, $(SCRACK))
	$(CMD) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp $(patsubst %.xx, $(INCDIR)/%.hpp, $(HELPER))
	$(CMD) -c $< -o $@

doc:
	doxygen

clean:
	rm -f $(OBJDIR)/*.o playfair playfairCracker
	rm -rf source_html/