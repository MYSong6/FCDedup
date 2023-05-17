
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
#define PORT_LOC XX
#define PORT_LOC1 XXX
#define MSGSIZE XX

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

	if (mysql_real_connect(&ceshi, "XX", "XX", "XX", "XX", XX, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string SELECT_SQL_str = "XXXX";
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
	SOCKET sClient;
	SOCKADDR_IN local;//服务器地址
	SOCKADDR client_user; //客户端地址
	memset(&local, 0, sizeof(local));

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent是错的" << endl;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC1);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// 任何一个客户端IP都可以连接

	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;
	int nsize = sizeof(SOCKADDR);

	char FILENAME_char[MSGSIZE],EndFlag_char[MSGSIZE];
	string FILENAME_str, EndFlag_str;

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, FILENAME_char, MSGSIZE, NULL);
	closesocket(sClient);

	for (int i = 0; i < MSGSIZE; i++) {
		if (FILENAME_char[i] == '!') {
			break;
		}
		FILENAME_str += FILENAME_char[i];
	}
	string OwnerID_str;
	for (int i = 0; i < FILENAME_str.size(); i++) {
		if (FILENAME_str[i] == '_') {
			break;
		}
		OwnerID_str += FILENAME_str[i];
	}

	std::string AccessKeyId = "XX";
	std::string AccessKeySecret = "XX";
	std::string Endpoint = "XX";
	std::string BucketName = "XX";
	std::string ObjectName1 = "XX";
	std::string ObjectName2 = "XX" ;
	cout << FILENAME_str << endl;

	InitializeSdk();

	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	DownloadObjectRequest request1(BucketName, ObjectName1, "XX");
	DownloadObjectRequest request2(BucketName, ObjectName2, "");

	auto outcome1 = client.ResumableDownloadObject(request1);
	auto outcome2 = client.ResumableDownloadObject(request2);


	if (outcome1.isSuccess()) {
		std::cout << "文件下载成功" << outcome1.result().Metadata().ContentLength() << std::endl;
	}
	else {
		/* 异常处理。*/
		std::cout << "Get cloud public parameter fail" <<
			",code:" << outcome1.error().Code() <<
			",message:" << outcome1.error().Message() <<
			",requestId:" << outcome1.error().RequestId() << std::endl;
		system("pause");
		ShutdownSdk();
		return -1;
	}
	if (outcome2.isSuccess()) {
		std::cout << "文件下载成功" << outcome2.result().Metadata().ContentLength() << std::endl;
	}
	else {
		/* 异常处理。*/
		std::cout << "Get cloud public parameter fail" <<
			",code:" << outcome2.error().Code() <<
			",message:" << outcome2.error().Message() <<
			",requestId:" << outcome2.error().RequestId() << std::endl;
		system("pause");
		ShutdownSdk();
		return -1;
	}

	EndFlag_char[0] = 'E';
	EndFlag_char[1] = 'n';
	EndFlag_char[2] = 'd';
	EndFlag_char[3] = '!';

	string metadate_path = "XXX";
	string Vmetadate_path = "XXX";

	fstream read_data1, read_data2, read_data3;
	read_data1.open(metadate_path);
	read_data2.open(Vmetadate_path);

	string BlockOrd_str;
	string vmdL_str, vmdR_str;
	while (getline(read_data1, BlockOrd_str)) {
		cout << BlockOrd_str << endl;
		getline(read_data2, vmdL_str);
		getline(read_data2, vmdR_str);
		SELECT_SQL_str.clear();
		SELECT_SQL_str = "XXXX";
		const char*  SELECT_SQL_char1 = SELECT_SQL_str.data();
		if (0 != mysql_query(&ceshi, SELECT_SQL_char1)) {
			printf("SQL语句(查询)执行失败: %s\n", mysql_error(&ceshi));
			system("pause");
			return 0;
		}
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);
		string SKFogPart1 = nextRow[2];
		string SKFogPart2 = nextRow[3];
		char vmdL_char[MSGSIZE], vmdR_char[MSGSIZE], SKFogPart1_char[MSGSIZE], SKFogPart2_char[MSGSIZE];
		for (int i = 0; i < vmdL_str.size(); i++) {
			vmdL_char[i] = vmdL_str[i];
		}
		vmdL_char[vmdL_str.size()] = '!';

		for (int i = 0; i < vmdR_str.size(); i++) {
			vmdR_char[i] = vmdR_str[i];
		}
		vmdR_char[vmdR_str.size()] = '!';

		for (int i = 0; i < SKFogPart1.size(); i++) {
			SKFogPart1_char[i] = SKFogPart1[i];
		}
		SKFogPart1_char[SKFogPart1.size()] = '!';


		for (int i = 0; i < SKFogPart2.size(); i++) {
			SKFogPart2_char[i] = SKFogPart2[i];
		}
		SKFogPart2_char[SKFogPart2.size()] = '!';

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, vmdL_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, vmdR_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, SKFogPart1_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, SKFogPart2_char, MSGSIZE, NULL);
		closesocket(sClient);

		SELECT_SQL_str.clear();
		SELECT_SQL_str = "XXX";
		const char*  SELECT_SQL_char2 = SELECT_SQL_str.data();
		if (0 != mysql_query(&ceshi, SELECT_SQL_char2)) {
			printf("SQL语句(查询)执行失败: %s\n", mysql_error(&ceshi));
			system("pause");
			return 0;
		}
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);
		string SKCloudPart_str = nextRow[0];
		char SKCloudPart_char[MSGSIZE];
		for (int i = 0; i < SKCloudPart_str.size(); i++) {
			SKCloudPart_char[i] = SKCloudPart_str[i];
		}
		SKCloudPart_char[SKCloudPart_str.size()] = '!';

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, SKCloudPart_char, MSGSIZE, NULL);
		closesocket(sClient);


		std::string ObjectName3 = "XXX";
		DownloadObjectRequest request3(BucketName, ObjectName3, "XXXX");
		auto outcome3 = client.ResumableDownloadObject(request3);
		if (outcome3.isSuccess()) {
			std::cout << "文件下载成功" << outcome3.result().Metadata().ContentLength() << std::endl;
		}
		else {
			/* 异常处理。*/
			std::cout << "Get cloud public parameter fail" <<
				",code:" << outcome3.error().Code() <<
				",message:" << outcome3.error().Message() <<
				",requestId:" << outcome3.error().RequestId() << std::endl;
			ShutdownSdk();
			return -1;
		}

		string ciphertext_path = "XXX";
		fstream read_cipher;
		read_cipher.open(ciphertext_path);
		char ch;
		int now_charNum = 0;
		char Cipher_char[MSGSIZE];
		while (read_cipher.get(ch)) {
			now_charNum++;
			Cipher_char[now_charNum - 1] = ch;
			if (now_charNum == MSGSIZE) {
				now_charNum = 0;

				listen(sListen, 1);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				send(sClient, Cipher_char, MSGSIZE, NULL);
				closesocket(sClient);
			}
		}
		if (now_charNum != 0) {
			Cipher_char[now_charNum] = '!';
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			send(sClient, Cipher_char, MSGSIZE, NULL);
			closesocket(sClient);
		}
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, EndFlag_char, MSGSIZE, NULL);
		closesocket(sClient);

	}

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, EndFlag_char, MSGSIZE, NULL);
	closesocket(sClient);


	WSACleanup();
		

	system("pause");
	return 0;

}
