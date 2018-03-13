# Gmk0
Learn playing gomoku from zero.

### Result of training
Training is available. Test training has started.
See [improve.md](./improve.md).
- After playing 150'000 games, Gmk0 is as strong as an ordinary alphabeta algorithm program. 
- But it's still far away from top level AI (3-27 Yixin16).

### Python version
Run pysrc/Main.py to selfplay or play with human,
or run pysrc/Trainer.py to train a nn from selfplay data.

require:
- python3.6
- numpy-1.13
- tensorflow-1.4.0 or tensorflow-gpu-1.4.0

### cpp version
cpp version is faster when selfplay.
compile and run 
Gmk0.exe --mode selfplay --network "weight.txt" --playout 400
to generate selfplay data


![image](./selfplay.png)

The neural network need some library, see cppsrc/NN/README.md
