
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

#define PORT 10655
#define MSGSIZE 2048
#define PAR_LENGTH 512

int main() {


	string FOG_ID = "fog_2";
	const char*  FOG_ID_char = FOG_ID.data();
	int Sensor_Num;
	string Sensor_RT_str, Owner_ID_str, Sensor_Num_str,OwnerPK_str;
	char Sensor_RT_char[MSGSIZE], Owner_ID_char[MSGSIZE], Sensor_Num_char[MSGSIZE], OwnerPK_char[MSGSIZE];

	/*————————————BEGIN——————————————*/
	/*———————获取数据收集节点的注册标签————————*/
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR client_user;
	memset(&local, 0, sizeof(local));
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListen出错" << endl;
	}
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;
	while (listen(sListen, 20) == -1) {
	}
	cout << "Listen结束" << endl;
	cout << "等待Owner的连接请求" << endl;

	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);

	recv(sClient, Owner_ID_char, MSGSIZE, NULL);
	recv(sClient, Sensor_Num_char, MSGSIZE, NULL);
	recv(sClient, OwnerPK_char, MSGSIZE, NULL);

	for (int i = 0; i < MSGSIZE; i++) {
		if (Owner_ID_char[i] == '!') {
			break;
		}
		Owner_ID_str += Owner_ID_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (Sensor_Num_char[i] == '!') {
			break;
		}
		Sensor_Num_str += Sensor_Num_char[i];
	}
	Sensor_Num = atoi(Sensor_Num_str.c_str());

	for (int i = 0; i < MSGSIZE; i++) {
		if (OwnerPK_char[i] == '!') {
			break;
		}
		OwnerPK_str += OwnerPK_char[i];
	}

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;
	MYSQL_RES *res;
	MYSQL_ROW nextRow;
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", FOG_ID_char, 3306, NULL, 0)) {
		cout << "数据库链接成功" << endl;
	}
	string INSERTPK_SQL_str = "INSERT INTO owner_pk VALUES('" + Owner_ID_str + "','"  + OwnerPK_str + "');";
	const char* INSERTPK_SQL_char = INSERTPK_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERTPK_SQL_char)) {
		cout << "RT插入失败" << endl;
		system("pause");
		return 0;
	}

	for (int i = 1; i <= Sensor_Num; i++) {

		recv(sClient, Sensor_RT_char, MSGSIZE, NULL);
		for (int j = 0; j < MSGSIZE; j++) {
			if (Sensor_RT_char[j] == '!') {
				break;
			}
			Sensor_RT_str += Sensor_RT_char[j];
		}
		string Sensor_ord = to_string(i);
		string INSERT_SQL_str = "INSERT INTO register_ticket VALUES('" + Owner_ID_str + "','" + Sensor_ord + "','" + Sensor_RT_str + "');";
		const char* INSERT_SQL_char = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
			cout << "RT插入失败" << endl;
			system("pause");
			return 0;
		}
		INSERT_SQL_str.clear();
		INSERT_SQL_str = "INSERT INTO owner_device_num VALUES('" + Owner_ID_str + "','" + Sensor_ord + "','" + to_string(0) + "');";
		const char* INSERTODN_SQL_char = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERTODN_SQL_char)) {
			cout << "RT插入失败" << endl;
			system("pause");
			return 0;
		}
		//cout << Sensor_RT_str << endl;
		Sensor_RT_str.clear();
	}
	string SELECT_SQL_str = "SELECT * FROM fog_parameter";
	const char* SELECT_SQL_char = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char)) {
		cout << "查询失败" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	string Fog_PK_str = nextRow[2];
	char Fog_PK_char[MSGSIZE];
	for (int i = 0; i < Fog_PK_str.size(); i++) {
		Fog_PK_char[i] = Fog_PK_str[i];
	}
	Fog_PK_char[Fog_PK_str.size()] = '!';
	send(sClient, Fog_PK_char, MSGSIZE, NULL);
	cout << "接收用户公钥，数据收集设备注册证明结束" << endl;
	system("pause");
	return 0;
}