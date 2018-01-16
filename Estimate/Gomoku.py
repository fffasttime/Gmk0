#Gomoku data
import numpy as np
import random
import ctypes
import operator
from math import sqrt
from io import *

BSIZE=15
BLSIZE=BSIZE*BSIZE
puct=1.4

print("[Info] Loading nn module")
from nn import TFProcess
network=TFProcess(False)
print("[Info] nn module loaded")

def inborder(dx, dy):
    return dx>=0 and dy >=0 and dx <BSIZE and dy<BSIZE

class MCTS:
    run_cnt=800;
    
    def run(self,_board, _nowcol):
        self.board=_board
        self.nowcol=_nowcol
        self.size=0;
        #cnt, vsum, child, fa, prob, move[225]
        self.node=[[0,0.0,[], -1, 0, -1]]
        for iter in range(self.run_cnt):
            nnode=0
            while True:
                maxv=-100.0 
                maxc=-1
                for ch in self.node[nnode][2]:
                    ucb=0
                    if self.node[ch][0]==0:
                        ucb=puct*self.node[ch][4]*sqrt(self.node[nnode][0])
                    else:
                        ucb=self.node[ch][1]/self.node[ch][0] + \
                            puct*self.node[ch][4]*sqrt(self.node[nnode][0]/(self.node[ch][0]+1))
                    if ucb>maxv:
                        maxv=ucb
                        maxc=ch
                if maxc==-1:
                    self.simulation_back(nnode)
                    break
                nnode=maxc
                self.make_move(self.node[nnode][5])
        count=[0] * 225
        blief=[0.0] * 225
        for ch in self.node[0][2]:
            count[self.node[ch][5]]=self.node[ch][0]
            if (self.node[ch][0]!=0):
                blief[self.node[ch][5]]=self.node[ch][1]/self.node[ch][0]
        return blief, np.array(count)
        """
        maxv=-100
        maxc=-1
        for ch in self.node[0][2]:
            if self.node[ch][1]>maxv:
                maxv=self.node[ch][0]
                maxc=self.node[ch][5]
        return maxc
        """
    def make_move(self, move):
        self.board[move//15][move%15]=self.nowcol
        self.nowcol=self.nowcol % 2 +1

    def unmake_move(self, move):
        self.board[move//15][move%15]=0
        self.nowcol=self.nowcol % 2 +1

    def judge_win(self):
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

    def get_value(self):
        ret= self.judge_win()
        if ret:
            return -1 if ret==self.nowcol else 1
        return 0

    def get_prob(self):
        ret=np.ndarray([BLSIZE], float)
        ret.fill(1)
        rcc=225
        for i in range(0,BSIZE):
            for j in range(0,BSIZE):
                if self.board[i][j]:
                    rcc-=1
                    ret[i*BSIZE+j]=0
        ret/=rcc
        return ret,rcc

    def run_net(self):
        raw_input=np.ndarray([1,2,BLSIZE])
        lboard=self.board.reshape([BLSIZE])
        raw_input[0][0]=(lboard==1).astype(float)
        raw_input[0][1]=(lboard==-1).astype(float)
        y,z=network.forward(raw_input)
        return y.reshape([BLSIZE]), z[0][0]
    
    def simulation_back(self, fa):
        probs,value=self.run_net()
        rcc=BLSIZE
        plist=sorted(enumerate(probs), key=operator.itemgetter(1),reverse=True)
        #value=self.get_value()
        plist=plist[0:rcc]
        #print(plist)
        for ch in plist:
            node=[0, 0.0, [], fa, ch[1], ch[0]]
            self.node[fa][2].append(len(self.node))
            self.node.append(node)
        self.node[fa][1]+=value
        self.node[fa][0]+=1
        while fa>0:
            self.unmake_move(self.node[fa][5])
            fa=self.node[fa][3]
            value=-value
            self.node[fa][1]+=value
            self.node[fa][0]+=1

class Gomoku:
    board=np.ndarray([BSIZE,BSIZE],int)
    nowcol=1
    movelist=[]
    def __init__(self):
        self.movelist=[]
        self.board.fill(0)

    def setpiece(self,dx, dy):
        if self.board[dx][dy]!=0 :
            return False
        self.board[dx][dy]=self.nowcol
        self.nowcol=(self.nowcol % 2) + 1
        self.movelist.append([dx, dy])
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
    blieflist=[]
    def runStep(self):
        mc=MCTS()
        blief, counts=mc.run(self.board.copy(), self.nowcol)
        print(counts)
        counts = np.power(counts, 2)
        self.blieflist.append(blief)
        counts=counts/counts.sum()
        ret=np.argmax(counts)
        #ret=(random.choices(range(225),counts))[0]
        return ret//15, ret%15
        #run=lib.run
        #run.restype = ctypes.c_char_p
        #print(run(toString))

    def selfplay(self):
        winner=0
        while True:
            mx,my = self.runStep()
            if not self.setpiece(mx,my):
                winner=self.nowcol % 2 +1
                break
            if self.judgewin():
                winner=self.nowcol
                break
            print(self.board)
        
        fout=open('data.txt', 'a')
        fout.write(str(self.movelist))
        fout.close()
