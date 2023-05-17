
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
#define PORT_LOC 16666
#define PORT_LOC1 13333
#define MSGSIZE 2048

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
	//将文件读入到ostringstream对象buf中
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串
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


	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "test", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string SELECT_SQL_str = "SELECT * FROM cloud_parameters;";
	const char*  SELECT_SQL_char = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char)) {
		printf("SQL语句(查询)执行失败: %s\n", mysql_error(&ceshi));
		system("pause");
		return 0;
	}
	MYSQL_RES *res;
	MYSQL_ROW nextRow;
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);

	string N_str = nextRow[0];
	string g_str = nextRow[1];
	string PK_str = nextRow[2];
	string p_str = nextRow[3];
	string q_str = nextRow[4];

	mpz_t N;
	mpz_init(N);
	const char* N_ptr = N_str.data();
	mpz_init_set_str(N, N_ptr, 10);
	pbc_param_t param;

	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	const char* g_ptr = g_str.data();
	const char* PK_ptr = PK_str.data();
	const char* p_ptr = p_str.data();
	const char* q_ptr = q_str.data();

	element_t g, PK, p, q;
	element_init_G1(g, pairing);
	element_init_G1(PK, pairing);
	element_init_Zr(p, pairing);
	element_init_Zr(q, pairing);
	element_set_str(g, g_ptr, 10);
	element_set_str(PK, PK_ptr, 10);
	element_set_str(p, p_ptr, 10);
	element_set_str(q, q_ptr, 10);

	/*—————————  BEGIN  ——————————*/
	/*—————————TCP初始化——————————*/
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient1;
	SOCKET sClient2;
	SOCKADDR_IN local;//服务器地址
	SOCKADDR client_user; //客户端地址
	memset(&local, 0, sizeof(local));

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent是错的" << endl;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// 任何一个客户端IP都可以连接

	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;

	while (1) {

		string LeftTau_str, RightTau_str;
		char LeftTau_char[MSGSIZE], RightTau_char[MSGSIZE],Res_char[MSGSIZE];

		listen(sListen, 2);
		cout << "Listen结束" << endl;
		cout << "等待client的连接请求" << endl;
		/*—————————   END   ——————————*/
		/*—————————TCP初始化——————————*/
		int nsize = sizeof(SOCKADDR);
		sClient1 = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient1, LeftTau_char, MSGSIZE, NULL);

		cout << "1111" << endl;

		sClient2 = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient2, RightTau_char, MSGSIZE, NULL);
		
		cout << "2222" << endl;

		for (int i = 0; i < MSGSIZE; i++) {
			if (LeftTau_char[i] == '!') {
				break;
			}
			LeftTau_str += LeftTau_char[i];
		}
		for (int i = 0; i < MSGSIZE; i++) {
			if (RightTau_char[i] == '!') {
				break;
			}
			RightTau_str += RightTau_char[i];
		}

		cout << RightTau_str << endl;
		cout << LeftTau_str << endl;
		const char* LeftTau_ptr = LeftTau_str.data();
		const char* RightTau_ptr = RightTau_str.data();

		element_t RightTau, LeftTau;
		element_init_GT(RightTau, pairing);
		element_init_GT(LeftTau, pairing);
		element_set_str(LeftTau, LeftTau_ptr, 10);
		element_set_str(RightTau, RightTau_ptr, 10);
		element_pow_zn(LeftTau, LeftTau, p);
		element_pow_zn(RightTau, RightTau, p);
		string Res;
		if (!element_cmp(LeftTau, RightTau)) {
			Res += "Dup!";
		}
		else {
			Res += "NoDup!";
		}
		for (int i = 0; i < Res.size(); i++) {
			Res_char[i] = Res[i];
		}
		cout << Res << endl;


		send(sClient1, Res_char, MSGSIZE, NULL);

		send(sClient2, Res_char, MSGSIZE, NULL);
		closesocket(sClient2);
		closesocket(sClient1);
		

	}
	WSACleanup();
		

	system("pause");
	return 0;

}