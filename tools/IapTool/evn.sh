#!/bin/bash
# 获取工作路径
homePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
workPath=$PWD
echo work dir [$homePath] [$workPath]

export GO111MODULE=on
export GOPATH=$homePath/evn/
export GOROOT=$GOPATH/go

[ -d $GOROOT ] || {
    mkdir -p $GOPATH
}
[ -d $GOROOT ] || {
    echo "no go env"
    pushd $GOPATH
    [ -f go1.16.linux-amd64.tar.gz ] || wget https://golang.org/dl/go1.16.linux-amd64.tar.gz
    tar -zxvf go1.16.linux-amd64.tar.gz
    popd
}
export PATH=$GOROOT/bin:$PATH

echo "export GOPROXY=https://goproxy.cn"
