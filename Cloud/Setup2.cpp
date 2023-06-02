
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
#include <alibabacloud/oss/OssClient.h>
#include "openssl/aes.h"
#include "openssl/sha.h"
# include "gmp.h"
# include <stdio.h>
# include <string.h>
# include "pbc.h"

#pragma comment(lib,"ws2_32.lib")
//# include <winsock.h>
//# include <OssClientImpl.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
//#include <WinSock2.h>
#define PORT_LOC XXX
#define MSGSIZE 1024

using namespace std;
using namespace AlibabaCloud::OSS;


string binaryToHex(const string& binaryStr) {
	string ret;
	static const char *hex = "0123456789ABCDEF";
	for (auto c : binaryStr) {
		ret.push_back(hex[(c >> 4) & 0xf]);
		ret.push_back(hex[c & 0xf]);
	}
	return ret;
}





string readFileIntoString(char * filename)
{
	ifstream ifile(filename);
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	return buf.str();
}

string HexToStr(std::string str)
{
	std::string hex = str;
	long len = hex.length();
	std::string newString;
	for (long i = 0; i< len; i += 2)
	{
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
		newString.push_back(chr);
	}
	return newString;
}

string unsignedCharToHexString(unsigned char ch) {
	const char hex_chars[] = "0123456789abcdef";
	unsigned int highHalfByte = (ch >> 4) & 0x0f;
	unsigned int lowHalfByte = (ch & 0x0f);
	string result;
	result += hex_chars[highHalfByte];
	result += hex_chars[lowHalfByte];
	return result;
}


/*Paper 4 Cloud receive data owner PK*/

int main(int argc, const char* argv[]) {


	string USER_ID_str, PK_str;
	char USER_ID_char[MSGSIZE], PK_char[MSGSIZE];

	
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
		cout << "sListent ERROR" << endl;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind success" << endl;

	while (listen(sListen, 20) == -1) {
	} 
	
	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);

	recv(sClient, USER_ID_char, MSGSIZE, NULL);
	recv(sClient, PK_char, MSGSIZE, NULL);
	for (int i = 0; i < MSGSIZE; i++) {
		if (USER_ID_char[i] == '!') {
			break;
		}
		USER_ID_str += USER_ID_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (PK_char[i] == '!') {
			break;
		}
		PK_str += PK_char[i];
	}

	string DELETE_SQL_str = "DELETE FROM Owner_PK WHERE Owner = '";
	DELETE_SQL_str += USER_ID_str;
	DELETE_SQL_str += "';";
	const char* DELETE_SQL_char = DELETE_SQL_str.data();

	string INSERT_SQL_str = "INSERT INTO Owner_PK VALUES('" + USER_ID_str + "','" + PK_str + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "test", 3306, NULL, 0)) {
		cout << "Connected MYSQL!!! " << endl;
	}
	mysql_query(&ceshi, DELETE_SQL_char);
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL insert fail: %s\n", mysql_error(&ceshi));
		system("pause");
		return 0;
	}
		
	closesocket(sClient);
	WSACleanup();

	system("pause");
	return 0;

}