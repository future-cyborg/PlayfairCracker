CXX 	= g++ -std=c++11
CXXFLAGS= -Wall -g -fmessage-length=0
OPTIMIZE= -Os -fomit-frame-pointer

SRCDIR  = src
OBJDIR  = obj
INCDIR  = include

CMD 	= $(CXX) $(CXXFLAGS) $(OPTIMIZE) -I$(INCDIR)

CMDPRG 	= optionsparser.h


VERSION=1.0
PACKAGEDIR=playfairCracker-$(VERSION)
TARBALL=../$(PACKAGEDIR).tar.gz

all: playfair playfairCracker doc

playfair: $(OBJDIR)/playfair.o $(OBJDIR)/Key.o $(INCDIR)/optionparser.h
	$(CMD) $(OBJDIR)/playfair.o $(OBJDIR)/Key.o -o $@

$(OBJDIR)/playfair.o: $(SRCDIR)/playfair.cpp $(INCDIR)/Key.h $(INCDIR)/optionparser.h
	$(CMD) -c $< -o $@

playfairCracker: $(OBJDIR)/playfairCracker.o $(OBJDIR)/PlayfairGenetic.o $(OBJDIR)/Key.o $(INCDIR)/helpers.h $(OBJDIR)/FrequencyCollector.o
	$(CMD) $(OBJDIR)/playfairCracker.o $(OBJDIR)/PlayfairGenetic.o $(OBJDIR)/Key.o $(OBJDIR)/FrequencyCollector.o -o $@

$(OBJDIR)/playfairCracker.o: $(SRCDIR)/playfairCracker.cpp $(INCDIR)/Key.h $(INCDIR)/helpers.h $(INCDIR)/PlayfairGenetic.h $(INCDIR)/FrequencyCollector.h
	$(CMD) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.h
	$(CMD) -c $< -o $@

doc: