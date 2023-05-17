
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif

//#pragma comment(lib,"ws2_32.lib")
# include<iostream>
//#pragma comment(lib,"libssl.lib")
//#pragma comment(lib, "libcrypto.lib")
#include <fstream>
#include "mysql.h"
#include "alibabacloud/oss/OssClient.h"
#include "openssl/aes.h"
#include "openssl/sha.h"
# include "gmp.h"
# include <stdio.h>
# include <string.h>
# include "pbc.h"

#pragma comment(lib,"ws2_32.lib")
//# include <winsock.h>
# include <OssClientImpl.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>

using namespace std;
using namespace AlibabaCloud::OSS;

#define CLOUD_SERVER_ADDRESS "X.X.X.X"
#define PORT_CLOUD_SERVER X
#define MSGSIZE XX
#define PAR_LENGTH XX

int main(int argc, const char * argv[]) {
    
    
    string FOG_ID = "XX";
    string Target_Fog_ID = "XX";
    const char*  FOG_ID_char = FOG_ID.data();
    const char* Cloud_DB = "XX";
    const int BlockSize = XXX;

    /*°™°™°™°™°™°™¥” ˝æ›ø‚÷–∂¡»°≤Œ ˝°™°™°™°™°™°™*/
    MYSQL ceshi;
    mysql_init(&ceshi);
    MYSQL_ROW row;
    if (mysql_real_connect(&ceshi, "XXX", "XXX", "XXX", FOG_ID_char, XX, NULL, 0)) {
        cout << " ˝æ›ø‚¡¥Ω”≥…π¶" << endl;
    }
    string SELECT_SQL_str = "SELECT * FROM cloud_parameter";
    const char* SELECT_SQL_char1 = SELECT_SQL_str.data();
    if (0 != mysql_query(&ceshi, SELECT_SQL_char1)) {
        cout << "≤È—Ø ß∞‹" << endl;
        system("pause");
        return 0;
    }

    MYSQL_RES *res;
    MYSQL_FIELD *field;
    MYSQL_ROW nextRow;
    res = mysql_store_result(&ceshi);
    nextRow = mysql_fetch_row(res);
    string N_str = nextRow[0];
    string g_str = nextRow[1];
    string CloudPK_str = nextRow[2];
    SELECT_SQL_str.clear();
    SELECT_SQL_str = "SELECT * FROM fog_parameter";
    const char* SELECT_SQL_char2 = SELECT_SQL_str.data();
    if (0 != mysql_query(&ceshi, SELECT_SQL_char2)) {
        cout << "≤È—Ø ß∞‹" << endl;
        system("pause");
        return 0;
    }
    res = mysql_store_result(&ceshi);
    nextRow = mysql_fetch_row(res);
    string SK_str = nextRow[0];
    string ISK_str = nextRow[1];
    string FogPK_str = nextRow[2];


    /*°™°™°™°™°™°™≤Œ ˝≥ı ºªØ°™°™°™°™°™°™*/
    mpz_t N;
    mpz_init(N);
    const char* ptr_N = N_str.data();
    mpz_init_set_str(N, ptr_N, 10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param, N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing, param);
    const char* g_char = g_str.data();
    const char* CloudPK_char = CloudPK_str.data();
    const char* SK_char = SK_str.data();
    const char* ISK_char = ISK_str.data();
    const char* FogPK_char = FogPK_str.data();
    element_t g, CloudPK, SK, ISK, FogPK;
    element_init_Zr(SK,pairing);
    element_init_Zr(ISK, pairing);
    element_init_G1(g, pairing);
    element_init_G1(CloudPK, pairing);
    element_init_G1(FogPK, pairing);
    element_set_str(SK,SK_char,10);
    element_set_str(ISK, ISK_char, 10);
    element_set_str(FogPK, FogPK_char, 10);
    element_set_str(g,g_char,10);
    element_set_str(CloudPK, CloudPK_char, 10);
    
   
   
    
   
   
    element_t pk,joint_pk;
    element_init_G1(joint_pk,pairing);
    element_init_G1(pk, pairing);
    
    FILE * stream;
    string str_pk;
    int index = 1;
    
    std::string AccessKeyId = "XX";
    std::string AccessKeySecret = "XX";
    std::string Endpoint = "XX";
    std::string BucketName = "XX";
    std::string ObjectName1 = "XX";
    cout << FILENAME_str << endl;

    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
    DownloadObjectRequest request1(BucketName, ObjectName1, "XX");

    auto outcome1 = client.ResumableDownloadObject(request1);


    if (outcome1.isSuccess()) {
        std::cout << "Œƒº˛œ¬‘ÿ≥…π¶" << outcome1.result().Metadata().ContentLength() << std::endl;
    }
    else {
        /* “Ï≥£¥¶¿Ì°£*/
        std::cout << "Get cloud public parameter fail" <<
            ",code:" << outcome1.error().Code() <<
            ",message:" << outcome1.error().Message() <<
            ",requestId:" << outcome1.error().RequestId() << std::endl;
        system("pause");
        ShutdownSdk();
        return -1;
    }
   
    para_p.open("XX");
    getline(para_p,str_pk);
        const char* p4 = str_pk.data();
        element_set_str(pk,p4,10);
        element_printf("pk: %B\n", pk);
        element_pow_zn(joint_pk,pk,sk);
        string  Path_joint_pk;
        Path_joint_pk += "XX";
        Path_joint_pk += to_string(index);
        Path_joint_pk += ".txt";
        char* temp_Path_joint_pk = Path_joint_pk.data();
        stream=fopen(temp_Path_joint_pk,"a+");
        char _n[2] = {'\0','\n'};
        element_out_str(stream,10,joint_pk);
        fwrite(_n, sizeof(char), sizeof(_n),stream);
        fclose(stream);
    
    
    para_p.close();
    

    return 0;
}
