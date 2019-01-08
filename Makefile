all:		mcts

clean:
		rm -rf ./out/* mcts

mcts:		./out/main.o ./out/common.o ./out/reversi.o ./out/cut.o ./out/mpi.o  ./out/parser.o ./out/cutil.o ./out/multithreading.o ./out/mcts.o 
		mpicxx -O3 -o mcts ./out/main.o ./out/common.o ./out/reversi.o ./out/mpi.o ./out/cut.o ./out/cutil.o ./out/parser.o ./out/multithreading.o ./out/mcts.o -L/usr/local/cuda/lib64 -lcudart -lcurses

#curses:		./out/main.o ./out/common.o ./out/reversi.o ./out/cut.o ./out/mpi.o ./out/parser.o ./out/cutil.o ./out/mcts.o
#		mpicxx -O3 -o curses ./out/main.o ./out/common.o ./out/reversi.o ./out/mpi.o ./out/cut.o ./out/cutil.o ./out/parser.o ./out/mcts.o -L/usr/local/cuda/lib64 -lcudart -lcurses

./out/main.o:		./src/main.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		mpicxx -O3 -o ./out/main.o -c  ./src/main.c

./out/common.o:	./src/common.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		mpicxx -O3 -o ./out/common.o -c  ./src/common.c

./out/reversi.o:	./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		mpicxx -O3 -o ./out/reversi.o -c  ./src/reversi.c
		
./out/cut.o:		./src/cut.c ./src/headers.h
		mpicxx -O3 -o ./out/cut.o -c  ./src/cut.c
		
./out/mpi.o:		./src/mpi.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		mpicxx -O3 -o ./out/mpi.o -c  ./src/mpi.c

./out/mcts.o: 		./src/mcts.cu ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h ./src/mpi.c ./src/mcts_gpu.cu
		nvcc -O3 --use_fast_math --ptxas-options=-v -lcudart -lcuda -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -o ./out/mcts.o -c  ./src/mcts.cu
		
./out/cutil.o:		./src/parser/cutil.cpp ./src/parser/cutil.h
		mpicxx -O3 -o ./out/cutil.o -c  ./src/parser/cutil.cpp
		
./out/parser.o: 		./src/parser/cmd_arg_reader.cpp ./src/parser/cmd_arg_reader.h
		mpicxx -O3 -o ./out/parser.o -c  ./src/parser/cmd_arg_reader.cpp
		
./out/multithreading.o:	./src/multithreading/multithreading.cpp ./src/multithreading/multithreading.h
		mpicxx -O3 -o ./out/multithreading.o -c ./src/multithreading/multithreading.cpp
