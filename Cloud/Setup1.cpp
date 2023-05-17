
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
#define MSGSIZE XXX

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
	//���ļ����뵽ostringstream����buf��
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	//������������buf�������ַ���
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


/*Paper 4 Cloud Setup*/
/*��ʼ����������������*/

int main(int argc, const char* argv[]) {

	/*����bit����*/
	const int parame_length = 512;
	/*����bit����*/

	string N_str, g_str, p_str, PK_str, q_str;
	char N_char[MSGSIZE], g_char[MSGSIZE], PK_char[MSGSIZE];

	MYSQL ceshi;
	mysql_init(&ceshi);
	//MYSQL_RES *result;
	MYSQL_ROW row;

	//mysql_real_connect(&ceshi, "localhost", "root", "625558889", "cloud_db", 3306, NULL, 0);

	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "test", 3306, NULL, 0)) {
		cout << "�ɹ�����MYSQL!!! " << endl;
	}
	string select_sql = "SELECT * FROM cloud_parameters;";
	const char* select_sql_char = select_sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char))
	{
		printf("��ѯʧ��: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //�ر�����
		system("pause");
		return 0;
	}

	MYSQL_RES *res;                                         //SQL���ִ�н����
	MYSQL_FIELD *field;                                     //�����ֶ���Ϣ�Ľṹָ��
	MYSQL_ROW nextRow;                                      //��Ų�ѯsql����ַ�������
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	if (nextRow == NULL) //�������ò���
	{	
		clock_t time = clock();
		gmp_randstate_t grt;
		gmp_randinit_default(grt);
		gmp_randseed_ui(grt, time);
		//Generate large prime p
		mpz_t p;
		mpz_init(p);
		mpz_urandomb(p, grt, parame_length);
		mpz_nextprime(p, p);
		//Generate large prime q
		mpz_t q;
		mpz_init(q);
		mpz_urandomb(q, grt, parame_length);
		mpz_nextprime(q, q);
		//Compute the order of G2  (n)
		mpz_t N;
		mpz_init(N);
		mpz_mul(N, p, q);

		pbc_param_t param;
		pbc_param_init_a1_gen(param, N);
		pairing_t pairing;
		pairing_init_pbc_param(pairing, param);

		element_t g,PK; 
		element_init_G1(g, pairing);
		element_init_G1(PK, pairing);
		element_random(g);
		element_pow_mpz(PK, g, q);
		N_str = mpz_get_str(NULL, 10, N);
		p_str = mpz_get_str(NULL,10,p);
		q_str = mpz_get_str(NULL, 10, q);
		//char g_char[2 * parame_length], PK_char[2 * parame_length];
		element_snprint(g_char, 2 * parame_length, g);
		element_snprint(PK_char, 2 * parame_length, PK);
		for (int i = 0; i < 2 * parame_length; i++) {
			if ((g_char[i] <= '9'&&g_char[i] >= '0') || g_char[i] == ']' || g_char[i] == '[' || g_char[i] == ',' || g_char[i] == ' ') {
				g_str += g_char[i];
			}
			else {
				break;
			}
		}
		for (int i = 0; i < 2 * parame_length; i++) {
			if ((PK_char[i] <= '9'&&PK_char[i] >= '0') || PK_char[i] == ']' || PK_char[i] == '[' || PK_char[i] == ',' || PK_char[i] == ' ') {
				PK_str += PK_char[i];
			}
			else {
				break;
			}
		}
		//cout << N_str.size() << endl;
		//cout << g_str.size() << endl;
		//cout << PK_str.size() << endl;
		for (int i = 0; i <N_str.size(); i++) {
			N_char[i] = N_str[i];
			
		}
		//cout << N_str << endl;
		//cout << g_str << endl;
		//cout << PK_str << endl;
		string INSERT_SQL_str = "INSERT INTO cloud_parameters values('" + N_str + "','" + g_str + "','" + PK_str + "','" + p_str + "','" + q_str + "');";
		const char* INSERT_SQL_char = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
			printf("SQL���(����)ִ��ʧ��: %s\n", mysql_error(&ceshi));
			system("pause");
			return 0;
		}

		//element_printf("g: %B\n", g);
		//cout << g_str << endl;
		//element_printf("PK: %B\n", PK);
		//cout << PK_str << endl;
		//gmp_printf("N=%Zd\n", N);
		//cout << N_str << endl;
		//gmp_printf("N=%Zd\n", N);
		//cout << p_str << endl;
	}
	else {//�Ѿ����ò�������ȡ���ȴ���ڵ��ȡ
		N_str = nextRow[0];
		g_str = nextRow[1];
		PK_str = nextRow[2];
		p_str = nextRow[3];
		q_str = nextRow[4];
	}

	int index = 0;
	for (int i = 0; i < N_str.size(); i++) {
		N_char[index] = N_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < g_str.size(); i++) {
		g_char[index] = g_str[i];
		PK_char[index] = PK_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < PK_str.size(); i++) {
		PK_char[index] = PK_str[i];
		index++;
	}
	/*�������������������������������� BEGIN ����������������������������������������*/
	/*��������������������������TCPͨ�Ž��������ݡ���������������������������������*/

	/*������������������  BEGIN  ��������������������*/
	/*������������������TCP��ʼ����������������������*/
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;//��������ַ
	SOCKADDR client_user; //�ͻ��˵�ַ
	memset(&local, 0, sizeof(local));

	char szMessage[MSGSIZE];
	int ret;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent�Ǵ��" << endl;
	}
	
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// �κ�һ���ͻ���IP����������

	while(::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR))==SOCKET_ERROR) {
	}
	cout << "Bind�ɹ�" << endl;

	while (listen(sListen, 20) == -1) {
	} //���һ���ͻ�������
	cout << "Listen����" << endl;
	cout << "�ȴ�client����������" << endl;
	/*������������������   END   ��������������������*/
	/*������������������TCP��ʼ����������������������*/
	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);

	cout << N_str << endl;
	cout << g_str << endl;
	cout << PK_str << endl;
	for (int i = 0; i <N_str.size(); i++) {
		cout << N_char[i];
	}
	cout << endl;
	for (int i = 0; i < g_str.size(); i++) {
		cout << g_char[i];
	}
	cout << endl;
	for (int i = 0; i < PK_str.size(); i++) {
		cout << PK_char[i];
	}
	cout << endl;
	N_char[N_str.size()] = '!';
	g_char[g_str.size()] = '!';
	PK_char[PK_str.size()] = '!';



	send(sClient, N_char, MSGSIZE, NULL);
	send(sClient, g_char, MSGSIZE, NULL);
	send(sClient, PK_char, MSGSIZE, NULL);

		
	closesocket(sClient);
	WSACleanup();

	system("pause");
	return 0;

}