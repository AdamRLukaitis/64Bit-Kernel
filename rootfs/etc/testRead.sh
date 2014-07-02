#!/bin/sh
cd bin
echo Reading default file
testRead
echo    
echo Reading input file 
echo
testRead etc/testUlimit.sh
exit