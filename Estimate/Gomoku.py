#Gomoku data
import numpy as np
import random
import ctypes
import operator
from io import *

from Search import MCTS

BSIZE=15
BLSIZE=BSIZE*BSIZE

def inborder(dx, dy):
    return dx>=0 and dy >=0 and dx <BSIZE and dy<BSIZE

def printb(board):
    for i in range(BSIZE):
        print(str(board[i]))

def strpos(mx, my):
    return chr(mx+65)+str(my)

class Gomoku:
    def __init__(self):
        self.nowcol=1
        self.movelist=[]
        self.decisionlist=[]
        self.board=np.ndarray([BSIZE,BSIZE],int)
        self.board.fill(0)

    def setpiece(self,dx, dy):
        if self.board[dx][dy]!=0 :
            return False
        self.board[dx][dy]=self.nowcol
        self.nowcol=(self.nowcol % 2) + 1
        return True

    def judgewin(self):
        for i in range(4,BSIZE):
            for j in range(BSIZE):
                if self.board[i][j]!=0:
                    pcol=self.board[i][j]
                    if self.board[i-1][j]==pcol and self.board[i-2][j]==pcol and self.board[i-3][j]==pcol and self.board[i-4][j]==pcol:
                        return pcol
        for i in range(BSIZE):
            for j in range(4,BSIZE):
                if self.board[i][j]!=0:
                    pcol=self.board[i][j]
                    if self.board[i][j-1]==pcol and self.board[i][j-2]==pcol and self.board[i][j-3]==pcol and self.board[i][j-4]==pcol:
                        return pcol
        for i in range(4,BSIZE):
            for j in range(4,BSIZE):
                if self.board[i][j]!=0:
                    pcol=self.board[i][j]
                    if self.board[i-1][j-1]==pcol and self.board[i-2][j-2]==pcol and self.board[i-3][j-3]==pcol and self.board[i-4][j-4]==pcol:
                        return pcol
        for i in range(4,BSIZE):
            for j in range(0,BSIZE-4):
                if self.board[i][j]!=0:
                    pcol=self.board[i][j]
                    if self.board[i-1][j+1]==pcol and self.board[i-2][j+2]==pcol and self.board[i-3][j+3]==pcol and self.board[i-4][j+4]==pcol:
                        return pcol
        return 0

    def random_play(self):
        count=0
        availplace=np.ndarray([BSIZE,BSIZE],bool)
        availplace.fill(False)
        for i in range(BSIZE):
            for j in range(BSIZE):
                if self.board[i][j]!=0 : 
                    count+=1
                    for k in range(-3,4):
                        for l in range(-3,4):
                            if inborder(i+k,j+l):
                                availplace[i+k][j+l]=True
        if count==0: return 7,7
        rlist=[]
        for i in range(BSIZE):
            for j in range(BSIZE):
                if availplace[i][j] and self.board[i][j]==0:
                    rlist.append((i,j))
        r=random.randint(0, len(rlist)-1)
        return rlist[r]

    def initoutAI(self):
        self.lib = ctypes.cdll.LoadLibrary("gmk.dll")
    
    def toString(self):
        s=str(len(movelist)) + ' '
        for i in movelist:
            s+= str(i[0]) + ' ' + str(i[1]) + ' '
        return s
    
    def runStep(self):
        mc=MCTS()
        counts=mc.run(self.board.copy(), self.nowcol)
        #print(counts)
        if np.sum((self.board>0).astype(float)) >= 25:
            te=2
        else:
            te=1
        #print(counts)
        counts = np.power(counts, te)
        counts=counts/counts.sum()
        #ret=np.argmax(counts)
        ret=(random.choices(range(225),counts))[0]
        self.decisionlist.append(counts)
        self.movelist.append(ret)
        return ret//15, ret%15

    def writeData(self,winner):
        fout=open("data/selfdata.txt","a")
        fout.write(str(len(self.movelist))+'\n')
        for i,x in enumerate(self.movelist):
            fout.write(str(x) + ' ')
            for j in range(BLSIZE):
                fout.write(str(round(self.decisionlist[i][j], 6))+' ')
            fout.write('\n')
        fout.write(str(winner)+'\n')
    
    def selfplay(self):
        winner=0
        while True:
            mx,my = self.runStep()
            print(strpos(mx, my), end=' ')
            if not self.setpiece(mx,my):
                winner=self.nowcol % 2 +1
                break
            if self.judgewin():
                winner=self.nowcol % 2 +1
                break
        print("\ncol", winner, "win")
        self.writeData(winner)

    def selfmatch(self):
        winner=0
        while True:
            mx,my = self.runStep()
            print(strpos(mx, my), end=' ')
            if not self.setpiece(mx,my):
                winner=self.nowcol % 2 +1
                break
            if self.judgewin():
                winner=self.nowcol % 2 +1
                break
        print("\ncol", winner, "win")
        return winner
        
