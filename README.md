# Distributed GPU Monte Carlo Tree Search

# TL;DR
###### Solves checkers/reversi (why didn't I do GO?)
###### Includes **self-play**

<img src="http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/tree.png" width="200" /> <img src="http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/research/mcts.png" width="300" />

###### Runs on GPU (CUDA) or CPU (C++)

![alt text](http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/research/phd_3.png "REVERSI")

###### Over 1 million game simulations/second on a single 8 year old 280 GTX GPU.

###### GPU version is quite efficient actually ( here is the score advantage plotted on the y-axis vs time, playing againt a single-core CPU)

<img src="http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/research/score.png" width="600" />

###### Scales up nicely using MPI (Message Passing Interface) to a large distributed system (tested on a 2048-node supercomputer, up to 3.5M GPU threads)

<img src="http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/research/phd_4.png" width="600" />

###### Has a very minimal ssh-friendly interface

<img src="http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/research/mcts_git.gif" width="400" />

# I used this code while working on my PhD thesis. The MPI version has been tested on the Japanese TSUBAME supercomputer.
### THESIS + BIBTEX
[Thesis](http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/phd_thesis.pdf)
[Slides](http://olab.is.s.u-tokyo.ac.jp/~kamil.rocki/gpu_mcts_slides.pdf)

```
