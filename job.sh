
##!/bin/sh
#
# sample
#
export LD_LIBRARY_PATH=/usr/local/cuda/lib64
mpirun -np 1 mcts --benchIter=250 --time=200 --cpuThreads=2 --blocks=4 --threads=64
#mpirun -np 2 mcts --benchIter=250 --time=200 --cpuThreads=2 --blocks=4 --threads=64 >> at30_cpu2_200ms.txt
#mpirun -np 3 mcts --benchIter=250 --time=200 --cpuThreads=3 --blocks=8 --threads=64 >> at30_cpu3_200ms.txt
#mpirun -np 4 mcts --benchIter=250 --time=200 --cpuThreads=4 --blocks=16 --threads=64 >> at30_cpu4_200ms.txt
#mpirun -np 5 mcts --benchIter=250 --time=200 --cpuThreads=5 --blocks=32 --threads=64 >> at30_cpu5_200ms.txt
#mpirun -np 6 mcts --benchIter=250 --time=200 --cpuThreads=6 --blocks=60 --threads=64 >> at30_cpu6_200ms.txt
#mpirun -np 7 mcts --benchIter=250 --time=200 --cpuThreads=7 --blocks=2 --threads=32 >> at30_cpu7_200ms.txt
#mpirun -np 8 mcts --benchIter=250 --time=200 --cpuThreads=8 --blocks=4 --threads=64 >> at30_cpu8_200ms.txt
#mpirun -np 9 mcts --benchIter=250 --time=200 --cpuThreads=9 --blocks=8 --threads=64 >> at30_cpu9_200ms.txt
#mpirun -np 10 mcts --benchIter=250 --time=200 --cpuThreads=10 --blocks=16 --threads=64 >> at30_cpu10_200ms.txt
#mpirun -np 11 mcts --benchIter=250 --time=200 --cpuThreads=11 --blocks=32 --threads=64 >> at30_cpu11_200ms.txt
#mpirun -np 12 mcts --benchIter=250 --time=200 --cpuThreads=12 --blocks=60 --threads=64 >> at30_cpu12_200ms.txt

