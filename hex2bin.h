#ifndef HEX2BIN_H
#define HEX2BIN_H

#include "stdio.h"
#include "stdlib.h"

typedef unsigned char BYTE;
typedef unsigned int UINT;

typedef struct{
    int len; //bin文件大小
    UINT startAddress; //刷写的起始地址
    BYTE *pContent;     //转化后的内容
}HexToBinData;

typedef struct{
    BYTE data[16];//数据
    BYTE len;   //数据长度
    UINT pos;   //偏移地址
    BYTE type;  //类型
}HexLinData;

int ConvertHexToBin(const char *str,HexToBinData *pData);

#endif // HEX2BIN_H
