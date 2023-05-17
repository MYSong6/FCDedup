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
#include <cmath>
#include <unistd.h>

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



#define CLOUD_SERVER_ADDRESS "192.168.1.8"
#define FOG_SERVER_ADDRESS_1 "192.168.1.3"
#define FOG_SERVER_ADDRESS_2 "192.168.1.3"
#define FOG_SERVER_ADDRESS_3 "192.168.1.3"
#define CLOUD_PORT 18765
#define MSGSIZE 2048
int main(int argc, const char * argv[]) {
    
    
    
    /*—————————————————BEGIN————————————————————*/
    /*———————————————参数初始化——————————————————*/
    string OwnerID_str = "Owner1";
    string DeviceID_str = "1";
    string FogID_str = "fog_1";
    string FileOrd_str = "1";
    
    const int BlockSize = 1024*32;
    
    string N_str, g_str,CloudPK_str,SK_str;
    string SK_path_str = "/Users/songmingyang/Desktop/paper4/Data/"+OwnerID_str+"/cloud_parameter.txt";
    fstream Read_ptr;
    Read_ptr.open(SK_path_str);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    getline(Read_ptr,CloudPK_str);
    Read_ptr.close();
    
    string Owner_SK_path = "/Users/songmingyang/Desktop/paper4/Data/";
    Owner_SK_path += OwnerID_str;
    Owner_SK_path += "/Owner_SK.txt";
    Read_ptr.open(Owner_SK_path);
    getline(Read_ptr,SK_str);
    
    mpz_t N;
    mpz_init(N);
    const char* ptr_N = N_str.data();
    const char* SK_ptr = SK_str.data();
    const char* CloudPK_ptr = CloudPK_str.data();
    const char* g_ptr = g_str.data();
    mpz_init_set_str(N,ptr_N,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    
    
    element_t g,CloudPK,SK;
    element_init_G1(g,pairing);
    element_init_G1(CloudPK,pairing);
    element_init_Zr(SK,pairing);
    
    element_set_str(g,g_ptr,10);
    element_set_str(CloudPK,CloudPK_ptr,10);
    element_set_str(SK,SK_ptr,10);
    
    
    /*—————————————————END——————————————————————*/
    /*———————————————参数初始化——————————————————*/
    
    int sClient;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(CLOUD_PORT );
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
    cout<<"连接云服务器成功"<<endl;
    string FILENAME =OwnerID_str+"_"+FogID_str+"_"+DeviceID_str+"_"+FileOrd_str+".txt";
    char FILENAME_char[MSGSIZE];
    for(int i=0;i< FILENAME.size();i++){
        FILENAME_char[i] =FILENAME[i];
    }
    FILENAME_char[FILENAME.size()]='!';
    send(sClient,FILENAME_char, MSGSIZE, 0);
    close(sClient);
    //cout<<"1111"<<endl;
    while(1){
        char vmdL_char[MSGSIZE], vmdR_char[MSGSIZE], SKFogPart1_char[MSGSIZE],SKFogPart2_char[MSGSIZE];
        string vmdL_str,vmdR_str,SKFogPart1_str,SKFogPart2_str;
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,vmdL_char, MSGSIZE, 0);
        close(sClient);
        if(vmdL_char[0]=='E'){
            break;
        }
        
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,vmdR_char, MSGSIZE, 0);
        close(sClient);
        
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,SKFogPart1_char, MSGSIZE, 0);
        close(sClient);
        
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,SKFogPart2_char, MSGSIZE, 0);
        close(sClient);
        for(int i=0;i<MSGSIZE;i++){
            if(vmdL_char[i] =='!'){
                break;
            }
            vmdL_str+=vmdL_char[i];
        }
        
        for(int i=0;i<MSGSIZE;i++){
            if(vmdR_char[i] =='!'){
                break;
            }
            vmdR_str+=vmdR_char[i];
        }
        for(int i=0;i<MSGSIZE;i++){
            if(SKFogPart1_char[i] =='!'){
                break;
            }
            SKFogPart1_str+=SKFogPart1_char[i];
        }
        for(int i=0;i<MSGSIZE;i++){
            if(SKFogPart2_char[i] =='!'){
                break;
            }
            SKFogPart2_str+=SKFogPart2_char[i];
        }
        
        element_t vmdL,vmdR,SKFogPart1,SKFogPart2,BlockHash,SKFogPart;
        element_init_G1(vmdL,pairing);
        element_init_G1(vmdR,pairing);
        element_init_G1(SKFogPart1,pairing);
        element_init_G1(SKFogPart2,pairing);
        element_init_G1(BlockHash,pairing);
        element_init_G1(SKFogPart,pairing);
        const char* vmdL_ptr = vmdL_str.data();
        const char* vmdR_ptr = vmdR_str.data();
        const char* SKFogPart1_ptr = SKFogPart1_str.data();
        const char* SKFogPart2_ptr = SKFogPart2_str.data();
        element_set_str(vmdL,vmdL_ptr,10);
        element_set_str(vmdR,vmdR_ptr,10);
        element_set_str(SKFogPart1,SKFogPart1_ptr,10);
        element_set_str(SKFogPart2,SKFogPart2_ptr,10);
        
        element_pow_zn(vmdL,vmdL,SK);
        element_div(BlockHash,vmdR,vmdL);
        
        element_pow_zn(SKFogPart2,SKFogPart2,SK);
        element_div(SKFogPart,SKFogPart1,SKFogPart2);
        
        char SKCloudPart_char[MSGSIZE];
        string SKCloudPart_str;
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,SKCloudPart_char, MSGSIZE, 0);
        close(sClient);
        
        for(int i=0;i<MSGSIZE;i++){
            if(SKCloudPart_char[i]=='!'){
                break;
            }
            SKCloudPart_str+=SKCloudPart_char[i];
        }
        element_t SKCloudPart,SKCloudPart_Real;
        element_init_G1(SKCloudPart,pairing);
        element_init_G1(SKCloudPart_Real,pairing);
        const char* SKCloudPart_ptr = SKCloudPart_str.data();
        element_set_str(SKCloudPart,SKCloudPart_ptr,10);
        element_div(SKCloudPart_Real,SKCloudPart,BlockHash);
        
        element_t EncSK;
        element_init_G1(EncSK,pairing);
        element_mul(EncSK,SKCloudPart_Real,SKFogPart);
        
        char EncSK_char[MSGSIZE];
        element_snprint(EncSK_char,MSGSIZE,EncSK);
        string EncSK_str;
        for(int i=0;i<MSGSIZE;i++){
            if ((EncSK_char[i] <= '9'&&EncSK_char[i] >= '0') || EncSK_char[i] == ']' || EncSK_char[i] == '[' || EncSK_char[i] == ',' || EncSK_char[i] == ' ') {
                EncSK_str+=EncSK_char[i];
            }
           else {
               break;
           }
            
        }
        char* EncSK_char_ptr = new char[EncSK_str.size()+1];
        for(int i=0;i<EncSK_str.size();i++){
            EncSK_char_ptr[i] = EncSK_str[i];
        }
        EncSK_char_ptr[EncSK_str.size()] = '\0';
        unsigned char FinalEncSK_UC[32];
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx,EncSK_char_ptr,strlen(EncSK_char_ptr));
        SHA256_Final(FinalEncSK_UC,&ctx);
        string temp_hash;
        for(int i=0;i<32;i++){
            temp_hash+=FinalEncSK_UC[i];
        }
        string hash_16 = binaryToHex(temp_hash);
        string FinalEncSK = hash_16.substr(0,64);
        unsigned char block_key[64];
        for(int i=0;i<64;i++){
            block_key[i] = FinalEncSK[i];
        }
        AES_KEY key_d;
        AES_set_decrypt_key(block_key,128,&key_d);
        string Hex_ciphertext;
        char Cipher_char[MSGSIZE];
        while(1){
            
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            recv(sClient,Cipher_char, MSGSIZE, 0);
            close(sClient);
            if(Cipher_char[0]=='E'){
                break;
            }
            
            for(int i=0;i<MSGSIZE;i++){
                if(Cipher_char[i]=='!'){
                    break;
                }
                Hex_ciphertext+=Cipher_char[i];
            }
        }
        
        string ciphertext_block_str = HexToStr(Hex_ciphertext);
        //cout<<ciphertext_block_str.size()<<endl;
        unsigned char* cipher_block= new unsigned char[BlockSize];
        for(int i=0;i<BlockSize;i++){
            cipher_block[i]=(unsigned char)ciphertext_block_str[i];
        }
        unsigned char* out_block = new unsigned char[BlockSize];
        for(int i=0; i<BlockSize/16;i++){
            AES_ecb_encrypt( cipher_block+i*16,out_block+i*16, &key_d, AES_DECRYPT);
        }

        string result;
        for(int i=0;i<BlockSize;i++){
            result+=out_block[i];
        }
        cout<<result<<endl;
        
        
    }
    
    
    
    return 0;
}
