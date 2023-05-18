

package is 1500 byte long

build with 

checksum of the slice (crc32) (10) + signal (2) + checksum of received message (10) + slice number recv (4)  + slice number sending (4) + total slice number (4) + full message's checksum(crc32)  (10) + data 

将受到的字符串拆分为多个长度至多为1460的数据块，用数组来储存这些数据块，



signal 02 means pulse connection

signal 01 means not received any data

signal 00 means received signal data











unzip openssl.zip
cd openssl
./config --prefix=/usr/local/openssl_1_1_1 --openssldir=/usr/local/openssl_1_1_1 shared
make
sudo make install