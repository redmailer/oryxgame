#!/bin/bash
# 
for file in ./pb/*
do
	if test -f $file
	then 
		echo rm -f $file
		rm -f $file
	fi
done

for file in ./pbtxt/*.proto
do
	if test -f $file
        then
                echo ./protocolbuf-linux/bin/protoc --cpp_out=./pb/ --proto_path=./pbtxt $file
		./protocolbuf-linux/bin/protoc --cpp_out=./pb/ --proto_path=./pbtxt $file
        fi

done
