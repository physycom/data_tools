EXE = histograms.exe


all:
	$(CXX) -std=c++11 -Isrc -o bin/$(EXE) src/histograms.cpp

clean:
	rm -rf *.sdf bin/*.ilk bin/*.pdb obj/*.log obj/histograms.* obj/*.idb obj/*.pdb

cleanall:
	rm -rf *.sdf bin/*.exe bin/*.ilk bin/*.pdb obj/*.log obj/histograms.* obj/*.idb obj/*.pdb
