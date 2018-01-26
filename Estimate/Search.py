#search
import numpy as np
import random
import ctypes
import operator
from math import sqrt

BSIZE=15
BLSIZE=BSIZE*BSIZE

from Gomoku import *

print("[Info] Loading nn module")
from nn import TFProcess
network=TFProcess(False, True)
print("[Info] nn module loaded")

BIGVALUE=1000
puct=2.5

def add_dirlect_noise(policy, epsilon, alpha):
    dirlect=np.random.dirichlet([alpha] * BLSIZE)
    policy=(1-epsilon)*policy + epsilon*dirlect
    return policy

class MCTS:
    run_cnt=120;
    max_thread=6
    
    def run(self,_board, _nowcol):
        self.board=_board
        self.nowcol=_nowcol
        self.startcol=_nowcol;
        self.size=0;
        self.globalstep=0
        #vcnt, vsum, child, fa, prob(225f), move, movech(225i), isEnd
        self.node=[]
        self.create_root()

        #first 10 nodes use 1 thread
        for i in range(min(range(10), run_cnt)):
            self.run_batch(1)
        if run_cnt>10:
            for i in range((run_cnt-10)//max_thread+1):
                self.run_batch(max_thread)
        
        #print(-self.node[0][1]/self.node[0][0])
        count=[0] * 225
        for ch in self.node[0][2]:
            count[self.node[ch][5]]=self.node[ch][0]
            '''
            if (self.node[ch][0]!=0):
                print(self.node[ch][1]/self.node[ch][0])
            '''
        return np.array(count)
    
    def make_move_board(self, move, board):
        board[move//15][move%15]=self.nowcol
        self.nowcol=self.nowcol % 2 +1
        
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
    
    def dump_board(self):
        lboard=self.board.reshape([BLSIZE])
        if self.nowcol==1:
            self.raw_input[self.dump_c][0]=(lboard==1).astype(float)
            self.raw_input[self.dump_c][1]=(lboard==2).astype(float)
        else:
            self.raw_input[self.dump_c][0]=(lboard==2).astype(float)
            self.raw_input[self.dump_c][1]=(lboard==1).astype(float)
        self.dump_c+=1

    def run1(self, dumps):
        self.raw_input=np.ndarray([dumps,2,BLSIZE])
        simulatelist=[]
        for i in range(dumps):
            self.board_temp=self.board.copy()
            self.nowcol=self.nowcol_start
            nnode, maxmove, isend = self.selection()
            if isend:
                cur=self.node[nnode][7][maxmove]
                value=1
                self.node[cur][0]+=1
                self.node[cur][1]+=val
                backprob(nnode, value)
            else:
                r=self.judge_win()
                if r:
                    val = 1
                    self.node[nnode][1]+=BIGNUM
                    self.expand_back(nnode, maxmove, np.zeros([225]), val, True)
                else:
                    simulatelist.append((nnode, maxmove))
                    dump_board(self.board_temp)
        if len(simulatelist)>0:
            y,z=network.forward(self.raw_input[:len(simulatelist)])
        for i, item in enumerate(simulatelist):
            self.node[nnode][1]+=BIGNUM
            self.expand_back(item[0], item[1], y[i], z[i])
            
    def selection(self):
        nnode=0
        while True:
            cur=self.node[nnode]
            #use numpy to make judge faster
            fvis=sqrt(cur[0])
            ucbs=np.zeros([BLSIZE])
            ucbs.fill(-cur[1]/cur[0]/1.5)
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
            self.make_move_board(maxmove, self.board_temp)
            if maxc==0 or self.node[maxc][7]:
                if maxc==0:
                    self.node[maxc][1]-=BIGNUM  #virtual_loss
                return nnode, maxmove, self.node[maxc][7]
            nnode=maxc
    
    def run_net_1(self):
        if self.startcol==2:
            probs=np.zeros([BLSIZE])
            probs+=1/BLSIZE
            value=0
            return probs,value
        raw_input=np.ndarray([1,2,BLSIZE])
        lboard=self.board.reshape([BLSIZE])
        if self.nowcol==1:
            raw_input[0][0]=(lboard==1).astype(float)
            raw_input[0][1]=(lboard==2).astype(float)
        else:
            raw_input[0][0]=(lboard==2).astype(float)
            raw_input[0][1]=(lboard==1).astype(float)
        
        y,z=network.forward(raw_input)
        #print(z[0][0])
        return y.reshape([BLSIZE]), z[0][0]

    def create_root(self):
        probs,value=self.run_net_1()
        value=-value
        if 1:
            probs=add_dirlect_noise(probs, 0.25, 0.02)
        node=[1, value, [], -1, probs, -1, np.zeros(BLSIZE, int), False]
        self.node.append(node)
    
    def expand_back(self, fa, move, probs, value, isend):
        node=[1, value, [], fa, probs, move, np.zeros(BLSIZE, int), isend]
        trcnt=len(self.node)
        
        self.node[fa][2].append(trcnt)
        self.node[fa][6][move]=trcnt
        
        self.node.append(node)

        self.backprop(fa, value)

    def backprop(self, fa, value):
        while fa>=0:
            value=-value
            self.node[fa][1]+=value
            self.node[fa][0]+=1
            fa=self.node[fa][3]
