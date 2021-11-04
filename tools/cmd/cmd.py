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
parser.add_argument("-port", help="串口端口 %(default)s)", default="COM12")
parser.add_argument("-garr", help="生成波形的arr %(default)s", default=42, type=int)
parser.add_argument("-gpsc", help="生成波形的psc %(default)s", default=2, type=int)
parser.add_argument("-carr", help="采集波形的arr %(default)s", default=100, type=int)
parser.add_argument("-cpsc", help="采集波形的psc %(default)s", default=168, type=int)
parser.add_argument("-capname", help="记录波形名字 %(default)s", default="mz2\\cap_20210809155738.csv")
args = parser.parse_args()

def gen():
    N = 1000
    tt = np.arange(0,N)
    xx = 50*np.cos(2*np.pi*tt/N)
    xx = xx + 50*np.cos(2*np.pi*tt/N*2)
    xx = xx + 127
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
    ff = 84000000 / (args.garr*args.gpsc*N)
    print("ff =", ff);
    count=serif.DWritePort(ser, "gen 2 %d %d;" % (args.garr-1,args.gpsc-1))
    strtmp = serif.DReadPort(ser)
    print(strtmp)
    count=serif.DWritePort(ser, "gen 100;")
    strtmp = serif.DReadPort(ser)
    print(strtmp)
    return


def draw(filestr, flg, Fs):
    data = pd.read_csv(filestr, encoding='gbk')
    print(data)
    Ns = len(data)
    print("Fs =", Fs, "Ns =", Ns);
    xdata = data.loc[:,"x"]
    ydata = data.loc[:,"y"]
    xdata = xdata/Fs;
    ydata = ydata/4096*3.3;
    plt.plot(xdata,ydata)
    if (flg == 1):
        plt.show()

    fftytmp=fft(ydata.values)
    # 画幅度谱
    ffty=np.abs(fftytmp)
    ffty= (ffty/Ns)*2;
    Ns = len(ffty)
    fftt=np.arange(0, Fs, Fs/Ns) 
    endpoint = int(len(fftt)/2)
    plt.plot(fftt[1:endpoint], ffty[1:endpoint])
    if (flg == 1):
        plt.show()

    # 画相位谱
    ffta = np.angle(fftytmp);
    plt.plot(fftt[1:endpoint], ffta[1:endpoint])
    if (flg == 1):
        plt.show()

def cap():
    filestr = "data\\cap_%s.csv" % (time.strftime("%Y%m%d%H%M%S", time.localtime()));
    f = open(filestr, "wb")

    ff = 168000000 / (args.carr*args.cpsc)
    count=serif.DWritePort(ser, "cap 2 %d %d;" % (args.carr-1, args.cpsc-1))
    strtmp = serif.DReadPort(ser)
    print(strtmp)

    count=serif.DWritePort(ser, "cap 0;")
    strtmp = serif.DReadPortV20(ser)
    #print(strtmp)
    strlist = strtmp.split('adcv:\r\n')
    #print(strlist[1]);
    f.write(str("x,y\r\n").encode("gbk"))
    f.write(strlist[1].encode("gbk"))
    f.close()
    draw(filestr, 1, ff)
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