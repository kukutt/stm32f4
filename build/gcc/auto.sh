#!/bin/bash
. ./evn.sh

make cleanall
make 
make out=boot
make out=noiap
