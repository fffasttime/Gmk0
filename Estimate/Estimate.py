import numpy as np
from nn import TFProcess

def test_add(a,b):
    print(a)
    return a,a+b

def nptest(input):
    net=TFProcess()
    y,z=net.forward(input)
    return y, z

print("[INFO] Module Estimate loaded")