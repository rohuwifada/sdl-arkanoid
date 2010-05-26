blocks:	main.cpp
	g++ main.cpp -o balls -g -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

release: main.cpp
	g++ -o3 -march=pentium-mmx main.cpp -o balls -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
