import serial #导入模块
import threading
import sys
import time
import pandas
import numpy

#cmdstr="bias 6 32767 32767 32767 32767 10 37128 100 1 30000 40000 88."
#              i     q     v     dsb   (ch dithev ditherdiff)

cmdstr="bias 6 21773 14929 24980 32767  0 21773 0 1 30000 40000 88;"
cmdstr="bias 6 21773 14929 24980 32767  2 14929 0 1 30000 40000 88;"
cmdstr="bias 6 22987 14953 24518 32767  2 14953 1000 1 30000 40000 88;"

#cmdstr="bias 7."
#cmdstr="fft."

fs=160000000/1875;

#打开串口
# 端口，GNU / Linux上的/ dev / ttyUSB0 等 或 Windows上的 COM3 等
# 波特率，标准值之一：50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200
# 超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
def DOpenPort(portx,bps,timeout):
    ret=False
    try:
        # 打开串口，并得到串口对象
        ser = serial.Serial(portx, bps, timeout=timeout)
        #判断是否打开成功
        if(ser.is_open):
           ret=True
    except Exception as e:
        print("---异常---：", e)
    return ser,ret

#关闭串口
def DColsePort(ser):
    ser.close()

#写数据
def DWritePort(ser,text):
    wtmp = text.encode("gbk");
    print(wtmp)
    result = ser.write(wtmp)  # 写数据
    return result

def dealstr(str, splitbyte, savefile, step):
    #print(str)
    # 1
    strlist = str.split(splitbyte)    # 用逗号分割str字符串，并保存到列表
    strlist = strlist[1:-1]
    x = pandas.DataFrame(numpy.arange(0, step*len(strlist), step),columns=['xdata'])
    y = pandas.DataFrame(strlist,columns=['ydata'])
    data = pandas.merge(x,y,left_index=True, right_index=True, how='outer')
    print(data[81:84])
    data.to_csv(savefile)

#读数据
def DReadPort(ser):
    ret = ser.read(ser.in_waiting).decode("gbk")
    for num in range(0,100):
        time.sleep(0.2)
        if (ser.in_waiting > 0):
            ret = ret + ser.read(ser.in_waiting).decode("gbk")
        else:
            break;
    return ret

def DReadPortV20(ser):
    ret = ser.read(ser.in_waiting).decode("gbk")
    flg = False;
    for num in range(0, 100):
        time.sleep(0.02)
        if (ser.in_waiting > 0):
            ret = ret + ser.read(ser.in_waiting).decode("gbk")
            flg = True;
            break;

    count = 0;
    while flg:
        if (ser.in_waiting > 0):
            ret = ret + ser.read(ser.in_waiting).decode("gbk")
            count = 0;
        else:
            count = count+1;

        if (count > 100):
            break;
        time.sleep(0.01)
    return ret

if __name__=="__main__":
    print("cmdlen", len(sys.argv))
    if ( 2 <= len(sys.argv)):
        ser,ret=DOpenPort(sys.argv[1],115200,None)
    else:
        ser,ret=DOpenPort("COM21",115200,None)

    if ( 3 == len(sys.argv)):
        cmdstr = sys.argv[2]

    if(ret==True):#判断串口是否成功打开
        #count=DWritePort(ser, "tgon 25000 100.")
        #strtmp = DReadPort(ser)
        #print("aaaaaaaaaa", strtmp)
        #time.sleep(1)
        count=DWritePort(ser, cmdstr)
        print("写入字节数：",count)
        strtmp = DReadPort(ser)
        print(len(strtmp), strtmp)
        print(strtmp[-200:])
        dealstr(strtmp, ';', "src.csv", 1/fs)
        dealstr(strtmp, '|', "fft1.csv", fs/4096)

        timestr = time.strftime("%Y%m%d%H%M%S", time.localtime())
        dealstr(strtmp, ';', "%s_src.csv" % (timestr), 1/fs)
        dealstr(strtmp, '|', "%s_fft.csv" % (timestr), fs/4096)
        DColsePort(ser)
