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
#include <unistd.h>
#include <gmp.h>
#include <stdio.h>
#include <string.h>
#include "/usr/local/include/pbc/pbc.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>


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
#define LOC_SERVER_ADDRESS "X.X.X.X"
#define FOG_SERVER_ADDRESS_1 "X.X.X.X"
#define FOG_PORT_1 XX
#define CLOUD_PORT XX
#define LOC_PORT XX
#define MSGSIZE XX
#define QUEUE_SIZE XX
int main(int argc, const char * argv[]) {
    
    
    
    // Read  N
    int Fog_Num = X;
    string USER_ID = "X";
    int Sensor_Num = X;
    int Fog_Sensor_Num = Sensor_Num/Fog_Num;
    char PK_char[MSGSIZE],USER_ID_char[MSGSIZE];
    
    
    
    string Owner_PK_path = "XXX";
    Owner_PK_path += USER_ID;
    Owner_PK_path +="XXXX";
    fstream read_ptr;
    read_ptr.open(Owner_PK_path);
    string PK_str;
    getline(read_ptr,PK_str);
    for(int i=0;i<PK_str.size();i++){
        PK_char[i] = PK_str[i];
    }
    PK_char[PK_str.size()] = '!';
    for(int i=0;i<USER_ID.size();i++){
        USER_ID_char[i] = USER_ID[i];
    }
    USER_ID_char[USER_ID.size()] = '!';
    read_ptr.close();
    
    /*——————————————————BEGIN——————————————————*/
    /*—————————————————读取公钥——————————————————*/
    
    
    /*——————————————————BEGIN————————————————————*/
    /*———————————————向云公开公钥——————————————————*/
    int sClient;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(CLOUD_PORT);
    addrSer.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    
    socklen_t addrlen = sizeof(struct sockaddr);
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,USER_ID_char, MSGSIZE, 0);
    send(sClient,PK_char, MSGSIZE, 0);
    
    cout<<"向云公开公钥完成"<<endl;
    close(sClient);
    /*———————————————————END————————————————————*/
    /*———————————————向云公开公钥——————————————————*/
    
    
    
    /*———————————————————————————BEGIN——————————————————————————*/
    /*———————————————向雾发送注册标签;接收雾节点公钥匙—————————————————*/
    string Sensor_RT_path = "XXXX";
    Sensor_RT_path += USER_ID;
    Sensor_RT_path +="XXXX";
    string Fog_PK_path = "XXXX";
    Fog_PK_path += USER_ID;
    Fog_PK_path +="XXXX";
    fstream write_ptr;
    write_ptr.open(Fog_PK_path,ios::out);
    
    
    string Fog_Sensor_Num_str =to_string(Fog_Sensor_Num);
    char Fog_Sensor_Num_char[MSGSIZE];
    for(int i=0;i<Fog_Sensor_Num_str.size();i++){
        Fog_Sensor_Num_char[i] = Fog_Sensor_Num_str[i];
    }
    Fog_Sensor_Num_char[Fog_Sensor_Num_str.size()] = '!';
    
    read_ptr.open(Sensor_RT_path);
    string RT_str;
    char RT_char[MSGSIZE];
    char Fog_PK_char[MSGSIZE];
    string Fog_PK_str;
    
    for(int i =0;i<Fog_Num;i++){
        int Port =FOG_PORT_1+i;
        struct sockaddr_in addrSer_Fog, addrCli1;
        addrSer_Fog.sin_family =AF_INET;
        addrSer_Fog.sin_port =  htons(Port);
        addrSer_Fog.sin_addr.s_addr = inet_addr(FOG_SERVER_ADDRESS_1);
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        
        bind(sClient,(struct sockaddr*)&addrCli1, sizeof(struct sockaddr));
        socklen_t addrlen1 = sizeof(struct sockaddr);
        while( connect(sClient,(struct sockaddr*)&addrSer_Fog, addrlen1)==-1){
        }
        send(sClient,USER_ID_char, MSGSIZE, 0);
        send(sClient,Fog_Sensor_Num_char, MSGSIZE, 0);
        send(sClient,PK_char, MSGSIZE, 0);
        
        for(int j=0;j<Fog_Sensor_Num;j++){
            getline(read_ptr,RT_str);
            //cout<<RT_str<<endl;
            for(int k=0;k<RT_str.size();k++){
                RT_char[k] = RT_str[k];
            }
            RT_char[RT_str.size()]='!';
            send(sClient,RT_char, MSGSIZE, 0);
            RT_str.clear();
        }
        
        recv(sClient,Fog_PK_char, MSGSIZE, 0);
        for(int j=0;j<MSGSIZE;j++){
            if(Fog_PK_char[j]=='!'){
                break;
            }
            Fog_PK_str += Fog_PK_char[j];
        }
        write_ptr<<Fog_PK_str<<endl;
        Fog_PK_str.clear();
        
        close(sClient);
        
        
    }
    read_ptr.close();
    write_ptr.close();
    cout<<"向雾节点发送注册标签完成"<<endl;
    /*—————————————————————————————END———————————————————————————*/
    /*———————————————向雾发送注册标签；接收雾节点的公钥—————————————————*/
    

    
    /*———————————————————————BEGIN————————————————————————*/
    /*————————————向数据收集设备发送密钥,云，雾公钥——————————————*/
    string Cloud_PP_path = "XXXXX";
    fstream Read_ptr2;
    Read_ptr2.open(Cloud_PP_path);
    string N_str,g_str,Cloud_PK_str;
    char N_char[MSGSIZE],g_char[MSGSIZE],Cloud_PK_char[MSGSIZE];
    getline(Read_ptr2,N_str);
    getline(Read_ptr2,g_str);
    getline(Read_ptr2,Cloud_PK_str);
    Read_ptr2.close();
    for(int i=0;i<MSGSIZE;i++){
        N_char[i] = N_str[i];
    }
    N_char[N_str.size()] = '!';
    for(int i=0;i<MSGSIZE;i++){
        g_char[i] = g_str[i];
    }
    g_char[g_str.size()] = '!';
    for(int i=0;i<MSGSIZE;i++){
        Cloud_PK_char[i] = Cloud_PK_str[i];
    }
    Cloud_PK_char[Cloud_PK_str.size()] = '!';
    
    
    
    string Sensor_SK_path = "XXXX";
    Sensor_SK_path += USER_ID;
    Sensor_SK_path += "XXXX";
    int FogID = 1;
    int SensorID = 1;
    string FogID_str = to_string(FogID);
    int sockSer = socket(AF_INET,SOCK_STREAM,0);
    if(sockSer==-1){
        perror("socket");
    }
    int yes=1;
    if(setsockopt(sockSer, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int))==-1){
        perror("setsockopt");
    }
    struct sockaddr_in addrSer_2, addrCli_2;
    addrSer_2.sin_family = AF_INET;
    addrSer_2.sin_port = htons(LOC_PORT);
    addrSer_2.sin_addr.s_addr = inet_addr(LOC_SERVER_ADDRESS);
    socklen_t addrlen_2 = sizeof(struct sockaddr);
    int res = ::bind(sockSer,(struct sockaddr*)&addrSer_2,addrlen_2);
    if(res==-1){
        perror("blind");
    }
    listen(sockSer,QUEUE_SIZE);
    
    char Sensor_SK_char[MSGSIZE];
    char FogID_char[MSGSIZE];
    char SensorID_char[MSGSIZE];

    fstream read_ptr1;
    read_ptr1.open(Fog_PK_path);
    read_ptr.open(Sensor_SK_path);
    string Sensor_SK_str;
    
    getline(read_ptr1,Fog_PK_str);
    for(int j=0;j<Fog_PK_str.size();j++){
        Fog_PK_char[j] = Fog_PK_str[j];
    }
    Fog_PK_char[Fog_PK_str.size()] = '!';
    Fog_PK_str.clear();
    
    for(int i=1;i<=Sensor_Num;i++){
        
        int sockConn = accept(sockSer, (struct sockaddr*)&addrCli_2, &addrlen_2);
        if(sockConn == -1){
            cout<<"accept失败"<<endl;
            return 0;
        }
        /*数据准备*/
        string SensorID_str = to_string(SensorID);
        for(int j=0;j<SensorID_str.size();j++){
            SensorID_char[j] = SensorID_str[j];
        }
        SensorID_char[SensorID_str.size()] = '!';
        for(int j=0;j<FogID_str.size();j++){
            FogID_char[j] = FogID_str[j];
        }
        FogID_char[FogID_str.size()] ='!';
        getline(read_ptr,Sensor_SK_str);
        for(int j=0;j<Sensor_SK_str.size();j++){
            Sensor_SK_char[j] = Sensor_SK_str[j];
        }
        Sensor_SK_char[Sensor_SK_str.size()] = '!';
        
        /*数据准备*/
        send(sockConn,FogID_char,MSGSIZE,0);
        send(sockConn,SensorID_char,MSGSIZE,0);
        send(sockConn,Sensor_SK_char,MSGSIZE,0);
        send(sockConn,Fog_PK_char,MSGSIZE,0);
        send(sockConn,N_char,MSGSIZE,0);
        send(sockConn,g_char,MSGSIZE,0);
        send(sockConn,Cloud_PK_char,MSGSIZE,0);
        send(sockConn,PK_char, MSGSIZE, 0);
        SensorID++;
        if(i%Fog_Sensor_Num==0){
            Fog_PK_str.clear();
            getline(read_ptr1,Fog_PK_str);
            for(int j=0;j<Fog_PK_str.size();j++){
                Fog_PK_char[j] = Fog_PK_str[j];
            }
            Fog_PK_char[Fog_PK_str.size()] = '!';
            FogID++;
            FogID_str.clear();
            FogID_str = to_string(FogID);
            SensorID=1;
        }
    }
    read_ptr1.close();
    read_ptr.close();
    cout<<"向数据收集设备发送私钥完成"<<endl;
    
    
    
    /*————————————————————————END——————————————————————————*/
    /*————————————向数据收集设备发送密钥,云，雾公钥——————————————*/
    
    return 0;
}
