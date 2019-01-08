#include "headers.h"


int main(int argc, const char** argv ) {
	
	cmdArguments args = parseInput(argc, argv);
	
	if (MODE_BENCHMARK)
			simulationsPerSecondBenchmark(args, argc, argv);
}
