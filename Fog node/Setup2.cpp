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


#define MSGSIZE 1024
#define PAR_LENGTH 256
#define PORT_LOC 12665

int main(int argc, const char * argv[]) {


	string FOG_ID = "fog_1";
	const char*  FOG_ID_char = FOG_ID.data();

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", FOG_ID_char, 3306, NULL, 0)) {
		cout << "Database connect success" << endl;
	}
	string SELECT_SQL_str = "SELECT * FROM cloud_parameter";
	const char* SELECT_SQL_char = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char)) {
		cout << "Query fail" << endl;
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
		cout << "Query fail" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	string SK_str = nextRow[0];
	string ISK_str = nextRow[1];
	string FogPK_str = nextRow[2];


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
	element_init_Zr(SK, pairing);
	element_init_Zr(ISK, pairing);
	element_init_G1(g, pairing);
	element_init_G1(CloudPK, pairing);
	element_init_G1(FogPK, pairing);
	element_set_str(SK, SK_char, 10);
	element_set_str(ISK, ISK_char, 10);
	element_set_str(FogPK, FogPK_char, 10);
	element_set_str(g, g_char, 10);
	element_set_str(CloudPK, CloudPK_char, 10);


	while (1) {


		WSADATA wsaData;
		SOCKET sListen;
		SOCKET sClient;
		SOCKADDR_IN local;
		SOCKADDR client_user;
		memset(&local, 0, sizeof(local));

		char szMessage[MSGSIZE];
		int ret;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sListen == INVALID_SOCKET) {
			cout << "sListent error" << endl;
		}

		local.sin_family = AF_INET;
		local.sin_port = htons(PORT_LOC);
		local.sin_addr.s_addr = htonl(INADDR_ANY);

		while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		}
		cout << "Bind Success!" << endl;

		while (listen(sListen, 20) == -1) {
		} 

		int nsize = sizeof(SOCKADDR);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		char Rec_PK[MSGSIZE], JPK_PK[MSGSIZE];
		string Rec_PK_str,JPK_str;
		recv(sClient,Rec_PK , MSGSIZE, NULL);
		for (int i = 0; i < MSGSIZE; i++) {
			if (Rec_PK[i] == '!') {
				break;
			}
			Rec_PK_str += Rec_PK[i];
		}
		const char* Rec_PK_ptr = Rec_PK_str.data();
		element_t pk, joint_pk;
		element_init_G1(joint_pk, pairing);
		element_init_G1(pk, pairing);
		element_set_str(pk, Rec_PK_ptr, 10);
		element_pow_zn(joint_pk, pk, SK);
		element_snprint(JPK_PK, 2* PAR_LENGTH, joint_pk);

		for (int i = 0; i < 2 * PAR_LENGTH; i++) {
			if ((JPK_PK[i] <= '9'&&JPK_PK[i] >= '0') || JPK_PK[i] == ']' || JPK_PK[i] == '[' || JPK_PK[i] == ',' || JPK_PK[i] == ' ') {
				JPK_str += JPK_PK[i];
			}
			else {
				break;
			}
		}
		JPK_PK[JPK_str.size()] = '!';
		send(sClient, JPK_PK, MSGSIZE, NULL);
		closesocket(sClient);

		WSACleanup();

	}

	return 0;
}