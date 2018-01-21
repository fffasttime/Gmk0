import numpy as np
import time
from nn import TFProcess
net=TFProcess(False, True)
print("[INFO] Module Estimate loaded")

def test_add(a,b):
    print(a)
    return a,a+b

def nptest():
    input=np.zeros([1,2,225])
    net.forward(input)
    t0=time.clock()
    for i in range(1600):
        input=np.zeros([1,2,225])+1/(i+1000)
        y,z=net.forward(input)
    print(time.clock()-t0)
    t0=time.clock()
    for i in range(200):
        input=np.zeros([8,2,225])+1/(i+1000)
        y,z=net.forward(input)
    print(time.clock()-t0)
    t0=time.clock()
    for i in range(1600):
        input=np.zeros([8,2,225])+1/(i+1000)
        y,z=net.forward(input)
    print(time.clock()-t0)
    t0=time.clock()
    for i in range(1000):
        input=np.zeros([16,2,225])+1/(i+1000)
        y,z=net.forward(input)
    print(time.clock()-t0)
    for i in range(20):
        input=np.zeros([800,2,225])+1/(i+1000)
        y,z=net.forward(input)
    print(time.clock()-t0)
    
nptest()
