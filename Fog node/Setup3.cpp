
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

#define PORT 10656
#define MSGSIZE 2048
#define PAR_LENGTH 512

int main() {


	string FOG_ID = "fog_3";
	const char*  FOG_ID_char = FOG_ID.data();
	int fog_ID = 3;
	int index = 1;
	int NoFog = 5;
	char PK_char[MSGSIZE], JPK_char[MSGSIZE];
	string JPK_str;

	MYSQL ceshi1;
	mysql_init(&ceshi1);
	MYSQL_ROW row1;
	if (mysql_real_connect(&ceshi1, "localhost", "root", "625558889", FOG_ID_char, 3306, NULL, 0)) {
		cout << "Databse link success" << endl;
	}
	string SELECT_SQL_str = "SELECT * FROM fog_parameter";
	const char* SELECT_SQL_char2 = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi1, SELECT_SQL_char2)) {
		cout << "Query fail" << endl;
		system("pause");
		return 0;
	}		
	MYSQL_RES *res1;
	MYSQL_FIELD *field1;
	MYSQL_ROW nextRow1;
	res1 = mysql_store_result(&ceshi1);
	nextRow1 = mysql_fetch_row(res1);
	string FogPK_str = nextRow1[2];
	for (int i = 0; i < FogPK_str.size(); i++) {
		PK_char[i] = FogPK_str[i];
	}
	PK_char[FogPK_str.size()] = '!';


	while (index < NoFog) {
		if (index == fog_ID) {
			continue;
		}
		MYSQL_ROW row;
		string SELECT_SQL1_str = "SELECT * FROM fog_ip_port WHERE Fog='fog_" +to_string(index) + "';";
		cout << SELECT_SQL1_str << endl;
		const char* SELECT_SQL1_char = SELECT_SQL1_str.data();
		if (0 != mysql_query(&ceshi1, SELECT_SQL1_char)) {
			cout << "Query Fail" << endl;
			system("pause");
			return 0;
		}
		MYSQL_RES *res;
		MYSQL_FIELD *field;
		MYSQL_ROW nextRow;
		res = mysql_store_result(&ceshi1);
		nextRow = mysql_fetch_row(res);
		string FogPort = nextRow[2];
		string FogIP = nextRow[1];
		const char* FogIP_ptr = FogIP.data();
		
		WSADATA wsaData;
		SOCKET sClient;
		SOCKADDR_IN server;
		int ret;
		WSAStartup(0x0202, &wsaData);
		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&server, 0, sizeof(SOCKADDR_IN));
		server.sin_family = AF_INET;
		server.sin_port = htons(atoi(FogPort.c_str()));
		server.sin_addr.s_addr = inet_addr(FogIP_ptr);

		while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
		}

		send(sClient, PK_char, MSGSIZE, NULL);
		recv(sClient, JPK_char, MSGSIZE, NULL);

		JPK_str.clear();
		for (int i = 0; i < MSGSIZE; i++) {
			if (JPK_char[i] == '!') {
				break;
			}
			JPK_str += JPK_char[i];
		}
		closesocket(sClient);

		string INSERT_SQL_str = "INSERT INTO jpk values('fog_" + to_string(index) + "','"  + JPK_str + "');";
		const char* INSERT_SQL_char1 = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi1, INSERT_SQL_char1)) {
			cout << "JPK storing fail" << endl;
			system("pause");
			return 0;
		}
		index++;
		system("pause");
	}

	
	return 0;
}