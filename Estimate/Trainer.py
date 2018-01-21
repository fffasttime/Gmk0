import numpy as np
from nn import TFProcess
from nn import batch_size


BSIZE=15

class GmkData:
    def __init__(self,filename):
        self.nowindex=0
        print("[Info] Start loading data from " + filename)
        td=np.loadtxt(filename,dtype=np.float32)
        print("[Info] Loaded, reshape data...")
        np.random.shuffle(td)
        tcol=td[:,0:BSIZE*BSIZE*2]
        tp=td[:,BSIZE*BSIZE*2:BSIZE*BSIZE*3]
        tval=td[:,BSIZE*BSIZE*3:BSIZE*BSIZE*3+1]
        
        tcol=tcol.reshape([-1,2, BSIZE*BSIZE])
        tp=tp.reshape([-1, BSIZE*BSIZE])
        tval=tval.reshape([-1,1])
        
        self.ecnt=td.shape[0]
        print("[Info] %d data loaded"%(self.ecnt))
        self.data=[tcol,tp,tval]
    def next_batch(self, batchsize):
        s1=[x[self.nowindex:self.nowindex+batchsize] for x in self.data]
        self.nowindex+=batchsize
        if self.nowindex>self.ecnt:
            self.nowindex%=self.ecnt
            return [np.vstack((s1[i],self.data[i][:self.nowindex])) for i in range(3)]
        else:
            return s1

def main():
    trainer=TFProcess(True)
    data=GmkData("data/gmkdata.txt")
    while trainer.process(data.next_batch(batch_size)):
        pass
main()
