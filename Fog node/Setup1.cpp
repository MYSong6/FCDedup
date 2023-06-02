
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

#define CLOUD_SERVER_ADDRESS "192.168.10.14" 
#define PORT_CLOUD_SERVER 10248 //Cloud parameter publish program prot
#define MSGSIZE 1024
#define PAR_LENGTH 512

int main() {


	string FOG_ID = "fog_3";
	const char*  FOG_ID_char = FOG_ID.data();
	int FogNode_num = 3;
	/*！！！！！！！！！！！！！！！BEGIN！！！！！！！！！！！！！！！！！*/
	WSADATA wsaData;
	SOCKET sClient;
	SOCKADDR_IN server;
	int ret;
	WSAStartup(0x0202, &wsaData);
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server, 0, sizeof(SOCKADDR_IN));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_CLOUD_SERVER);
	server.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);

	while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
	}
	

	char N_char[MSGSIZE],g_char[MSGSIZE],PK_Cloud_char[MSGSIZE];
	ret = recv(sClient, N_char, MSGSIZE, 0);
	ret = recv(sClient, g_char, MSGSIZE, 0);
	ret = recv(sClient, PK_Cloud_char, MSGSIZE, 0);

	string N_str, g_str, PK_Cloud_str;
	for (int i = 0; i < MSGSIZE; i++) {
		if (N_char[i] == '!') {
			break;
		}
		N_str += N_char[i];
	}
	for (int i = 0; i <  MSGSIZE; i++) {
		if (g_char[i] == '!') {
			break;
		}
		g_str += g_char[i];
	}
	for (int i = 0; i <  MSGSIZE; i++) {
		if (PK_Cloud_char[i] == '!') {
			break;
		}
		PK_Cloud_str += PK_Cloud_char[i];
	}
	
	/*！！！！！！！！！！！！！！！END！！！！！！！！！！！！！！！！！*/
	


	/*！！！！！！！！！！！！！！！BEGIN！！！！！！！！！！！！！！！！！*/
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
	if (nextRow == NULL) {
		string INSERT_SQL_str = "INSERT INTO cloud_parameter values('" + N_str+"','" + g_str + "','" + PK_Cloud_str + "');";
		const char* INSERT_SQL_char = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
			cout << "堝歌方峨秘払移" << endl;
			system("pause");
			return 0;
		}
	}

	mpz_t N;
	mpz_init(N);
	const char* ptr_N = N_str.data();
	mpz_init_set_str(N,ptr_N,10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param,N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	SELECT_SQL_str.clear();
	SELECT_SQL_str += "SELECT * FROM fog_parameter";
	const char* SELECT_SQL_char1 = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char1)) {
		cout << "Query fail" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	if (nextRow == NULL) {   
		/*！！！！！！BEGIN！！！！！！！*/
		mpz_t sk_t, in_sk_t;
		mpz_init(sk_t);
		mpz_init(in_sk_t);
		pbc_mpz_random(sk_t, N);
		mpz_invert(in_sk_t, sk_t, N);

		element_t g, sk, pk, in_sk;
		element_init_G1(g, pairing);
		element_init_G1(pk, pairing);
		element_init_Zr(sk, pairing);
		element_init_Zr(in_sk, pairing);
		const char* ptr_g = g_str.data();
		element_set_str(g, ptr_g, 10);
		element_set_mpz(sk, sk_t);
		element_set_mpz(in_sk, in_sk_t);
		element_pow_zn(pk, g, in_sk);

		string sk_str = mpz_get_str(NULL, 10, sk_t);
		string Isk_str = mpz_get_str(NULL, 10, in_sk_t);
		string pk_str;
		char pk_char[MSGSIZE];
		element_snprint(pk_char, 2 * PAR_LENGTH, pk);
		for (int i = 0; i < 2 * PAR_LENGTH; i++) {
			if ((pk_char[i] <= '9'&&pk_char[i] >= '0') || pk_char[i] == ']' || pk_char[i] == '[' || pk_char[i] == ',' || pk_char[i] == ' ') {
				pk_str += pk_char[i];
			}
			else {
				break;
			}
		}
	
		string INSERT_SQL_str = "INSERT INTO fog_parameter values('" + sk_str + "','"+ Isk_str+"','" + pk_str + "');";
		const char* INSERT_SQL_char1 = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERT_SQL_char1)) {
			cout << "Fog paarameter storing fail" << endl;
			system("pause");
			return 0;
		}
	
	}
	

	system("pause");
	return 0;
}