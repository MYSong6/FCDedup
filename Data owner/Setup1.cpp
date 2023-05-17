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
#define FOG_PORT_1 XX
#define FOG_PORT_2 XX
#define FOG_PORT_3 XX
#define CLOUD_PORT XX
#define MSGSIZE XX
int main(int argc, const char * argv[]) {
    
    
    
    // Read  N
    string USER_ID = "XX";
    int Sensor_Num = X;
    
    
    
    /*——————————————————BEGIN————————————————————*/
    /*———————————————获取云公开参数——————————————————*/
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
    cout<<"连接云服务器成功"<<endl;
    
    char N_char[MSGSIZE],g_char[MSGSIZE], PK_Cloud_char[MSGSIZE];
    recv(sClient,N_char, MSGSIZE, 0);
    recv(sClient,g_char, MSGSIZE, 0);
    recv(sClient,PK_Cloud_char, MSGSIZE, 0);
    
    string N_str, g_str, PK_Cloud_str;
    for(int i=0;i<MSGSIZE; i++){
        if(N_char[i]=='!'){
            break;
        }
        N_str += N_char[i];
    }
    for(int i=0;i<MSGSIZE; i++){
        if(g_char[i]=='!'){
            break;
        }
        g_str += g_char[i];
    }
    for(int i=0;i<MSGSIZE; i++){
        if(PK_Cloud_char[i]=='!'){
            break;
        }
        PK_Cloud_str += PK_Cloud_char[i];
    }
    cout<<"云公共参数获取结束"<<endl;
    /*————————————————————END————————————————————*/
    /*———————————————获取云公开参数——————————————————*/
    
    
    /*——————————————————BEGIN————————————————————*/
    /*———————————————存储云公开参数——————————————————*/
    string cloud_parameter_path = "/Users/songmingyang/Desktop/paper4/Data/";
    cloud_parameter_path += USER_ID;
    cloud_parameter_path += "/Cloud_parameter.txt";
    fstream write_ptr;
    write_ptr.open(cloud_parameter_path,ios::out);
    
    write_ptr<<N_str<<endl;
    write_ptr<<g_str<<endl;
    write_ptr<<PK_Cloud_str<<endl;
    
    write_ptr.close();
    /*———————————————————END——————————————————————*/
    /*———————————————存储云公开参数——————————————————*/
    
    
    /*——————————————————BEGIN————————————————————*/
    /*———————————————初始化双线性映射————————————————*/
    mpz_t N;
    mpz_init(N);
    const char* ptr_N = N_str.data();
    mpz_init_set_str(N,ptr_N,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    //cout<<N<<endl;
    /*—————————————————————END——————————————————————*/
    /*———————————————初始化双线性映射——————————————————*/
    
    
    string Owner_SK_path = "XXXXX";
    Owner_SK_path += USER_ID;
    Owner_SK_path += "XXX";
    string Owner_PK_path = "XXX";
    Owner_PK_path += USER_ID;
    Owner_PK_path +="XXXt";
    string Owner_SV_path = "XXX";
    Owner_SV_path += USER_ID;
    Owner_SV_path +="XXX";
    
    string Sensor_SK_path = "XXXX";
    Sensor_SK_path += USER_ID;
    Sensor_SK_path += "XXXX";
    string Sensor_RT_path = "XXXX";
    Sensor_RT_path += USER_ID;
    Sensor_RT_path +="XXXXX";
    
    /*————————————————————————BEGIN—————————————————————*/
    /*———————————————密钥，公钥，秘密值生成存储————————————————*/
    mpz_t sk_t,sv_t;
    mpz_init(sk_t);
    mpz_init(sv_t);
    pbc_mpz_random(sk_t, N);
    pbc_mpz_random(sv_t, N);
    element_t g,sk,pk,sv;
    element_init_G1(g,pairing);
    element_init_G1(pk,pairing);
    element_init_Zr(sv,pairing);
    element_init_Zr(sk,pairing);
    const char* ptr_g = g_str.data();
    element_set_str(g,ptr_g,10);
    element_set_mpz(sk,sk_t);
    element_set_mpz(sv,sv_t);
    element_pow_zn(pk,g,sk);
    
    write_ptr.open(Owner_SK_path,ios::out);
    string sk_str = mpz_get_str(NULL, 10, sk_t);
    write_ptr<<sk_str<<endl;
    write_ptr.close();
    
    write_ptr.open(Owner_SV_path,ios::out);
    string sv_str = mpz_get_str(NULL, 10, sv_t);
    write_ptr<<sv_str<<endl;
    write_ptr.close();
    
    
    char PK_path_char[1000];
    strcpy(PK_path_char,Owner_PK_path.c_str());
    FILE* stream = fopen(PK_path_char,"w");
    
    char PK_char[2048];
    element_snprint(PK_char,2048,pk);
    
    for (int i = 0; i<2048; i++) {
        if ((PK_char[i] <= '9'&&PK_char[i] >= '0') || PK_char[i] == ']' || PK_char[i] == '[' || PK_char[i] == ',' || PK_char[i] == ' ') {
            fprintf(stream, "%c", PK_char[i]);
        }
        else {
            fprintf(stream, "%c", '\n');
            break;
        }
    }
    fclose(stream);
    /*————————————————————————END———————————————————————*/
    /*———————————————密钥，公钥,秘密值生成存储————————————————*/
    
    
    
    
    /*————————————————————————BEGIN—————————————————————————*/
    /*———————————————为Sensors生成密钥和注册标签————————————————*/
    write_ptr.open(Sensor_SK_path,ios::out);
    char Sensor_RT_path_char[1000];
    strcpy(Sensor_RT_path_char,Sensor_RT_path.c_str());
    FILE* stream1 = fopen(Sensor_RT_path_char,"w");
    for(int j=0;j<Sensor_Num;j++){
        
        mpz_t Sensor_sk_t;
        mpz_init(Sensor_sk_t);
        pbc_mpz_random(Sensor_sk_t, N);
        element_t Sensor_RT, Sensor_sk, sub_sv_Sensorsk;
        element_init_G1(Sensor_RT,pairing);
        element_init_Zr(Sensor_sk,pairing);
        element_init_Zr(sub_sv_Sensorsk,pairing);
        element_set_mpz(Sensor_sk, Sensor_sk_t);
        element_sub(sub_sv_Sensorsk,sv,Sensor_sk);
        element_pow_zn(Sensor_RT,g,sub_sv_Sensorsk);
        
        //element_printf("sv: %B\n",sv);
        //element_printf("Sensor_sk: %B\n",Sensor_sk);
        //element_printf("sub_sv_Sensorsk: %B\n",sub_sv_Sensorsk);
        
        string Sensor_sk_str = mpz_get_str(NULL, 10, Sensor_sk_t);
        write_ptr<<Sensor_sk_str <<endl;
        
        char Sensor_RT_char[2048];
        element_snprint(Sensor_RT_char,2048,Sensor_RT);
        
        for (int i = 0; i<2048; i++) {
            if ((Sensor_RT_char[i] <= '9'&&Sensor_RT_char[i] >= '0') || Sensor_RT_char[i] == ']' || Sensor_RT_char[i] == '[' || Sensor_RT_char[i] == ',' || Sensor_RT_char[i] == ' ') {
                fprintf(stream1, "%c", Sensor_RT_char[i]);
            }
            else {
                fprintf(stream1, "%c", '\n');
                break;
            }
        }
        
    }
    write_ptr.close();
    fclose(stream1);
    cout<<"数据收集节点私钥，注册标签生成完成"<<endl;
    
    /*————————————————————————END———————————————————————————*/
    /*———————————————为Sensors生成密钥和注册标签————————————————*/
    
    
    
    
    return 0;
}
