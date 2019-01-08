# mcts_mpi
Monte Carlo Tree Search with MPI

http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/phd_thesis.pdf
http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/gpu_mcts_slides.pdf


As a PhD student I was a member of ULP-HPC (Ultra Low-Power, High-Performance Computing via Modeling and Optimization of Next Generation HPC Technologies) project. My PhD dissertation described a very effective parallelization scheme a novel, approximate Monte Carlo Tree Search (MCTS) algorithm. It is a method for making optimal decisions in artificial intelligence (AI) problems, typically move planning in combinatorial games. It combines the generality of random simulation with the precision of tree search. I have thoroughly investigated the problems related to parallelizing the algorithm and successively implemented a parallel multi-GPU version of the MCTS algorithm using CUDA on the TSUBAME 2.0 supercomputer. I was able to scale it up to 256 GPUs and 2048 CPUs showing tremendous improvement over the sequential program or any other existing work at that time. In addition to that, I analyzed the factors affecting the overall performance of the parallel version and the limitations of its scalability.Implementation and analysis of large scale parallel tree searching algorithms on GPU - Minimax, MCTS (Monte Carlo Tree Search) using CUDA and MPI. Analysis of power-usage related issues.

mpi
The motivation behind this work was caused by the emerging GPU-based supercomputer systems and their high computational potential combined with relatively low power usage compared to CPUs. As a problem to be solved I chose an AI GPU-based agent in the game of Reversi (Othello) which provides a sufficiently complex problem for tree searching with non-uniform structure.

tree
 

The research covered areas such as: Artificial Intelligence, Tree Search, Monte Carlo/Random methods, Parallel processing, General Purpose GPU Programming (GPGPU)
