package main

import "fmt"
//import "errors"
import "os"
import "io"
import "time"
import "github.com/tarm/goserial"
import "flag"
import "bytes"
//import "io/ioutil"
import "log"
import "strings"
import "encoding/hex"

const MAXRWLEN = 8000
const ADDR_APP = 0x4000
var port string
var file string
var baud int
var to int
var logger *log.Logger;
var excelFileName string


func serialrw(iorwc io.ReadWriteCloser, str string)(string){
    buffer := make([]byte, MAXRWLEN)

    if (iorwc == nil){
        return ""
    }

    //发命令之前清空缓冲区
    num, err := iorwc.Read(buffer)
    if err != nil {
        fmt.Println(err)
        return ""
    }

    fmt.Printf("s:%s\r\n", str)
    //发送命令
    num, err = iorwc.Write([]byte(str))
    if err != nil {
        fmt.Println(err)
        return ""
    }

    var tmpstr string = ""
    for i := 0; i < 3000; i++ {

        time.Sleep(time.Duration(to)*time.Millisecond);
        num, err = iorwc.Read(buffer)
        if num > 0 {
            tmpstr += fmt.Sprintf("%s", string(buffer[:num]))
        }else{
            break;
        }
    }

    //打印输出读到的信息
    //fmt.Printf("[%d:%d][%s]\r\n", num, len(tmpstr), tmpstr)
    fmt.Printf("r:%s\r\n", tmpstr)
    return tmpstr
}

func upgrade(iorwc io.ReadWriteCloser)(int){
    file, err := os.OpenFile(file, os.O_RDONLY, os.ModePerm)
    if err != nil {
        defer file.Close()
        os.Exit(0)
    }
    var buffer bytes.Buffer
    var recvbyte []uint8
    io.Copy(&buffer, file)
    _bytes := buffer.Bytes()
    strlen := len(_bytes)

    serialrw(iorwc, "iap_init;")

    /* erase */
    for i:=0; i < strlen; i+=512 {
        tmp := fmt.Sprintf("iap_erase %08x;", ADDR_APP+i)
        serialrw(iorwc, tmp);
    }

    /* write */
    for i:=0; i < strlen; i+=512 {
        tmp := fmt.Sprintf("iap_write %08x %s;", ADDR_APP+i, hex.EncodeToString(_bytes[i:i+512]))
        serialrw(iorwc, tmp);
    }

    /* read */
    for i:=0; i < strlen; i+=512 {
        tmp := fmt.Sprintf("iap_read %08x 512;", ADDR_APP+i)
        retstr := serialrw(iorwc, tmp);
        countSplit := strings.Split(retstr, " ")
        b, _ := hex.DecodeString(countSplit[3])
        recvbyte=append(recvbyte, b...)
    }

    if ((len(recvbyte) >= len(_bytes)) && (bytes.Equal(recvbyte[0:strlen], _bytes[0:strlen]))){
        return 0;
    }else{
        return -1;
    }

    return -1;
}

func test() {
    var b bytes.Buffer
    b.Reset();
    cfg := &serial.Config{Name: port, Baud: baud, ReadTimeout: 50 /*毫秒*/}
    iorwc, err := serial.OpenPort(cfg)
    if err != nil {
        fmt.Println(err)
    }else{
        defer iorwc.Close();
    }

    flg := 1;
    errflg := 1;
    retstr := ""
    for {
        if (iorwc == nil){
            break;
        }
        /* app->iap */
        errflg = 1;
        for j := 0; j <= 5; j++ {
            retstr = serialrw(iorwc, "test;")
            if ((len(retstr) > 5) && (strings.Index(retstr, "error") < 0)){
                errflg = 0;
                break;
            }
            
            fmt.Println(j, "app->iap", retstr)
        }

        retstr = serialrw(iorwc, "iap_init;")
        retstr = serialrw(iorwc, "iap_erase 0x3ff00;")
        retstr = serialrw(iorwc, "iap_write 0x3ff00 ad5005b0;")
        retstr = serialrw(iorwc, "reboot;")
        time.Sleep(time.Duration(500)*time.Millisecond);

        errflg = 1;
        for j := 0; j <= 5; j++ {
            retstr = serialrw(iorwc, "test;")
            if (strings.Index(retstr, "boot_") >= 0){
                errflg = 0;
                break;
            }
            
            fmt.Println(j, "app->iap", retstr)
        }
        if (0 != errflg){
            break;
        }

        /* 升级 */
        if (0 != upgrade(iorwc)){
            break;
        }

        /* 校验 */

        /* iap->app */
        retstr = serialrw(iorwc, "iap_run;")
        time.Sleep(time.Duration(500)*time.Millisecond);

        /* clear flg */
        errflg = 1;
        for j := 0; j <= 5; j++ {
            retstr = serialrw(iorwc, "test;")
            if (strings.Index(retstr, "app_") >= 0){
                errflg = 0;
                break;
            }
            
            fmt.Println(j, "app->iap", retstr)
        }
        if (0 != errflg){
            break;
        }
        retstr = serialrw(iorwc, "iap_init;")
        retstr = serialrw(iorwc, "iap_erase 0x3ff00;")
        retstr = serialrw(iorwc, "reboot;")

        flg = 0;
        break;
    }

    if (flg == 0){
        fmt.Println("upgrade ok");
    }else{
        fmt.Println("upgrade error");
    }
}

func main() {
    flag.StringVar(&port, "p", "COM11", "串口端口")
    flag.StringVar(&file, "f", "tracker-app.bin", "升级文件")
    flag.IntVar(&baud, "b", 115200, "串口波特率")
    flag.IntVar(&to, "t", 50, "包间延时")

    logFile,err:=os.Create("test.log")
    defer logFile.Close()
    if err !=nil{
        log.Fatalln("create file log.log failed")
    }
    logger =log.New(logFile, "", 0)
    flag.Parse()

    test();
}
