#!/bin/sh
/bin/echo Setting the ulimit as 1 
ulimit -s 1
/bin/echo call a func with a recursive call it itself in user code 
/bin/testStack
exit