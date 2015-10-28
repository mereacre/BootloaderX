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
#include "HexFile.h"

int             gftHandle = -1;
struct pollfd   gWaitDescriptor;
unsigned char   gSendBuf[MAX_BUF_SIZE], gRecBuf[MAX_BUF_SIZE];

std::vector<HEX> hexFileData;

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
int SetFlashAddress(unsigned long address)
{
    ssize_t nRecBytes = 0;
    
    if(address>=LIMIT_BASE_ADDRESS) {
        printf("Address %lX exceeds the limit\n", address);
        return 0;
    }
    gSendBuf[0] = 0x48;  // Character "H"
    gSendBuf[1] = (unsigned char) ((address>>16) & 0xFF);
    gSendBuf[2] = (unsigned char) (address>>8 & 0xFF);
    gSendBuf[3] = (unsigned char) (address & 0xFF);
    
    WriteToPort(&gftHandle, 4, &gSendBuf[0]);
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    
    if (nRecBytes!=1 || gRecBuf[0]!=0x0D) {
        printf("Couldn't set the FLASH address\n");
        return 0;
    }
    
    return 1;
}

ssize_t ReadFlashPage(unsigned long address, unsigned short byteCount)
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
    //PrintReadBuf(0, byteCountRead);
    
    return byteCountRead;
}

int WriteFlashPage(unsigned long address, unsigned short byteCount, unsigned char *dataBuf)
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

int ExecuteApp(void)
{
    ssize_t nRecBytes = 0;
    
    gSendBuf[0] = 0x45;  // Character "E"
    WriteToPort(&gftHandle, 1, &gSendBuf[0]);
    nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
    
    if (nRecBytes!=1 || gRecBuf[0]!=0x0D) {
        printf("Couldn't execute APP\n");
        return 0;
    }
    
    return 1;
}

int main(int argc, const char * argv[]) {
    struct  termios oldTio;
    auto portPathName = DEFAULT_PORT_PATH_NAME;
    char *hexFilePath;
    char *cmdStr;
    ssize_t nRecBytes = 0;
    unsigned char cmdType = 0;
    
    if (argc<4) {
        std::cout << "Not enough input arguments" << std::endl;
        std::cout << "Use SerialComm [cmd] portpath hexfilepath" << std::endl;
        std::cout << "[cmd]:" << std::endl;
        std::cout << "-p programm" << std::endl;
        std::cout << "-e erase" << std::endl;
        std::cout << "-E execute App" << std::endl;
        std::cout << "Example: SerialComm -p /dev/tty.usbserial-FTWOHPEA HexFile.hex" << std::endl;
        return 1;
    } else {
        cmdStr = (char*)argv[1];
        if (!strcmp(cmdStr, "-p"))
            cmdType = 1;
        else if (!strcmp(cmdStr, "-e"))
            cmdType = 2;
        else if (!strcmp(cmdStr, "-E"))
            cmdType = 3;
        portPathName = argv[2];
        hexFilePath = (char*)argv[3];
    }
    
    if((gftHandle = open(portPathName, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY))==-1) {
        std::cout << "Couldn't open port: "<< portPathName << std::endl;
        return 1;
    } else {
        std::cout << "[Port]: " << portPathName << " opened " << std::endl;
        
        SetDefaultPortSettings(&gftHandle, &oldTio);
        
        gWaitDescriptor.fd = gftHandle;
        gWaitDescriptor.events = POLLIN;

        gSendBuf[0] = 0x53;  // Character "S"
        WriteToPort(&gftHandle, 1, &gSendBuf[0]);
        
        nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
        gRecBuf[nRecBytes] = '\0';
        std::cout << "[Programmer]: "<< gRecBuf <<std::endl;
        
        gSendBuf[0] = 0x73;  // Character "s"
        WriteToPort(&gftHandle, 1, &gSendBuf[0]);
        nRecBytes = ReadSeriaData(0, MAX_BUF_SIZE);
        
        if (nRecBytes<3) {
            std::cout << "Wrong device signature" << std::endl;
            ClosePortDevice(&gftHandle, &oldTio);
            return 1;
        } else printf("[Device signature]: %X %X %X\n", gRecBuf[0], gRecBuf[1], gRecBuf[2]);
        
        unsigned short blkSize = GetDeviceBlockSize();
        printf("[Block size]: %d\n", blkSize);
        
        //unsigned char loadF = 0x01;
        
        if (LoadHexFile(hexFilePath, &hexFileData) && cmdType==1) {
            printf("[Hex File]: %s opened\n", hexFilePath);
            
            unsigned char dataBuf[PAGE_SIZE_BYTES];
            unsigned char *memBuf = NULL, *prevMemBuf = NULL;
            unsigned long baseAddress = 0x00, memSize = 0x00, extendedAddress = 0x00, writeAddress = 0x00;
            unsigned long prevAddr = 0x00;
            
            memset(&dataBuf[0], 0xFF, PAGE_SIZE_BYTES);
            
            for (int idx=0; idx<hexFileData.size(); idx++) {
                HEX hdata =hexFileData[idx];
                
                writeAddress = extendedAddress + hdata.address;
                
                if (hdata.type==0x02) {
                    
                    printf("Detected extended address %lX\n", hdata.extendedaddress);
                    extendedAddress = hdata.extendedaddress;
                    
                } else if (!hdata.type) {
                    if (writeAddress && writeAddress<prevAddr) {
                        printf("Addresses are not in sequence\n");
                        printf("Address %lX\n", writeAddress);
                        break;
                    } else if (writeAddress>prevAddr) {
                        printf("Gap in the address %lX\n", writeAddress);
                        break;
                    } else prevAddr = writeAddress+hdata.len;
                    
                    memBuf = (unsigned char*)realloc(prevMemBuf, writeAddress+hdata.len);
                    
                    if(memBuf!=NULL) {
                        prevMemBuf = memBuf;
                        memcpy(memBuf+writeAddress, hdata.data.data(), hdata.len);
                        
                        memSize = writeAddress+hdata.len;
                    } else {
                        printf("Couldn't allocate memory for hex data\n");
                        if(prevMemBuf!=NULL) free(prevMemBuf);
                        break;
                    }
                } else if (hdata.type==1) {
                    if(memBuf==NULL) {
                        printf("Reached the end of hex file\n");
                        break;
                    } else {
                        int pageIdx = 0;
                        printf("[Generating]: memory size %lu, number of pages %lu and remainder %lu\n", memSize, memSize/PAGE_SIZE_BYTES, memSize %PAGE_SIZE_BYTES);
                        printf("[At base address]: %lX\n", baseAddress);
                        for (pageIdx=0; pageIdx<memSize/PAGE_SIZE_BYTES; pageIdx++) {
                            printf("Writing page %d at address %lX...\n", pageIdx+1, baseAddress);
                            if (WriteFlashPage(baseAddress>>1, PAGE_SIZE_BYTES, memBuf+pageIdx*PAGE_SIZE_BYTES)) {
                                nRecBytes = ReadFlashPage(baseAddress>>1, PAGE_SIZE_BYTES);
                                if (!memcmp(memBuf+pageIdx*PAGE_SIZE_BYTES, gRecBuf, PAGE_SIZE_BYTES) && nRecBytes==PAGE_SIZE_BYTES)
                                    printf("Page written ok\n");
                                else {
                                    printf("Bytes read not equal to bytes written!\n");
                                    break;
                                }
                            } else {
                                printf("Page write fault\n");
                                break;
                            }
                            baseAddress += PAGE_SIZE_BYTES;
                        }
                        
                        printf("Writing page %d at address %lX...\n", pageIdx+1, baseAddress);
                        if (WriteFlashPage(baseAddress>>1, memSize % PAGE_SIZE_BYTES, memBuf+pageIdx*PAGE_SIZE_BYTES)) {
                            nRecBytes = ReadFlashPage(baseAddress>>1, memSize % PAGE_SIZE_BYTES);
                            if (!memcmp(memBuf+pageIdx*PAGE_SIZE_BYTES, gRecBuf, memSize % PAGE_SIZE_BYTES) && nRecBytes==memSize % PAGE_SIZE_BYTES)
                                printf("Page written ok\n");
                            else {
                                printf("Bytes read not equal to bytes written!\n");
                                break;
                            }
                        } else {
                            printf("Page write fault\n");
                            break;
                        }
                    }
                }
            }
            //EraseFlash();
            //if (WriteFlashPage(0x00, PAGE_SIZE_BYTES, &dataBuf[0]))
            //nRecBytes = ReadFlashPage(0x000, PAGE_SIZE_BYTES);
            if(prevMemBuf!=NULL) free(prevMemBuf);
            
            //if(memBuf!=NULL) free(memBuf);

        } else if (cmdType==2){
            printf("Erasing Device application FLASH section...\n");
            EraseFlash();
            printf("Done\n");
        } else if(cmdType==3){
            printf("Starting execution of the FLASH APP...\n");
            ExecuteApp();
            printf("Done\n");
        }
    }

    ClosePortDevice(&gftHandle, &oldTio);
    return 0;
}

