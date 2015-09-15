//
//  HexFile.h
//  SerialComm
//
//  Created by Alexandru on 11/09/2015.
//  Copyright (c) 2015 NquiringMinds LTD. All rights reserved.
//

#ifndef SerialComm_HexFile_h
#define SerialComm_HexFile_h

#include <vector>

typedef struct HEX {
    unsigned char len;
    unsigned short address;
    unsigned char type;
    unsigned int extendedaddress;
    std::vector<unsigned char> data;
    unsigned char crc;
}HEX;

int LoadHexFile(char *filepath, std::vector<HEX> *hexFileData);
#endif
