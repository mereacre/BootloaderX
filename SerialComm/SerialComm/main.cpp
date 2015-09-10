//
//  main.cpp
//  SerialComm
//
//  Created by Alexandru Mereacre on 10/09/2015.
//  Copyright (c) 2015 NquiringMinds LTD. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <assert.h>
#include <pthread.h>
#include <curses.h>
#include <time.h>
#include <termios.h>
#include <iostream>
#include <signal.h>
#include <poll.h>
#include <limits.h>
#include <iostream>
#include <list>

#include "SerialPort.h"
#include "defines.h"

int             gftHandle = -1;
struct pollfd   gWaitDescriptor;
unsigned char   gSendBuf[MAX_BUF_SIZE], gRecBuf[MAX_BUF_SIZE];

void PrintReadBuf(unsigned short offset, int nBytes)
{
    for (int idx = 0; idx<nBytes; idx++)
        printf("Data[%d]: %X\n", idx, gRecBuf[offset+idx]);
    
}

ssize_t ReadSeriaData(unsigned short offset, int nBytes)
{
    ssize_t bytesRead = 0;
    
    int pollRc = poll(&gWaitDescriptor, 1, -1);
    if (pollRc < 0) {
        std::cout << "Error: setting the poll" << std::endl;
    } else if(pollRc > 0) {
        
        if( gWaitDescriptor.revents & POLLIN ) {
            bytesRead = read(gftHandle, &gRecBuf[offset], nBytes);
            //std::cout << "Bytes read: " << bytesRead << std::endl;
        }
        else if(gWaitDescriptor.revents & POLLHUP || gWaitDescriptor.revents & POLLERR) {
            std::cout << "Error: poll read gftHandle" << std::endl;
        }
    }
    
    return bytesRead;
}
int SetFlashAddress(unsigned short address)
{
    ssize_t nRecBytes = 0;
    
    gSendBuf[0] = 0x41;  // Character "A"
    gSendBuf[1] = (unsigned char) ((address>>8) & 0xFF);
    gSendBuf[2] = (unsigned char) (address & 0xFF);
    
    WriteToPort(&gftHandle, 3, &gSendBuf[0]);
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    
    if (nRecBytes!=1 || gRecBuf[0]!=0x0D) {
        printf("Couldn't set the FLASH address\n");
        return 0;
    }
    
    return 1;
}

ssize_t ReadFlashPage(unsigned short address, unsigned short byteCount)
{
    ssize_t nRecBytes = 0;
    unsigned short byteCountRead = 0;
    
    if (!SetFlashAddress(address))
        return 0;
    
    gSendBuf[0] = 0x67;  // Character "g"
    gSendBuf[1] = (unsigned char) ((byteCount>>8) & 0xFF);
    gSendBuf[2] = (unsigned char) (byteCount & 0xFF);
    
    gSendBuf[3] = 0x46;  // Character "F"
    //gSendBuf[3] = 0x45;  // Character "E"
    
    WriteToPort(&gftHandle, 4, &gSendBuf[0]);
    while(byteCount>0) {
        nRecBytes = ReadSeriaData(byteCountRead, MAX_BUF_SIZE);
        byteCount -= nRecBytes;
        byteCountRead += nRecBytes;
        if (!nRecBytes)
            break;
    }
    
    printf("Number of bytes read %d\n", byteCountRead);
    PrintReadBuf(0, byteCountRead);
    
    return nRecBytes;
}

int WriteFlashPage(unsigned short address, unsigned short byteCount, unsigned char *dataBuf)
{
    ssize_t nRecBytes = 0;
    
    if (!SetFlashAddress(address))
        return 0;
    
    gSendBuf[0] = 0x42;  // Character "B"
    gSendBuf[1] = (unsigned char) ((byteCount>>8) & 0xFF);
    gSendBuf[2] = (unsigned char) (byteCount & 0xFF);
    
    gSendBuf[3] = 0x46;  // Character "F"
    //gSendBuf[3] = 0x45;  // Character "E"
    memcpy(&gSendBuf[4], dataBuf, byteCount);
    WriteToPort(&gftHandle, 4+byteCount, &gSendBuf[0]);
    
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    
    if (nRecBytes!=1 || gRecBuf[0]!=0x0D) {
        printf("Couldn't write block to FLASH\n");
        return 0;
    }
    
    return 1;
}
unsigned short GetDeviceBlockSize(void)
{
    unsigned short blockSize = 0x0;
    ssize_t nRecBytes = 0;
    
    gSendBuf[0] = 0x62;  // Character "b"
    WriteToPort(&gftHandle, 1, &gSendBuf[0]);
    
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    if(nRecBytes!=3 && gRecBuf[0]!='Y') {
        printf("Couldn't get the block size\n");
        return 0;
    }
        
    blockSize = (unsigned short)(gRecBuf[1]<<8);
    blockSize = blockSize | (unsigned short) gRecBuf[2];

    return blockSize;
}

int EraseFlash(void)
{
    ssize_t nRecBytes = 0;
    
    gSendBuf[0] = 0x65;  // Character "e"
    WriteToPort(&gftHandle, 1, &gSendBuf[0]);
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    
    if (nRecBytes!=1 || gRecBuf[0]!=0x0D) {
        printf("Couldn't erase FLASH\n");
        return 0;
    }

    return 1;
}

int main(int argc, const char * argv[]) {
    struct  termios oldTio;
    auto portPathName = DEFAULT_PORT_PATH_NAME;
    ssize_t nRecBytes = 0;
    
    if (argc<2) {
        std::cout << "Not enough input arguments" << std::endl;
        std::cout << "Use SerialComm portpath" << std::endl;
        return 1;
    } else
        portPathName = argv[1];
    
    if((gftHandle = open(portPathName, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY))==-1) {
        std::cout << "Couldn't open port: "<< portPathName << std::endl;
        return 1;
    } else {
        std::cout << "Port: " << portPathName << " opened " << std::endl;
        
        SetDefaultPortSettings(&gftHandle, &oldTio);
        
        gWaitDescriptor.fd = gftHandle;
        gWaitDescriptor.events = POLLIN;

        gSendBuf[0] = 0x53;  // Character "S"
        WriteToPort(&gftHandle, 1, &gSendBuf[0]);
        
        nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
        gRecBuf[nRecBytes] = '\0';
        std::cout << "Programmer: "<< gRecBuf <<std::endl;
        
        gSendBuf[0] = 0x73;  // Character "s"
        WriteToPort(&gftHandle, 1, &gSendBuf[0]);
        nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
        
        if (nRecBytes<3) {
            std::cout << "Wrong device signature" << std::endl;
            ClosePortDevice(&gftHandle, &oldTio);
            return 1;
        } else printf("Device signature: %X %X %X\n", gRecBuf[0], gRecBuf[1], gRecBuf[2]);
        
        unsigned short blkSize = GetDeviceBlockSize();
        printf("Block size: %d\n", blkSize);
        
        unsigned char dataBuf[PAGE_SIZE_BYTES];
        memset(&dataBuf[0], 0x0C, PAGE_SIZE_BYTES);
        //EraseFlash();
        if (WriteFlashPage(0x00, PAGE_SIZE_BYTES, &dataBuf[0]))
            nRecBytes = ReadFlashPage(0x000, PAGE_SIZE_BYTES);
    }
    
    ClosePortDevice(&gftHandle, &oldTio);
    return 0;
}

