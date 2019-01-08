#include "./parser/cutil.h"
#include "headers.h"
	
cmdArguments parseInput(int argc, const char **argv) {

	cmdArguments args;
	char *inputArg = NULL;
	
	args.benchmarkIterations 	= DEFAULT_BENCHMARK_ITERATIONS;
	args.benchmarkDepth			= DEFAULT_BENCHMARK_DEPTH;
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "benchIter", &inputArg))
	
		args.benchmarkIterations = atoi (inputArg);
		
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "benchDepth", &inputArg))
	
		args.benchmarkDepth = atoi (inputArg);
		
	
	
	return args;
	
}
