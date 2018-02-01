from graphics import *
from math import *
import numpy as np
from Gomoku import *
import random
WIN_SIZE=300
SPACE_SKIP=WIN_SIZE/BSIZE

def setupwindow():
    window=GraphWin("this is a gomoku game", WIN_SIZE, WIN_SIZE)
    
    return window

def initboard(window):
    #window.setBackground("gray")
    background=Rectangle(Point(0,0),Point(WIN_SIZE,WIN_SIZE))
    background.setFill('gray')
    background.draw(window)
    c,i=0,SPACE_SKIP/2
    
    while c <= BSIZE:
        l=Line(Point(i,SPACE_SKIP/2),Point(i,WIN_SIZE-SPACE_SKIP/2))
        l.draw(window)
        i+=SPACE_SKIP
        c+=1
    c,i=0,SPACE_SKIP/2
    while c <= BSIZE:
        l=Line(Point(SPACE_SKIP/2,i),Point(WIN_SIZE-SPACE_SKIP/2,i))
        l.draw(window)
        i+=SPACE_SKIP
        c+=1

def runGUI():
    window = setupwindow()
    while True:
        initboard(window)
        game = Gomoku()
        #game.selfplay()
        #game.initoutAI()
        col=1
        user_inputw=False
        user_inputb=False
        while True:
            cx,cy=0,0
            #game.runAI()
            if user_inputw and col==2 or user_inputb and col==1:
                try:
                    p1 = window.getMouse()
                except Exception as e:
                    return
                cy = round((p1.getX()-SPACE_SKIP/2)/SPACE_SKIP)
                cx = round((p1.getY()-SPACE_SKIP/2)/SPACE_SKIP)
            else:
                cx,cy=game.runStep(True, 2)
                p1 = window.getMouse()
            if not inborder(cx,cy): continue
            if not game.setpiece(cx,cy): continue
            piece = Circle(Point(SPACE_SKIP*(cy+0.5),SPACE_SKIP*(cx+0.5)),8)
            if col==2: piece.setFill('white')
            else: piece.setFill('black')
            piece.draw(window)
            if game.judgewin() !=0:
                break
            col=(col % 2) + 1
        if col==1:
            message = Text(Point(150,150),"Black win!")
        else:
            message = Text(Point(150,150),"White win!")
        message.setSize(30)
        message.setFill('white')
        message.draw(window)
        #game.writeData(col)
        try:
            window.getMouse()
        except Exception as e:
            return
    window.close()

def selfplay():
    for i in range(7012):
        print("Game",i)
        game = Gomoku()
        game.selfplay()

def selfmatch():
    win1, win2=0.0,0.0
    for i in range(50):
        print("Game",i)
        game = Gomoku()
        winner=game.selfmatch(1)
        if winner==1:
            win1+=1
        elif winner==0:
            win1+=0.5
    for i in range(50):
        print("Game",i)
        game = Gomoku()
        winner=game.selfmatch(2)
        if winner==2:
            win2+=1
        elif winner==0:
            win2+=0.5
    print(win1, win2)

#selfmatch()
selfplay()
#runGUI()
