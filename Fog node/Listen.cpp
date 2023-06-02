
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

#define LOC_PORT XXX
#define MSGSIZE 2048
#define PAR_LENGTH 512
#define QUEUE_SIZE 1

int main() {


	string FOG_ID = "fog_1";
	const char*  FOG_ID_char = FOG_ID.data();


	/*——————从数据库中读取参数——————*/
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", FOG_ID_char, 3306, NULL, 0)) {
		cout << "Database linked" << endl;
	}
	string SELECT_SQL_str = "SELECT * FROM cloud_parameter";
	const char* SELECT_SQL_char1 = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char1)) {
		cout << "Query Fail" << endl;
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
		cout << "Query Fail" << endl;
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

	

	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR client_user;
	memset(&local, 0, sizeof(local));
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "Listen error" << endl;
	}
	local.sin_family = AF_INET;
	local.sin_port = htons(LOC_PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind fail" << endl;

	WSADATA wsaData1;
	SOCKET sClient_Fog;
	SOCKADDR_IN server1;
	WSAStartup(0x0202, &wsaData1);
	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server1, 0, sizeof(SOCKADDR_IN));
	server1.sin_family = AF_INET;
	server1.sin_port = htons(PORT_CLOUD_SERVER);
	server1.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);

	int nsize = sizeof(SOCKADDR);
	while (1) {
		char Owner_char[MSGSIZE], Order_char[MSGSIZE], SKFogPart1_char[MSGSIZE],SKFogPart2_char[MSGSIZE];
		string Owner_str,Order_str, SKFogPart1_str, SKFogPart2_str;

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient, Order_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient, Owner_char, MSGSIZE, NULL);
		closesocket(sClient);

		
		for (int i = 0; i < MSGSIZE; i++) {
			if (Owner_char[i] == '!') {
				break;
			}
			Owner_str += Owner_char[i];
		}

		for (int i = 0; i < MSGSIZE; i++) {
			if (Order_char[i] == '!') {
				break;
			}
			Order_str += Order_char[i];
		}



		const char* x_ptr = x_str.data();
		element_t x,y;
		element_init_G1(x, pairing);
		element_init_G1(y, pairing);
		element_set_str(x, x_ptr, 10);
		element_pow_zn(y, x, SK);
		element_snprint(y_char, MSGSIZE, y);
		

		SELECT_SQL_str.clear();
		SELECT_SQL_str = "XXXXX";
		const char* SELECT_SQL_char3 = SELECT_SQL_str.data();
		if (0 != mysql_query(&ceshi, SELECT_SQL_char3)) {
			cout << "Query fail" << endl;
			system("pause");
			return 0;
		}
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);
		string SKShare_str = nextRow[X];
		
		SELECT_SQL_str.clear();
		SELECT_SQL_str = "XXXXX";
		const char* SELECT_SQL_char3 = SELECT_SQL_str.data();
		if (0 != mysql_query(&ceshi, SELECT_SQL_char3)) {
			cout << "Query fail" << endl;
			system("pause");
			return 0;
		}
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);
		string OwnerPK_str = nextRow[X];


		element_t g_r, OwnerPK, OwnerPK_r, r, to_Client, Kshare;
		element_init_G1(g_r, pairing);
		element_init_G1(OwnerPK, pairing);
		element_init_G1(OwnerPK_r, pairing);
		element_init_Zr(r, pairing);
		element_init_G1(Kshare, pairing);
		element_init_G1(to_Client, pairing);
		element_random(r);
		const char* OwnerPK_ptr = OwnerPK_str.data();
		const char* Kshare_ptr = SKShare_str.data();
		element_pow_zn(g_r, g, r);
		element_set_str(OwnerPK,OwnerPK_ptr,10);
		element_set_str(Kshar, Kshare_ptr, 10);
		element_pow_zn(OwnerPK_r,OwnerPK,r);
		element_mul(to_Client,Kshare,OwnerPK_r);



		string temp;
		element_snprint(SKFogPart1_char, MSGSIZE, to_Client);
		for (int j = 0; j < MSGSIZE; j++) {
			if ((SKFogPart1_char[j] <= '9'&&SKFogPart1_char[j] >= '0') || SKFogPart1_char[j] == ']' || SKFogPart1_char[j] == '[' || SKFogPart1_char[j] == ',' || SKFogPart1_char[j] == ' ') {
				temp += SKFogPart1_char[j];
			}
			else {
				break;
			}
		}
		SKFogPart1_char[temp.size()] = '!';

		temp.clear();
		element_snprint(SKFogPart2_char, MSGSIZE, g_r);
		for (int j = 0; j < MSGSIZE; j++) {
			if ((SKFogPart2_char[j] <= '9'&&SKFogPart2_char[j] >= '0') || SKFogPart2_char[j] == ']' || SKFogPart2_char[j] == '[' || SKFogPart2_char[j] == ',' || SKFogPart2_char[j] == ' ') {
				temp += SKFogPart2_char[j];
			}
			else {
				break;
			}
		}
		SKFogPart2_char[temp.size()] = '!';

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, SKFogPart1_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, SKFogPart2_char, MSGSIZE, NULL);
		closesocket(sClient);

		}
	}

	system("pause");
	return 0;
}


	