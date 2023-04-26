CC = g++
CCFLAGS = -g -std=c++11
a = -lncursesw


SnakeGame: Main.o SnakeGame.o Point.o
	$(CC) $(CCFLAGS) -o test Main.o SnakeGame.o Point.o $(a)
clean:
	rm -f *.o

%.o : %.cpp %.h
	$(CC) $(CCFLAGS) -c $< $(a)

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $< $(a)

% : %.cpp
	$(CC) $(CCFLAGS) -o $@ $< $(a)
