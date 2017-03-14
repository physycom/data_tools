
EXE1 = histograms.exe
EXE2 = average.exe
EXE3 = simultaneous.exe

all: dirtree
	$(CXX) -std=c++11 -Isrc/jsoncons/src -o bin/$(EXE1) src/histograms.cpp
	$(CXX) -std=c++11 -o bin/$(EXE2) src/average.cpp
	$(CXX) -std=c++11 -o bin/$(EXE3) src/simultaneous.cpp

dirtree:
	@mkdir -p bin
	@mkdir -p obj

clean:
	rm -f bin/$(EXE1) bin/$(EXE2) bin/$(EXE3) obj/*

cleanall:
	rm -rf bin obj


