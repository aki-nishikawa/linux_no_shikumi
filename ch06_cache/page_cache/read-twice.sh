#! /bin/bash

echo "$(date) : start file creation"
dd if=/dev/zero of=testfile oflag=direct bs=1M count=1K
echo "$(date) : finished file creation"
echo "free"
free
echo "sleep 3 second"
sleep 3

echo "$(date) : start 1st read"
time cat testfile > /dev/null
echo "$(date) : finished 1st read"
echo "free"
free
echo "sleep 3 second"
sleep 3

echo "$(date) : start 2nd read"
time cat testfile > /dev/null
echo "$(date) : finished 2nd read"
echo "free"
free
echo "sleep 3 second"
sleep 3

echo "delete file"
rm testfile
echo "program finished"