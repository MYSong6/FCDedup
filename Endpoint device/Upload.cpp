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


#define CLOUD_SERVER_ADDRESS "X.X.X.X"
#define OWNER_SERVER_ADDRESS "X.X.X.X"
#define FOG_SERVER_ADDRESS_1 "X.X.X.X"
#define FOG_PORT_1 XX
#define CLOUD_PORT XX
#define OWNER_PORT XX
#define MSGSIZE XX
#define QUEUE_SIZE XX
int main(int argc, const char * argv[]) {
    
    string Device = "X";
    string Owner = "X";
    int file_ord = X;
    const int BlockSize = XX;
    
    char Owner_char[MSGSIZE],DeviceID_char[MSGSIZE], Dup_char[MSGSIZE];
    for(int i=0;i<Owner.size();i++){
        Owner_char[i] = Owner[i];
    }
    Owner_char[Owner.size()] = '!';
    string DeviceID=Device.substr(6,Device.size()-6);
    int deviceID =atoi(DeviceID.c_str());
    string tempDeviceID;
    if(deviceID<=5){
        tempDeviceID += DeviceID;
    }
    else{
        tempDeviceID += to_string(atoi(DeviceID.c_str())%6+1);
    }
    cout<<tempDeviceID<<endl;
   // cout<<tempDeviceID<<endl;
    for(int i=0;i<tempDeviceID.size();i++){
        DeviceID_char[i] = tempDeviceID[i];
    }
    DeviceID_char[tempDeviceID.size()] = '!';
    
    /*——————————————————————参数读取————————————————————————*/
    string FogID_str, DeviceID_str,SK_str,FogPK_str,N_str,g_str,Cloud_PK_str,Owner_PK_str;
    string SK_path_str = "XXXXX";
    fstream Read_ptr;
    Read_ptr.open(SK_path_str);
    getline(Read_ptr,FogID_str);
    getline(Read_ptr,DeviceID_str);
    getline(Read_ptr,SK_str);
    getline(Read_ptr,FogPK_str);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    getline(Read_ptr,Cloud_PK_str);
    getline(Read_ptr,Owner_PK_str);
    Read_ptr.close();
    int FOG_PORT = FOG_PORT_1+atoi(FogID_str.c_str());
    
    
    
   
   
    /*————————————————————————参数初始化——————————————————————*/
    mpz_t N;
    mpz_init(N);
    const char* N_char = N_str.data();
    mpz_init_set_str(N,N_char,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    const char* g_char = g_str.data();
    const char* FogPK_char = FogPK_str.data();
    const char* SK_char = SK_str.data();
    const char* CloudPK_char = Cloud_PK_str.data();
    const char* OwnerPK_char = Owner_PK_str.data();
    element_t g,FogPK,CloudPK,SK,OwnerPK;
    element_init_G1(g,pairing);
    element_init_Zr(SK,pairing);
    element_init_G1(FogPK,pairing);
    element_init_G1(CloudPK,pairing);
    element_init_G1(OwnerPK,pairing);
    element_set_str(g,g_char,10);
    element_set_str(FogPK,FogPK_char,10);
    element_set_str(CloudPK,CloudPK_char,10);
    element_set_str(SK,SK_char,10);
    element_set_str(OwnerPK,OwnerPK_char,10);
   
    
 
    
    /*————————————————————数据分块加密——————————————————————*/
    element_t EncSK_left,EncSK_right,EncSK;
    element_init_G1(EncSK_left, pairing);
    element_init_G1(EncSK_right, pairing);
    element_init_G1(EncSK, pairing);
    element_random(EncSK_left);
    element_random(EncSK_right);
    element_mul(EncSK,EncSK_left,EncSK_right);
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
    AES_KEY key_e;
    AES_set_encrypt_key(block_key,128,&key_e);
    string ALL_ciphertext;
 
    
    /*——————————TCP初始化——————————*/
    int sClient;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(FOG_PORT);
    addrSer.sin_addr.s_addr = inet_addr(FOG_SERVER_ADDRESS_1);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    
    socklen_t addrlen = sizeof(struct sockaddr);
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
   
    
    char FileOrd_char[MSGSIZE],BlockNum_char[MSGSIZE];
    string FileOrd_str = to_string(file_ord);
    for(int i=0;i<FileOrd_str.size();i++){
        FileOrd_char[i] =FileOrd_str[i];
    }
    FileOrd_char[FileOrd_str.size()]='!';
    
        
        
    string Plaintext_path = "xxxxxxxxxxxxxxxxxxxx";
    char ch;
    Read_ptr.open(Plaintext_path);
    int chNum=0,BlockNum;
    while(Read_ptr.get(ch)){
        chNum++;
    }
    Read_ptr.close();
    BlockNum=ceil(double(chNum)/double(BlockSize));
    string BlockNum_str = to_string(BlockNum);
    for(int i=0;i<BlockNum_str.size();i++){
        BlockNum_char[i] =BlockNum_str[i];
    }
    BlockNum_char[BlockNum_str.size()]='!';
   
    send(sClient,Owner_char, MSGSIZE, 0);
    send(sClient,DeviceID_char, MSGSIZE, 0);
    send(sClient,FileOrd_char, MSGSIZE, 0);
    send(sClient,BlockNum_char, MSGSIZE, 0);
    close(sClient);
    Read_ptr.open(Plaintext_path);

        
    if(Read_ptr.fail()){
        cout<<"文件不存在"<<endl;
    }
    string DataBuff;
    string All_plaintext;
    int current_char_num = 0;
    mpz_t block_h;
    mpz_init(block_h);
    char FogTagRight_char[MSGSIZE],FogTagLeft_char[MSGSIZE],FLAG[MSGSIZE],RECV_char[MSGSIZE];
    RECV_char[0]='R';
    RECV_char[1]='E';
    RECV_char[2]='C';
    string x_str,y_str,tau_str;
    char x_char[MSGSIZE],y_char[MSGSIZE],tau_char[MSGSIZE],h_char[MSGSIZE];
    int times=0;
    int now_charNum=0;
    while(Read_ptr.get(ch)){
        now_charNum++;
        DataBuff+=ch;
        current_char_num++;
        All_plaintext+=ch;
        if(current_char_num==BlockSize||now_charNum==chNum){
            /*————————————重新初始化客户端————————————*/
            
            
            times++;
        
            
            /*——————————生成两个哈希值————————*/
            char* DataBuff_char = new char[current_char_num+1];
            for(int i=0;i<current_char_num;i++){
                DataBuff_char[i] = DataBuff[i];
            }
            DataBuff_char[current_char_num]='\n';
            
            unsigned char SHA256OUT_Block[32];
            char* block_hash_char = new char[32];
            SHA256_CTX ctx1;
            SHA256_Init(&ctx1);
            SHA256_Update(&ctx1,DataBuff_char,strlen(DataBuff_char));
            SHA256_Final(SHA256OUT_Block,&ctx1);
         
            
            string temp_hash;
            for(int i=0;i<32;i++){
                block_hash_char[i]=SHA256OUT_Block[i];
                temp_hash+=SHA256OUT_Block[i];
            }
            element_t BlockHash1,BlockHash;
            element_init_Zr(BlockHash,pairing);
            element_init_G1(BlockHash1, pairing);
            element_from_hash(BlockHash1,block_hash_char,32);
            string hash_16 = binaryToHex(temp_hash);
            string Finalhash= hash_16.substr(0,64);
                //string Hex_plaintext = binaryToHex(DataBuff);
            const char* block_h_char= Finalhash.data();
            mpz_init_set_str(block_h,block_h_char,16);
            element_set_mpz(BlockHash,block_h);
            delete[] DataBuff_char;
            delete[] block_hash_char;
                
           
                
            /*—————————————BEGIN——————————————*/
            /*————————计算雾级去重文件标签————————*/
            /*——{Fog_tag_left,Fog_tag_right}——*/
     
            element_t r1,Fog_tag_left,Fog_tag_right,HAddSK, PK_f_r;
            element_init_Zr(r1,pairing);
            element_init_Zr(HAddSK,pairing);
            element_init_G1(Fog_tag_left,pairing);
            element_init_G1(Fog_tag_right,pairing);
            element_init_G1(PK_f_r,pairing);
            element_random(r1);
            element_pow_zn(Fog_tag_left, g, r1);
            element_add(HAddSK, SK,BlockHash);
            element_pow_zn(PK_f_r,FogPK,r1);
            element_pow_zn(Fog_tag_right,g,HAddSK);
            element_mul(Fog_tag_right, Fog_tag_right,PK_f_r);
            element_snprint(FogTagRight_char, MSGSIZE, Fog_tag_right);
            element_snprint(FogTagLeft_char, MSGSIZE, Fog_tag_left);
            string FogTagRight_str, FogTagLeft_str;
            for(int i=0;i<MSGSIZE;i++){
                if ((FogTagRight_char[i] <= '9'&&FogTagRight_char[i] >= '0') || FogTagRight_char[i] == ']' || FogTagRight_char[i] == '[' || FogTagRight_char[i] == ',' || FogTagRight_char[i] == ' ') {
                        FogTagRight_str+=FogTagRight_char[i];
                    }
                else {
                    break;
                }
            }
            for(int i=0;i<MSGSIZE;i++){
                if ((FogTagLeft_char[i] <= '9'&&FogTagLeft_char[i] >= '0') || FogTagLeft_char[i] == ']' || FogTagLeft_char[i] == '[' || FogTagLeft_char[i] == ',' || FogTagLeft_char[i] == ' ') {
                    FogTagLeft_str+=FogTagLeft_char[i];
                }
                else {
                    break;
                }
            }
            FogTagLeft_char[FogTagLeft_str.size()]='!';
            FogTagRight_char[FogTagRight_str.size()]='!';
            /*——————————————END————————————————*/
            /*————————计算雾级去重文件标签————————*/
            
            
            int sendNo1, sendNo2, recvNo;
            /*————————————————BEGIN————————————————*/
            /*————————————发送雾级去重标签————————————*/
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            sendNo1=send(sClient,FogTagLeft_char, MSGSIZE, 0);
          
            close(sClient);
            
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
           
            sendNo2=send(sClient,FogTagRight_char, MSGSIZE, 0);
          
            close(sClient);
            
            cout<<"FogTagRight_char:"<<sendNo1<<endl;
            cout<<"FogTagLeft_char:"<<sendNo2<<endl;
            /*————————————————END————————————————*/
            /*————————————发送雾级去重标签————————————*/
            
            //element_printf("Fog_tag_left: %B\n",Fog_tag_left);
            //element_printf("Fog_tag_right: %B\n",Fog_tag_right);
                 
            /*————————————————BEGIN——————————————*/
            /*——————————————计算验证元数据——————————*/
            /*——————————{vm_left,vm_right}————————*/
            
            element_t r2, vm_left, vm_right, PK_o_r;
            element_init_G1(vm_left, pairing);
            element_init_G1(vm_right, pairing);
            element_init_G1(PK_o_r, pairing);
            element_init_Zr(r2, pairing);
            element_random(r2);
            element_pow_zn(vm_left, g, r2);
            element_pow_zn(PK_o_r,OwnerPK, r2);
            element_mul(vm_right, PK_o_r, BlockHash1);
           
         
               
            string vm_left_str, vm_right_str;
            char vm_left_char[MSGSIZE], vm_right_char[MSGSIZE];
            element_snprint(vm_left_char,MSGSIZE,vm_left);
            element_snprint(vm_right_char,MSGSIZE,vm_right);
                
            for (int i = 0; i<2048; i++) {
                if ((vm_left_char[i] <= '9'&&vm_left_char[i] >= '0') || vm_left_char[i] == ']' || vm_left_char[i] == '[' || vm_left_char[i] == ',' || vm_left_char[i] == ' ') {
                        vm_left_str+=vm_left_char[i];
                }
                else {
                    break;
                }
            }
            for (int i = 0; i<2048; i++) {
                if ((vm_right_char[i] <= '9'&&vm_right_char[i] >= '0') || vm_right_char[i] == ']' || vm_right_char[i] == '[' || vm_right_char[i] == ',' || vm_right_char[i] == ' ') {
                    vm_right_str+=vm_right_char[i];
                }
                else {
                    break;
                }
            }
            vm_left_char[vm_left_str.size()] = '!';
            vm_right_char[vm_right_str.size()] = '!';
           
            /*————————————————END————————————————*/
            /*——————————————计算验证元数据——————————*/
            
            
            /*————————BEGIN————————*/
            /*————接收雾级查重结果————*/
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
           
            recvNo=recv(sClient,Dup_char, MSGSIZE, 0);
            
            close(sClient);
           
   
                   
            string Dup_str;
            for(int i=0;i<MSGSIZE;i++){
                if(Dup_char[i]=='!'){
                    break;
                }
                Dup_str+=Dup_char[i];
            }
         
            /*————————END————————*/
            /*————接收雾级查重结果————*/
            
            
            
            
            /*————————BEGIN————————*/
            /*————————雾级重复———————*/
            if(Dup_str=="Dup"){
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                sendNo2=send(sClient,vm_left_char , MSGSIZE, 0);
               
                close(sClient);
                
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                sendNo2=send(sClient,vm_right_char , MSGSIZE, 0);
             
                close(sClient);
                
            }
            else if(Dup_str=="NoDup"){ /*——————雾级不重复———————*/
 
               
                mpz_t r_t;
                mpz_init(r_t);
                pbc_mpz_random(r_t, N);
                element_t r,g_r,x,FogPK_r,z,y;
                element_init_Zr(r,pairing);
                element_init_G1(g_r,pairing);
                element_init_G1(x, pairing);
                element_init_G1(FogPK_r,pairing);
                element_init_G1(y,pairing);
                element_init_G1(z,pairing);
                    
                element_set_mpz(r,r_t);
                element_pow_zn(g_r,g,r);
                element_pow_zn(FogPK_r, FogPK, r);
                element_mul(x,BlockHash1,FogPK_r);
              
                
                element_snprint(x_char, MSGSIZE, x);
                x_str.clear();
                y_str.clear();
                for (int i = 0; i<MSGSIZE; i++) {
                    if ((x_char[i] <= '9'&&x_char[i] >= '0') || x_char[i] == ']' || x_char[i] == '[' || x_char[i] == ',' || x_char[i] == ' ') {
                        x_str+=x_char[i];
                    }
                    else {
                        break;
                    }
                }
                x_char[x_str.size()]='!';
                
                
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
               
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                sendNo1=send(sClient,x_char, MSGSIZE, 0);
               
                close(sClient);
                 
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
                recvNo=recv(sClient,y_char, MSGSIZE, 0);
                close(sClient);
                
               
                for (int i = 0; i<MSGSIZE; i++) {
                    if(y_char[i]=='!'){
                        break;
                    }
                    y_str+=y_char[i];
                }
                const char* y_ptr = y_str.data();
                element_set_str(y,y_ptr,10);
                  
                element_div(z,y,g_r);
              
                element_printf("z: %B\n",z);
          
                
                
                /*———————————————BEGIN————————————————*/
                /*——————————云查重标签辅助生成————————————*/
                element_t CloudPK_r,tau;
                element_init_G1(CloudPK_r, pairing);
                element_init_G1(tau,pairing);
                element_pow_zn(CloudPK_r, CloudPK, r);
                element_mul(tau,z,CloudPK_r);
                tau_str.clear();
                element_snprint(tau_char, MSGSIZE, tau);
                for (int i = 0; i<MSGSIZE; i++) {
                    if ((tau_char[i] <= '9'&&tau_char[i] >= '0') || tau_char[i] == ']' || tau_char[i] == '[' || tau_char[i] == ',' || tau_char[i] == ' ') {
                        tau_str+=tau_char[i];
                    }
                    else {
                        break;
                    }
                }
                for(int i=0;i<tau_str.size();i++){
                    tau_char[i] = tau_str[i];
                }
                tau_char[tau_str.size()]='!';
                string sh = Finalhash.substr(0,15);
                for(int i=0;i<sh.size();i++){
                    h_char[i] =sh[i];
                }
                h_char[sh.size()] = '!';
                /*————————————————END————————————————*/
                /*——————————云查重标签辅助生成————————————*/
                
                
                /*——————————————BEGIN————————————————*/
                /*——————————发送云查重标签辅助————————————*/
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
                sendNo1=send(sClient,tau_char, MSGSIZE, 0);
                close(sClient);
                
                
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                sendNo2=send(sClient,h_char, MSGSIZE, 0);
               
                close(sClient);
                
                /*——————————————END————————————————*/
                /*——————————发送云查重标签辅助————————————*/
                
                /*————————————————BEGIN——————————————*/
                /*————————————接收云查重结果————————————*/
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                recvNo=recv(sClient,Dup_char, MSGSIZE, 0);
               
                close(sClient);
                /*————————————————BEGIN——————————————*/
                /*————————————接收云查重结果————————————*/
                
                if(Dup_char[0]=='N'){//cross-owner 不重复
                    /*——————————————BEGIN———————————————*/
                    /*————————————发送密钥分享————————————*/
                   
                    element_t SKToFog,SKToCloud,a1,a2,PKCloud_a1,g_a1,PKFog_a2,g_a2;
                    element_init_G1(SKToFog,pairing);
                    element_init_G1(SKToCloud, pairing);
                    element_init_Zr(a1, pairing);
                    element_init_Zr(a2, pairing);
                    element_init_G1(PKCloud_a1,pairing);
                    element_init_G1(g_a1,pairing);
                    element_init_G1(PKFog_a2,pairing);
                    element_init_G1(g_a2,pairing);
                    element_random(a1);
                    element_random(a2);
                    element_pow_zn(PKCloud_a1,CloudPK,a1);
                    element_pow_zn(PKFog_a2,FogPK,a2);
                    element_pow_zn(g_a1,g,a1);
                    element_pow_zn(g_a2,g,a2);
                    element_mul(SKToCloud,BlockHash1,EncSK_right);
                    //element_printf("SKToCloud!!!!!!: %B\n",SKToCloud);
                    element_mul(SKToCloud, SKToCloud, PKCloud_a1);
                    element_mul(SKToFog,PKFog_a2,EncSK_left);
                  
                    char SKToCloud_char[MSGSIZE], SKToFog_char[MSGSIZE],g_a1_char[MSGSIZE],g_a2_char[MSGSIZE];
                
                    element_snprint(SKToCloud_char, MSGSIZE, SKToCloud);
                    element_snprint(SKToFog_char, MSGSIZE, SKToFog);
                    element_snprint(g_a1_char, MSGSIZE, g_a1);
                    element_snprint(g_a2_char, MSGSIZE, g_a2);
                
                    string SKToCloud_str, SKToFog_str, g_a1_str,g_a2_str;
                    for(int i=0;i<MSGSIZE;i++){
                        if ((SKToCloud_char[i] <= '9'&&SKToCloud_char[i] >= '0') || SKToCloud_char[i] == ']' || SKToCloud_char[i] == '[' || SKToCloud_char[i] == ',' || SKToCloud_char[i] == ' ') {
                            SKToCloud_str+=SKToCloud_char[i];
                        }
                        else {
                            break;
                        }
                    }
                    for(int i=0;i<MSGSIZE;i++){
                        if ((g_a1_char[i] <= '9'&&g_a1_char[i] >= '0') || g_a1_char[i] == ']' || g_a1_char[i] == '[' || g_a1_char[i] == ',' || g_a1_char[i] == ' ') {
                        g_a1_str+=g_a1_char[i];
                            }
                        else {
                            break;
                        }
                    }
                    for(int i=0;i<MSGSIZE;i++){
                        if ((SKToFog_char[i] <= '9'&&SKToFog_char[i] >= '0') || SKToFog_char[i] == ']' || SKToFog_char[i] == '[' || SKToFog_char[i] == ',' || SKToFog_char[i] == ' ') {
                            SKToFog_str+=SKToFog_char[i];
                        }
                        else {
                            break;
                        }
                    }
                    for(int i=0;i<MSGSIZE;i++){
                        if ((g_a2_char[i] <= '9'&&g_a2_char[i] >= '0') || g_a2_char[i] == ']' || g_a2_char[i] == '[' || g_a2_char[i] == ',' || g_a2_char[i] == ' ') {
                            g_a2_str+=g_a2_char[i];
                        }
                        else {
                            break;
                        }
                    }
                
                    SKToCloud_char[SKToCloud_str.size()]='!';
                    SKToFog_char[SKToFog_str.size()]='!';
                    g_a1_char[g_a1_str.size()]='!';
                    g_a2_char[g_a2_str.size()]='!';
                
                    sClient = socket(AF_INET,SOCK_STREAM,0);
                    if(sClient == -1){
                        cout<<"客户端初始化步骤1失败"<<endl;
                        return 0;
                    }
                    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                    }
                  
                    sendNo1=send(sClient, SKToCloud_char, MSGSIZE, 0);
                   
                    close(sClient);
                
                    sClient = socket(AF_INET,SOCK_STREAM,0);
                    if(sClient == -1){
                        cout<<"客户端初始化步骤1失败"<<endl;
                        return 0;
                    }
                    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                    }
                   
                    sendNo1=send(sClient, g_a1_char, MSGSIZE, 0);
                   
                    close(sClient);
                
                    sClient = socket(AF_INET,SOCK_STREAM,0);
                    if(sClient == -1){
                        cout<<"客户端初始化步骤1失败"<<endl;
                        return 0;
                    }
                    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                    }
                    
                    sendNo2=send(sClient, SKToFog_char, MSGSIZE, 0);
                  
                    close(sClient);
                
                    sClient = socket(AF_INET,SOCK_STREAM,0);
                    if(sClient == -1){
                        cout<<"客户端初始化步骤1失败"<<endl;
                        return 0;
                    }
                    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                    }
                   
                    sendNo2=send(sClient, g_a2_char, MSGSIZE, 0);
                   
                    close(sClient);
                
                    element_printf("EncSK_left: %B\n",EncSK_left);
                }
                
                /*————-————————重复只发送验证元数据——————————*/
                /*————-—不重复发送密钥分享，验证元数据，密文—————*/
                /*————————————————发送验证元数据————————————*/
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
                
                sendNo2=send(sClient,vm_left_char , MSGSIZE, 0);
               
                close(sClient);
                
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
               
                sendNo2=send(sClient,vm_right_char , MSGSIZE, 0);
               
                close(sClient);
                
                
                if(Dup_char[0]=='N'){
                    /*————————————————BEGIN——————————————*/
                    /*————————————————加密————————————————*/
                    unsigned char block_plaintext[BlockSize];
                    for(int i=0;i<BlockSize;i++){
                        block_plaintext[i] = DataBuff[i];
                    }
                    unsigned char block_ciphertext[BlockSize];
                    
                    for(int i=0; i<BlockSize/16;i++){
                        AES_ecb_encrypt(block_plaintext+i*16,block_ciphertext+i*16, &key_e, AES_ENCRYPT);
                    }
                   
                    close(sClient);
                    //string block_ciphertext_str = (char*)block_ciphertext;
                    string Hex_ciphertext;
                    for(int i=0;i<BlockSize;i++){
                        string temp_res = unsignedCharToHexString(block_ciphertext[i]);
                        Hex_ciphertext += temp_res;
                    }
                    
                    
                    /*——————————发送密文——————————*/
                    char HexCipher_char[MSGSIZE], CipherLen_char[MSGSIZE];
                    string CipherLen_str = to_string(Hex_ciphertext.size());
                    cout<<"密文长度："<<CipherLen_str<<endl;
                    
                    for(int i=0;i<MSGSIZE;i++){
                        CipherLen_char[i] =CipherLen_str[i];
                    }
                    CipherLen_char[CipherLen_str.size()]='!';
                    
                    sClient = socket(AF_INET,SOCK_STREAM,0);
                    if(sClient == -1){
                        cout<<"客户端初始化步骤1失败"<<endl;
                        return 0;
                    }
                    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                    }
                    sendNo2=send(sClient,CipherLen_char, MSGSIZE, 0);
                    close(sClient);
                    
                    int send_time = ceil(double(atoi(CipherLen_str.c_str()))/double(MSGSIZE));
                    int now_ord = 0;
                    cout<<"发送次数"<<send_time<<endl;
                    double communication_time_ciph = 0;
                    for(int i=0;i<send_time;i++){
                        if(i==send_time-1){
                            for(int j =0;j<(atoi(CipherLen_str.c_str())-i*MSGSIZE);j++){
                                HexCipher_char[j] =Hex_ciphertext[now_ord];
                                now_ord++;
                            }
                        }
                        else{
                            for(int j =0; j<MSGSIZE; j++){
                                HexCipher_char[j] =Hex_ciphertext[now_ord];
                                now_ord++;
                            }
                        }
                        sClient = socket(AF_INET,SOCK_STREAM,0);
                        
                        if(sClient == -1){
                            cout<<"客户端初始化步骤1失败"<<endl;
                            return 0;
                        }
                        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                        }
                      
                        sendNo2=send(sClient,HexCipher_char,MSGSIZE, 0);
                      
        
                        close(sClient);
                    }
                    
                    
                }
                
            }
          
                
            DataBuff.clear();
            current_char_num = 0;
     
        }
            
    }
    return 0;
}
