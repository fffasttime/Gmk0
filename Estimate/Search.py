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
network=TFProcess("./paras/I8/")
network_opp=TFProcess("./paras/I7/")
print("[Info] nn module loaded")

BIGVALUE=10000
puct=1.6

def add_dirlect_noise(policy, epsilon, alpha):
    dirlect=np.random.dirichlet([alpha] * BLSIZE)
    policy=(1-epsilon)*policy + epsilon*dirlect
    return policy

class MCTS:
    run_cnt=120
    max_thread=6
    
    def run(self,_board, _nowcol, _net=network):
        self.board=_board
        self.nowcol=_nowcol
        self.startcol=_nowcol;
        self.size=0;
        self.net=_net
        self.globalstep=0
        #vcnt, vsum, child, fa, prob(225f), move, movech(225i), isEnd
        self.node=[]
        self.create_root()

        #first 10 nodes use 1 thread
        for i in range(min(10, self.run_cnt)):
            self.run_batch(1)
        if self.run_cnt>10:
            for i in range((self.run_cnt-10)//self.max_thread+1):
                self.run_batch(self.max_thread)
        
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

    def judge_win(self, board):
        for i in range(4,BSIZE):
            for j in range(BSIZE):
                if board[i][j]!=0:
                    pcol=board[i][j]
                    if board[i-1][j]==pcol and board[i-2][j]==pcol and board[i-3][j]==pcol and board[i-4][j]==pcol:
                        return pcol
        for i in range(BSIZE):
            for j in range(4,BSIZE):
                if board[i][j]!=0:
                    pcol=board[i][j]
                    if board[i][j-1]==pcol and board[i][j-2]==pcol and board[i][j-3]==pcol and board[i][j-4]==pcol:
                        return pcol
        for i in range(4,BSIZE):
            for j in range(4,BSIZE):
                if board[i][j]!=0:
                    pcol=board[i][j]
                    if board[i-1][j-1]==pcol and board[i-2][j-2]==pcol and board[i-3][j-3]==pcol and board[i-4][j-4]==pcol:
                        return pcol
        for i in range(4,BSIZE):
            for j in range(0,BSIZE-4):
                if board[i][j]!=0:
                    pcol=board[i][j]
                    if board[i-1][j+1]==pcol and board[i-2][j+2]==pcol and board[i-3][j+3]==pcol and board[i-4][j+4]==pcol:
                        return pcol
        return 0

    def judge_win_move(self, board, move, scol):
        x0=move//BSIZE
        y0=move%BSIZE
        #row, col, diagonal
        c1,c2,c3,c4=0,0,0,0
        flag1,flag2,flag3,flag4=True, True, True, True
        for i in range(1,5):
            if flag1 and (y0+i>=BSIZE or board[x0][y0+i]!=scol):
                flag1=False
            if flag2 and (x0+i>=BSIZE or board[x0+i][y0]!=scol):
                flag2=False
            if flag3 and (x0+i>=BSIZE or y0+i>=BSIZE or board[x0+i][y0+i]!=scol):
                flag3=False
            if flag4 and (x0+i>=BSIZE or y0-i<0 or board[x0+i][y0-i]!=scol):
                flag4=False
            if flag1: c1+=1
            if flag2: c2+=1
            if flag3: c3+=1
            if flag4: c4+=1
        flag1,flag2,flag3,flag4=True, True, True, True
        for i in range(1,5):
            if flag1 and (y0-i<0 or board[x0][y0-i]!=scol):
                flag1=False
            if flag2 and (x0-i<0 or board[x0-i][y0]!=scol):
                flag2=False
            if flag3 and (x0-i<0 or y0-i<0 or board[x0-i][y0-i]!=scol):
                flag3=False
            if flag4 and (x0-i<0 or y0+i>=BSIZE or board[x0-i][y0+i]!=scol):
                flag4=False
            if flag1: c1+=1
            if flag2: c2+=1
            if flag3: c3+=1
            if flag4: c4+=1
        #link 5
        if c1>=4 or c2>=4 or c3>=4 or c4>=4:
            return True
        return False
            
    def get_value(self):
        ret= self.judge_win(self.board)
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
    
    def dump_board(self, board):
        lboard=board.reshape([BLSIZE])
        if self.nowcol==1:
            self.raw_input[self.dump_c][0]=(lboard==1).astype(float)
            self.raw_input[self.dump_c][1]=(lboard==2).astype(float)
        else:
            self.raw_input[self.dump_c][0]=(lboard==2).astype(float)
            self.raw_input[self.dump_c][1]=(lboard==1).astype(float)
        self.dump_c+=1

    def run_batch(self, dumps):
        self.raw_input=np.ndarray([dumps,2,BLSIZE])
        simulatelist=[]
        self.dump_c=0
        for i in range(dumps):
            self.board_temp=self.board.copy()
            self.nowcol=self.startcol
            nnode, maxmove, isend = self.selection()
            if isend:
                cur=self.node[nnode][6][maxmove]
                value=1
                self.node[cur][0]+=1
                self.node[cur][1]+=value
                self.backprop(nnode, value)
            else:
                #r=self.judge_win(self.board_temp)
                r=self.judge_win_move(self.board_temp, maxmove, self.nowcol%2+1)
                if r:
                    self.expand_back_end(nnode, maxmove)
                else:
                    self.expand_leaf(nnode, maxmove)
                    simulatelist.append((nnode, maxmove))
                    self.dump_board(self.board_temp)
        if self.dump_c>0:
            y,z=self.net.forward(self.raw_input[:self.dump_c])
        for i, item in enumerate(simulatelist):
            self.back_leaf(item[0], item[1], y[i], z[i][0])
            
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
                return nnode, maxmove, self.node[maxc][7]
            nnode=maxc
    
    def run_net_1(self):
        raw_input=np.ndarray([1,2,BLSIZE])
        lboard=self.board.reshape([BLSIZE])
        if self.nowcol==1:
            raw_input[0][0]=(lboard==1).astype(float)
            raw_input[0][1]=(lboard==2).astype(float)
        else:
            raw_input[0][0]=(lboard==2).astype(float)
            raw_input[0][1]=(lboard==1).astype(float)
        
        y,z=self.net.forward(raw_input)
        #print(z[0][0])
        return y.reshape([BLSIZE]), z[0][0]

    def create_root(self):
        probs,value=self.run_net_1()
        value=-value
        if 1:
            probs=add_dirlect_noise(probs, 0.25, 0.02)
        node=[1, value, [], -1, probs, -1, np.zeros(BLSIZE, int), False]
        self.node.append(node)

    def expand_back_end(self, fa, move):
        node=[1, 1, [], fa, np.zeros(BLSIZE), move, np.zeros(BLSIZE, int), True]
        trcnt=len(self.node)
        self.node[fa][2].append(trcnt)
        self.node[fa][6][move]=trcnt
        self.node.append(node)
        self.backprop(fa, 1)

    def expand_leaf(self, fa, move):
        #value use virtual loss
        node=[1, -BIGVALUE, [], fa, np.zeros(BLSIZE), move, np.zeros(BLSIZE, int), False]
        trcnt=len(self.node)
        self.node[fa][2].append(trcnt)
        self.node[fa][6][move]=trcnt
        self.node.append(node)
        
    def back_leaf(self, fa, move, probs, value):
        value=-value
        cur=self.node[fa][6][move]
        self.node[cur][1]=value
        self.node[cur][4]=probs
            
        self.backprop(fa, value)

    def backprop(self, fa, value):
        while fa>=0:
            value=-value
            self.node[fa][1]+=value
            self.node[fa][0]+=1
            fa=self.node[fa][3]
