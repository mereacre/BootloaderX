//
//  HexFile.cpp
//  SerialComm
//
//  Created by Alexandru on 11/09/2015.
//  Copyright (c) 2015 NquiringMinds LTD. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "HexFile.h"

int LoadHexFile(char *filepath, std::vector<HEX> *hexFileData)
{
    FILE *fp = fopen(filepath, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if(fp==NULL) {
        printf("Couldn't open file: %s\n", filepath);
        return 0;
    }
    
    while ((read = getline(&line, &len, fp)) != -1) {
        HEX hexData;
        char digitAddr[4];
        hexData.extendedaddress = 0x0;
        
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        if(line[0]!=':' && line[0]!=0x0D) {
            printf("Corrupt file, please correct the first character in line %s\n", line);
            fclose(fp);
            return 0;
        } else if(line[0]==':') {
            
            sscanf(line+1, "%2hhx", &hexData.len);
            
            memcpy(digitAddr, line+3, 4);
            hexData.address = (unsigned short)strtol(digitAddr, NULL, 16);
            
            sscanf(line+7, "%2hhx", &hexData.type);
            
            if (hexData.type>2) {
                printf("Data type %d in %s not supported\n", hexData.type, line);
                fclose(fp);
                return 0;
            } else if(!hexData.type) {
                unsigned char databyte;
                for (int idx=0;idx<hexData.len; idx++) {
                    if (9+idx*2>read) {
                        printf("Wrong amount of data on a line %s\n", line);
                        fclose(fp);
                        return 0;
                    }
                    sscanf(line+9+idx*2, "%2hhx", &databyte);
                    hexData.data.push_back(databyte);
                }
            } else if(hexData.type==2) {
                memcpy(digitAddr, line+9, 4);
                hexData.extendedaddress = ((unsigned int)strtol(digitAddr, NULL, 16))<<4;
            }
            
            if (hexData.type==1)
                sscanf(line+9, "%2hhx", &hexData.crc);
            else
                sscanf(line+9+hexData.len*2, "%2hhx", &hexData.crc);
            
            //printf("Data len: %X\n", hexData.len);
            //printf("Data address: %X\n", hexData.address);
            //printf("Data type: %X\n", hexData.type);
            //printf("Extended address: %X\n", hexData.extendedaddress);
            //for(int idx=0; idx<hexData.data.size();idx++)
            //    printf("%X ", hexData.data[idx]);
            //printf("\n");
            //printf("Data CRC: %X\n", hexData.crc);
            hexFileData->push_back(hexData);
        }
        
    }
    fclose(fp);
    return 1;
}