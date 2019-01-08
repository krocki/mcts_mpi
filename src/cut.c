#include "./parser/cutil.h"
#include "headers.h"

cmdArguments parseInput(int argc, const char **argv) {
    
	cmdArguments args;
	char *inputArg = NULL;
	
	args.benchmarkIterations 	= DEFAULT_BENCHMARK_ITERATIONS;
	args.depth					= DEFAULT_DEPTH;
	args.maxTime				= DEFAULT_TIME;
	args.C						= DEFAULT_C;
	args.voteType[0]			= AVERAGE_VOTE;
	args.voteType[1]			= AVERAGE_VOTE;
	args.maxprocs				= 1;
	args.cpuThreads				= 1;
	args.gpuThreads				= 0;
	args.blocks					= 1;
	args.threads				= 1;
	args.device					= 0;
	args.MCTS_ver					= 0;
	if ( cutGetCmdLineArgumentstr(argc, argv, "benchIter", &inputArg))
        
		args.benchmarkIterations = atoi (inputArg);
    
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "benchDepth", &inputArg))
        
		args.depth = atoi (inputArg);
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "time", &inputArg))
        
		args.maxTime = (float) (atoi (inputArg))/1000.0f;
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "c", &inputArg))
        
		args.C = atof (inputArg);
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "method0", &inputArg))
        
		args.voteType[0] = atoi (inputArg);
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "method1", &inputArg))
        
		args.voteType[1] = atoi (inputArg);
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "maxprocs", &inputArg))
        
		args.maxprocs = atoi (inputArg);
	
	if ( cutGetCmdLineArgumentstr(argc, argv, "cpuThreads", &inputArg))
        
		args.cpuThreads = atoi (inputArg);
    
	if ( cutGetCmdLineArgumentstr(argc, argv, "gpuThreads", &inputArg))
        
		args.gpuThreads = atoi (inputArg);
	if ( cutGetCmdLineArgumentstr(argc, argv, "blocks", &inputArg))
        
		args.blocks = atoi (inputArg);
	if ( cutGetCmdLineArgumentstr(argc, argv, "threads", &inputArg))
        
		args.threads = atoi (inputArg);
    
	if ( cutGetCmdLineArgumentstr(argc, argv, "device", &inputArg))
        
		args.device = atoi (inputArg);
	if ( cutGetCmdLineArgumentstr(argc, argv, "MCTS_ver", &inputArg))
        
		args.MCTS_ver = atoi (inputArg);
    
	return args;
	
}
