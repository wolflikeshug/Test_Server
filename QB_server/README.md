

package is 1500 byte long

build with 

checksum of the slice (crc32) + signal + received slice number + received full message's checksum(crc32)  + slice number sending + total slice number + sent full message's checksum(crc32) + data 



10 + 2 + 4 + 10 + 4 + 4 + 10 + 1456

signal 00 means received signal data

signal 01 means not received any data





cJSON Library

This project uses cJSON library for JSON parsing.
Copyright (c) 2009-2017 Dave Gamble and cJSON contributors
https://github.com/DaveGamble/cJSON

cJSON is licensed under the MIT License.
