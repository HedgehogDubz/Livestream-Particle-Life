CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -framework Cocoa -framework IOKit -framework OpenGL

particlelife: main.cpp particle_life.cpp particle_life.hpp
	$(CXX) $(CXXFLAGS) main.cpp particle_life.cpp -o particlelife $(LDFLAGS)

run: particlelife
	./particlelife

clean:
	rm -rf particlelife *.dSYM

.PHONY: run clean
