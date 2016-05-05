
EXE1 = histograms.exe
EXE2 = average.exe

all: dirtree
	$(CXX) -std=c++11 -Isrc -o bin/$(EXE1) src/histograms.cpp
	$(CXX) -std=c++11 -o bin/$(EXE2) src/average.cpp

dirtree:
	mkdir -p bin
	mkdir -p obj

clean:
	rm -rf *.sdf bin/*.ilk bin/*.pdb obj/*.log obj/*.idb obj/*.pdb

cleanall:
	rm -rf *.sdf bin obj


