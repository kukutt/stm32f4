import matplotlib.pyplot as plt
from scipy import signal
from scipy.fftpack import fft,ifft
from scipy.io import loadmat
import pandas as pd
import time
import random
import numpy as np
import sys
import os
import serif
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("m", choices = ['test', 'gen', 'cap'])
parser.add_argument("-port", help="串口端口", default="COM12")
parser.add_argument("-capname", help="记录波形名字", default="mz2\\cap_20210809155738.csv")
args = parser.parse_args()

def gen():
    N = 1000
    tt = np.arange(0,N)
    xx = 127*np.cos(2*np.pi*tt/N)+127
    xx = xx.astype("uint8")
    #print(xx.dtype, len(xx))
    for iii in range(0,len(xx),50):
        count=serif.DWritePort(ser, "gen 0 %d 50 %s;" % (tt[iii], xx[iii:iii+50].tobytes().hex()))
        strtmp = serif.DReadPort(ser)
        print(strtmp)
    count=serif.DWritePort(ser, "gen 1;")
    strtmp = serif.DReadPort(ser)
    tmponestr = strtmp.splitlines()
    print(len(tmponestr))
    for iii in range(2, len(tmponestr), 1):
        if (int(tmponestr[iii]) != xx[iii-2]):
            print("error", iii, tmponestr[iii], xx[iii]);
            return
    arr = 42
    psc = 2
    ff = 84000000 / (arr*psc*N)
    print("ff =", ff);
    count=serif.DWritePort(ser, "gen 2 %d %d;" % (arr-1,psc-1))
    strtmp = serif.DReadPort(ser)
    print(strtmp)
    count=serif.DWritePort(ser, "gen 100;")
    strtmp = serif.DReadPort(ser)
    print(strtmp)
    return


def draw(filestr, flg):
    data = pd.read_csv(filestr, encoding='gbk')
    print(data)
    xdata = data.loc[:,"x"]
    ydata = data.loc[:,"y"]
    plt.plot(xdata,ydata)
    if (flg == 1):
        plt.show()

def cap():
    filestr = "data\\cap_%s.csv" % (time.strftime("%Y%m%d%H%M%S", time.localtime()));
    f = open(filestr, "wb")
    count=serif.DWritePort(ser, "cap;")
    strtmp = serif.DReadPortV20(ser)
    #print(strtmp)
    strlist = strtmp.split('adcv:\r\n')
    print(strlist[1]);
    f.write(str("x,y\r\n").encode("gbk"))
    f.write(strlist[1].encode("gbk"))
    f.close()
    draw(filestr, 1)
    return filestr

def test():
    N = 1024
    tt = np.arange(0,N)
    xx = np.cos(2*np.pi*tt/N)
    ss = np.sin(2*np.pi*tt/N)
    plt.plot(xx)
    plt.show()
    ff = fft(xx)
    cc = xx * ss
    plt.plot(cc)
    plt.show()
    print(cc.sum())
    print(ff[1])

def beforeinit():
    path="data"
    isExists=os.path.exists(path)
    if not isExists:
        os.makedirs(path) 
        print(path,' 创建成功')
        return True
    else:
        print(path,' 目录已存在')
        return False

if __name__ == "__main__":

    print(args.m)
    beforeinit()

    exitflg = 1;
    if (args.m == "test"):
        test()
    else:
        exitflg = 0;

    if (exitflg == 1):
        os._exit(0)

    ser,ret=serif.DOpenPort(args.port,115200,None)
    if (args.m == "gen"):
        gen();
    elif (args.m == "cap"):
        cap();
    else:
        print("error")

    serif.DColsePort(ser)