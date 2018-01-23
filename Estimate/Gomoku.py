#Gomoku data
import numpy as np
import random
import ctypes
import operator
from math import sqrt
from io import *

BSIZE=15
BLSIZE=BSIZE*BSIZE
BIGVALUE=1000

puct=2

print("[Info] Loading nn module")
from nn import TFProcess
network=TFProcess(False, False)
print("[Info] nn module loaded")

def inborder(dx, dy):
    return dx>=0 and dy >=0 and dx <BSIZE and dy<BSIZE

def printb(board):
    for i in range(BSIZE):
        print(str(board[i]))

class MCTS:
    run_cnt=80;
    
    def run(self,_board, _nowcol):
        self.board=_board
        self.nowcol=_nowcol
        self.size=0;
        self.globalstep=0
        #vcnt, vsum, child, fa, prob(225f), move, movech(225i), isEnd
        self.node=[]
        self.simulation_back(-1,0)
        
        for iter in range(self.run_cnt):
            self.globalstep=iter + 1
            nnode=0
            while True:
                cur=self.node[nnode]
                #use numpy to make judge faster
                fvis=sqrt(cur[0])
                ucbs=np.zeros([BLSIZE])
                #ucbs.fill(-cur[1]/cur[0])
                vcnts=np.zeros([BLSIZE])
                for ch in cur[2]:
                    move=self.node[ch][5]
                    ucbs[move]=self.node[ch][1]/self.node[ch][0]
                    vcnts[move]=self.node[ch][0]
                ucbs += puct * sqrt(cur[0]) * cur[4] / (1 + vcnts)
                #delete invalid move
                ucbs-=(self.board>0).reshape([BLSIZE]).astype(float)*BIGVALUE
                maxmove=np.argmax(ucbs)
                #maxv=ucbs[maxmove]
                maxc=cur[6][maxmove]
                self.make_move(maxmove)
                if maxc==0 or self.node[maxc][7]:
                    self.simulation_back(nnode,maxmove)
                    break
                nnode=maxc
        count=[0] * 225
        blief=[0.0] * 225
        for ch in self.node[0][2]:
            count[self.node[ch][5]]=self.node[ch][0]
            '''
            if (self.node[ch][0]!=0):
                blief[self.node[ch][5]]=self.node[ch][1]/self.node[ch][0]
                print(self.node[ch][1]/self.node[ch][0])
            '''
        return np.array(count)
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
        if self.nowcol==1:
            raw_input[0][0]=(lboard==1).astype(float)
            raw_input[0][1]=(lboard==2).astype(float)
        else:
            raw_input[0][0]=(lboard==2).astype(float)
            raw_input[0][1]=(lboard==1).astype(float)
        
        y,z=network.forward(raw_input)
        return y.reshape([BLSIZE]), z[0][0]
    
    def simulation_back(self, fa, move):
        ret=self.judge_win()
        isend=False
        if ret:
            value=-1 if self.nowcol==ret else 1
            probs=np.zeros([BLSIZE])
            isend=True
        else:
            probs,value=self.run_net()
            value=-value
            
        #isend
        if fa>=0 and self.node[fa][6][move]>0:
            cur=self.node[fa][6][move]
            self.node[cur][0]+=1
            self.node[cur][1]+=value
            self.unmake_move(move)
            self.backprop(fa, value)
            return
        
        if self.globalstep==0:
            pass
            #print(probs)
            #print(value)
        if (fa>=0):
            self.unmake_move(move)
        node=[1, value, [], fa, probs, move, np.zeros(BLSIZE, int), isend]
        trcnt=len(self.node)
        if fa>=0:
            self.node[fa][2].append(trcnt)
            self.node[fa][6][move]=trcnt
        
        self.node.append(node)

        self.backprop(fa, value)

    def backprop(self, fa, value):
        while fa>=0:
            if fa>0:
                self.unmake_move(self.node[fa][5])
            value=-value
            self.node[fa][1]+=value
            self.node[fa][0]+=1
            fa=self.node[fa][3]

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
        #if self.nowcol==1:
        #    mc.run_cnt=2
        counts=mc.run(self.board.copy(), self.nowcol)
        counts = np.power(counts, 1)
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
                fout.write(str(self.decisionlist[i][j])+' ')
            fout.write('\n')
        fout.write(str(winner))
    
    def selfplay(self):
        winner=0
        while True:
            mx,my = self.runStep()
            print(mx, my)
            if not self.setpiece(mx,my):
                winner=self.nowcol % 2 +1
                break
            if self.judgewin():
                winner=self.nowcol
                break
        print("col", winner, "win")
        self.writeData(winner)
