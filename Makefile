all:		mcts

clean:	
		rm -rf ./out/* mcts

mcts:		main.o common.o reversi.o cut.o mpi.o parser.o cutil.o
	
		mpicxx -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -DUNIX -O3 -o mcts ./out/main.o ./out/common.o ./out/reversi.o ./out/mpi.o ./out/cut.o ./out/cutil.o ./out/parser.o

main.o:		./src/main.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/main.o -c  ./src/main.c

common.o:	./src/common.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/common.o -c  ./src/common.c

reversi.o:	./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/reversi.o -c  ./src/reversi.c
		
cut.o:		./src/cut.c ./src/headers.h
		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/cut.o -c  ./src/cut.c
		
mpi.o:		./src/mpi.c ./src/headers.h ./src/common.c ./src/reversi.c ./src/reversi.h
		mpicxx -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -DUNIX -O3 -o ./out/mpi.o -c  ./src/mpi.c
		
cutil.o:		./src/parser/cutil.cpp ./src/parser/cutil.h
		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/cutil.o -c  ./src/parser/cutil.cpp
		
parser.o: 		./src/parser/cmd_arg_reader.cpp ./src/parser/cmd_arg_reader.h

		g++  -I/usr/local/cuda/include -DUNIX -O3 -Wall -Wimplicit -Wswitch -Wformat -Wchar-subscripts -Wparentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -fno-strict-aliasing -o ./out/parser.o -c  ./src/parser/cmd_arg_reader.cpp
