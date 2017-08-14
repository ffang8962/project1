#!/bin/sh

make

a=0

while [ $a -lt 1 ]
do
#   ./orderprocessor input | tee result
   ./orderprocessor input
   a=`expr $a + 1`
done
