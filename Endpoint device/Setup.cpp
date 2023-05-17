//
//  main.cpp
//  Client
//
//  Created by 宋明洋 on 2022/1/25.
//
//#ifdef _MSC_VER
//#ifdef _WIN64
//#include <WinSock2.h>
//#elif _WIN32
#include <sys/socket.h>
//#endif
//#endif

#include <fstream>
#include <iostream>
#include <gmpxx.h>
#include<unistd.h>
#include <gmp.h>
#include <stdio.h>
#include <string.h>
#include "/usr/local/include/pbc/pbc.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <ctime>
#include<netinet/in.h>
#include<arpa/inet.h>


#pragma comment(lib,"ws2_32.lib")
#include <assert.h>

using namespace std;

string binaryToHex(const string& binaryStr){
    string ret;
    static const char *hex = "0123456789ABCDEF";
    for(auto c:binaryStr){
        ret.push_back(hex[(c>>4)&0xf]);
        ret.push_back(hex[c&0xf]);
    }
    return ret;
}

unsigned char* str2hex(char *str) {
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (unsigned char *)malloc(str_len / 2);
    for (i = 0; i <str_len; i = i + 2) {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}

string HexToStr(std::string str){
    std::string hex = str;
    long len = hex.length();
    std::string newString;
    for (long i = 0; i<len; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}

string unsignedCharToHexString(unsigned char ch) {
    const char hex_chars[] = "0123456789abcdef";
    unsigned int highHalfByte = (ch>> 4) & 0x0f;
    unsigned int lowHalfByte = (ch &0x0f);
    string result;
    result += hex_chars[highHalfByte];
    result += hex_chars[lowHalfByte];
    return result;
}


#define CLOUD_SERVER_ADDRESS "X.X.X.X"
#define OWNER_SERVER_ADDRESS "X.X.X.X"
#define FOG_SERVER_ADDRESS_1 "X.X.X.X"
#define FOG_PORT_1 XX
#define CLOUD_PORT XX
#define OWNER_PORT XX
#define MSGSIZE XXX
#define QUEUE_SIZE XX
int main(int argc, const char * argv[]) {
    
    string Device = "X";
    string Owner = "X";
    string FogID_str, SensorID_str,SK_str,FogPK_str,N_str,g_str,Cloud_PK_str,Owner_PK_str;
    char FogID_char[MSGSIZE],SensorID_char[MSGSIZE],SK_char[MSGSIZE],FogPK_char[MSGSIZE],N_char[MSGSIZE],g_char[MSGSIZE],Cloud_PK_char[MSGSIZE],Owner_PK_char[MSGSIZE];
    
    
    int sClient;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(OWNER_PORT);
    addrSer.sin_addr.s_addr = inet_addr(OWNER_SERVER_ADDRESS);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    
    socklen_t addrlen = sizeof(struct sockaddr);
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    cout<<"连接成功"<<endl;
    recv(sClient,FogID_char, MSGSIZE, 0);
    recv(sClient,SensorID_char, MSGSIZE, 0);
    recv(sClient,SK_char, MSGSIZE, 0);
    recv(sClient,FogPK_char, MSGSIZE, 0);
    recv(sClient,N_char, MSGSIZE, 0);
    recv(sClient,g_char, MSGSIZE, 0);
    recv(sClient,Cloud_PK_char, MSGSIZE, 0);
    recv(sClient,Owner_PK_char, MSGSIZE, 0);
    
    for(int i=0;i<MSGSIZE;i++){
        if(FogID_char[i]=='!'){
            break;
        }
        FogID_str+=FogID_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(SensorID_char[i]=='!'){
            break;
        }
        SensorID_str+=SensorID_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(SK_char[i]=='!'){
            break;
        }
        SK_str+=SK_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(FogPK_char[i]=='!'){
            break;
        }
        FogPK_str+=FogPK_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(N_char[i]=='!'){
            break;
        }
        N_str+=N_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(g_char[i]=='!'){
            break;
        }
        g_str+=g_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(Cloud_PK_char[i]=='!'){
            break;
        }
        Cloud_PK_str+=Cloud_PK_char[i];
    }
    for(int i=0;i<MSGSIZE;i++){
        if(Owner_PK_char[i]=='!'){
            break;
        }
        Owner_PK_str+=Owner_PK_char[i];
    }
    string SK_path_str = "";
    
    
    
    fstream write_ptr;
    write_ptr.open(SK_path_str,ios::out);
    write_ptr<<FogID_str<<endl;
    write_ptr<<SensorID_str<<endl;
    write_ptr<<SK_str<<endl;
    write_ptr<<FogPK_str<<endl;
    write_ptr<<N_str<<endl;
    write_ptr<<g_str<<endl;
    write_ptr<<Cloud_PK_str<<endl;
    write_ptr<<Owner_PK_str<<endl;
    write_ptr.close();
    
    return 0;
}
