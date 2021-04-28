#!/bin/bash
# 获取工作路径
homePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
workPath=$PWD
echo work dir [$homePath] [$workPath]

[ -d $homePath/evn ] || {
    pushd $homePath
    [ -f $homePath/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2 ] || {
        wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    }
    tar -jxvf gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    mv gcc-arm-none-eabi-5_4-2016q3 evn
    rm -rf gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    popd
}
export PATH=$homePath/gcc-arm-none-eabi-5_4-2016q3/bin/:$PATH

echo $PATH

