import numpy as np
print("[Info] Loading nn module")
from nn import TFProcess
network=TFProcess(False, True)
print("[Info] nn module loaded")

raw_input=np.zeros([1,2,225])
print(network.forward(raw_input))
