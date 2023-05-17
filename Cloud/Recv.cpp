
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


int main(int argc, const char* argv[]) {

	/*��������������������������ȡ���ʼ����������������������*/
	
	
	string N_str, g_str, p_str, PK_str,q_str;
	const int BlockSize = XXXX;

	
	std::string AccessKeyId = "XXXX";
	std::string AccessKeySecret = "XXXX";
	std::string Endpoint = "XXXXX";
	std::string BucketName = "XXXX";
	InitializeSdk();
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);


	MYSQL ceshi;
	mysql_init(&ceshi);
	//MYSQL_RES *result;
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "X", "X", "XX", "XX", X, NULL, 0)) {
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
	
	N_str = nextRow[0];
	g_str = nextRow[1];
	PK_str = nextRow[2];
	p_str = nextRow[3];
	q_str = nextRow[4];
	

	const char* ptr_N = N_str.data();
	const char* g_ptr = g_str.data();
	const char* PK_ptr = PK_str.data();
	const char* p_ptr = p_str.data();
	const char* q_ptr = q_str.data();
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, ptr_N, 10);
	
	pbc_param_t param;
	pbc_param_init_a1_gen(param,N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);


	element_t g, PK, p, q;
	element_init_G1(g,pairing);
	element_init_G1(PK, pairing);
	element_init_Zr(p, pairing);
	element_init_Zr(q, pairing);
	element_set_str(g,g_ptr,10);
	element_set_str(PK, PK_ptr, 10);
	element_set_str(p, p_ptr, 10);
	element_set_str(q, q_ptr, 10);


	/*�������������������������������� BEGIN ����������������������������������������*/
	/*��������������������������TCPͨ�Ž��������ݡ���������������������������������*/
	char FileOrd_char[MSGSIZE],Device_char[MSGSIZE],VmetadateLeft_char[MSGSIZE], VmetadateRight_char[MSGSIZE],Owner_char[MSGSIZE],BlockOrd_char[MSGSIZE],BlockNum_char[MSGSIZE], Fog_char[MSGSIZE], sh_char[MSGSIZE], SKCloudPart_char[MSGSIZE], g_a1_char[MSGSIZE];
	string FileOrd_str,Device_str,VmetadateLeft_str,VmetadateRight_str, BlockNum_str, sh_str, SKCloudPart_str, g_a1_str, Fog_str, Owner_str;

	char Cipher_char[BlockSize * 2], CipherLen_char[MSGSIZE];
	string Ciphertext_str,CiphertextLen_str;
	/*������������������  BEGIN  ��������������������*/
	/*������������������TCP��ʼ����������������������*/
	
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;//��������ַ
	SOCKADDR client_user; //�ͻ��˵�ַ
	memset(&local, 0, sizeof(local));

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
	


	listen(sListen, 1);
	cout << "Listen����" << endl;
	cout << "�ȴ�client����������" << endl;
	/*������������������   END   ��������������������*/
	/*������������������TCP��ʼ����������������������*/
	int nsize = sizeof(SOCKADDR);

	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	
	recv(sClient, Fog_char,MSGSIZE,NULL);
	
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	
	recv(sClient, BlockNum_char, MSGSIZE, NULL);
	
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	
	recv(sClient, Owner_char, MSGSIZE, NULL);
	
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	
	recv(sClient, Device_char, MSGSIZE, NULL);
	
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	
	recv(sClient, FileOrd_char, MSGSIZE, NULL);
	
	closesocket(sClient);

	for (int i = 0; i < MSGSIZE; i++) {
		if (BlockNum_char[i] == '!') {
			break;
		}
		BlockNum_str += BlockNum_char[i];
	}
	int BlockNum = atoi(BlockNum_str.c_str());
	for (int i = 0; i < MSGSIZE; i++) {
		if( Fog_char[i] == '!') {
			break;
		}
		Fog_str +=Fog_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (Owner_char[i] == '!') {
			break;
		}
		Owner_str += Owner_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (Device_char[i] == '!') {
			break;
		}
		Device_str += Device_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (FileOrd_char[i] == '!') {
			break;
		}
		FileOrd_str += FileOrd_char[i];
	}

	element_printf("g: %B\n", g);
	element_printf("PK: %B\n", PK);
	element_printf("p: %B\n", p);
	element_printf("q: %B\n", q);
	

	string metadate_path = "XXXX";
	string Vmetadate_path = "XXXX";

	fstream write_metadate,write_Vmetadate;
	write_metadate.open(metadate_path,ios::out);
	write_Vmetadate.open(Vmetadate_path, ios::out);


	char FLAG_char[MSGSIZE];
	string FLAG_str;
	for (int ord = 0; ord < BlockNum; ord++) {

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		
		recv(sClient, FLAG_char, MSGSIZE, NULL);
		
		closesocket(sClient);
		FLAG_str.clear();
		for (int i = 0; i < MSGSIZE; i++) {
			if (FLAG_char[i] == '!') {
				break;
			}
			FLAG_str += FLAG_char[i];
		}
		if (FLAG_str == "FogDup") {//���ھͳ������ظ�
			char Link_char[MSGSIZE], VmetadateL_char[MSGSIZE], VmetadateR_char[MSGSIZE];
			string Link_str, VmetadateL_str, VmetadateR_str;


			/*��������������������Link����֤Ԫ���ݡ���������������*/
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, Link_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateL_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateR_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			for (int i = 0; i < MSGSIZE; i++) {
				if (Link_char[i] == '!') {
					break;
				}
				Link_str += Link_char[i];
			}

			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateL_char[i] >= '0'&&VmetadateL_char[i] <= '9') || VmetadateL_char[i] == ']' || VmetadateL_char[i] == '[' || VmetadateL_char[i] == ',' || VmetadateL_char[i] == ' ') {
					VmetadateL_str += VmetadateL_char[i];
				}
				else {
					break;
				}
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateR_char[i] >= '0'&&VmetadateR_char[i] <= '9') || VmetadateR_char[i] == ']' || VmetadateR_char[i] == '[' || VmetadateR_char[i] == ',' || VmetadateR_char[i] == ' ') {
					VmetadateR_str += VmetadateR_char[i];
				}
				else {
					break;
				}
			}

			write_metadate << Link_str << endl;
			write_Vmetadate << VmetadateL_str << endl;
			write_Vmetadate << VmetadateR_str << endl;
			continue; //�����¸���Ľ���


		}
		else if (FLAG_str == "Dup") {

			char BlockOrd_char[MSGSIZE],SKFogPart1_char[MSGSIZE], SKFogPart2_char[MSGSIZE];
			string BlockOrd_str,SKFogPart1_str,SKFogPart2_str;

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, BlockOrd_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateLeft_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateRight_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, SKFogPart1_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, SKFogPart2_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			VmetadateRight_str.clear();
			VmetadateLeft_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateLeft_char[i] >= '0'&&VmetadateLeft_char[i] <= '9') || VmetadateLeft_char[i] == ']' || VmetadateLeft_char[i] == '[' || VmetadateLeft_char[i] == ',' || VmetadateLeft_char[i] == ' ') {
					VmetadateLeft_str += VmetadateLeft_char[i];
				}
				else {
					break;
				}
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateRight_char[i] >= '0'&&VmetadateRight_char[i] <= '9') || VmetadateRight_char[i] == ']' || VmetadateRight_char[i] == '[' || VmetadateRight_char[i] == ',' || VmetadateRight_char[i] == ' ') {
					VmetadateRight_str += VmetadateRight_char[i];
				}
				else {
					break;
				}
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if (BlockOrd_char[i] == '!') {
					break;
				}
				BlockOrd_str += BlockOrd_char[i];
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if (SKFogPart1_char[i] == '!') {
					break;
				}
				SKFogPart1_str += SKFogPart1_char[i];
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if (SKFogPart2_char[i] == '!') {
					break;
				}
				SKFogPart2_str += SKFogPart2_char[i];
			}

			write_metadate << BlockOrd_str << endl;

			write_Vmetadate << VmetadateLeft_str << endl;
			write_Vmetadate << VmetadateRight_str << endl;

			
			string New_select_SQL_str = "SELECT * FROM decrypt_aux WHERE Owner = '" + Owner_str + "' and Link = '" + BlockOrd_str+"';";
			const char* New_select_SQL_char = New_select_SQL_str.data();
			if (0 != mysql_query(&ceshi, New_select_SQL_char)) {
				printf("SQL���(��ѯ)ִ��ʧ��: %s\n", mysql_error(&ceshi));
			}
			res = mysql_store_result(&ceshi);
			nextRow = mysql_fetch_row(res);
			if (nextRow == NULL) {
				string DecryptAux_INSERT_str = "INSERT INTO decrypt_aux VALUES('" + Owner_str + "','" + BlockOrd_str + "','" + SKFogPart1_str + "','" + SKFogPart2_str + "');";
				const char* DecryptAux_INSERT_char = DecryptAux_INSERT_str.data();

				if (0 != mysql_query(&ceshi, DecryptAux_INSERT_char)) {
					printf("SQL���(����)ִ��ʧ��: %s\n", mysql_error(&ceshi));
				}
			}
			

		}
		else if (FLAG_str == "NoDup") {//�̹�ϣ��û����
			/*���������������ն̹�ϣ,����Կ��������������*/

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, sh_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			sh_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if (sh_char[i] == '!') {
					break;
				}
				sh_str += sh_char[i];
			}
			cout << sh_str << endl;
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, SKCloudPart_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			SKCloudPart_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if (SKCloudPart_char[i] == '!') {
					break;
				}
				SKCloudPart_str += SKCloudPart_char[i];
			}

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, g_a1_char, MSGSIZE, NULL);
			
			closesocket(sClient);
			g_a1_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if (g_a1_char[i] == '!') {
					break;
				}
				g_a1_str += g_a1_char[i];
			}
			cout << "sh:" << sh_str << endl;
			cout << "g_a1"<<g_a1_str << endl;
			cout << "SKCloudPart" << SKCloudPart_str << endl;
			//element_t g_a1, SKCloudPart;


			/*����������������Ϊ�����ݿ�ѡLink����������������*/
			
			select_sql = "SELECT * FROM link_order;";
			const char* select_sql_char = select_sql.data();
			if (0 != mysql_query(&ceshi, select_sql_char))
			{
				printf("��ѯʧ��: %s\n", mysql_error(&ceshi));
				mysql_close(&ceshi);                         //�ر�����
				system("pause");
				return 0;
			}
                                //��Ų�ѯsql����ַ�������
			res = mysql_store_result(&ceshi);
			nextRow = mysql_fetch_row(res);
			string Pre_ord = nextRow[0];
			string New_ord = to_string(atoi(Pre_ord.c_str())+1);

			string upldate_sql = "update link_order set Link_ord='" + New_ord + "';";
			const char* uplodate_sql_char = upldate_sql.data();
			if (mysql_query(&ceshi, uplodate_sql_char)) {
				cout << "�޸���һ��Link_orderʧ��:" << mysql_error(&ceshi) << endl;
			}
			else {
				cout << "������һ��Link_order�ɹ�!!" << endl;
			}
			string SKPart_str;
			
			/*�����������������ָ��ƴ洢����Կ������������������*/
			element_t SKPart, g_a1;
			element_init_G1(g_a1, pairing);
			element_init_G1(SKPart, pairing);
			const char* g_a1_ptr = g_a1_str.data();
			const char* SKPart_ptr = SKCloudPart_str.data();
			element_set_str(SKPart,SKPart_ptr,10);
			element_set_str(g_a1, g_a1_ptr, 10);
			element_pow_zn(g_a1, g_a1, q);
			element_div(SKPart,SKPart,g_a1);

			

			element_printf("SKPart: %B\n", SKPart);
			element_snprint(SKCloudPart_char,MSGSIZE,SKPart);

			SKCloudPart_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if ((SKCloudPart_char[i] >= '0'&&SKCloudPart_char[i] <= '9') || SKCloudPart_char[i] == ']' || SKCloudPart_char[i] == '[' || SKCloudPart_char[i] == ',' || SKCloudPart_char[i] == ' ') {
					SKCloudPart_str += SKCloudPart_char[i];
				}
				else {
					break;
				}
			}

			/*������������������������ݿ⡪��������*/
			
			string SQL_INSERT_str = "INSERT INTO block_index VALUES('" + sh_str + "','" + Fog_str + "','" + SKCloudPart_str + "','" + New_ord + "');";
			const char* SQL_INSERT_char = SQL_INSERT_str.data();

			if (0 != mysql_query(&ceshi, SQL_INSERT_char)) {
				printf("SQL���(����)ִ��ʧ��: %s\n", mysql_error(&ceshi));
			}
			
			for (int i = 0; i < New_ord.size(); i++) {
				BlockOrd_char[i] = New_ord[i];
			}
			BlockOrd_char[New_ord.size()] = '!';

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			send(sClient, BlockOrd_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			/*������������������֤Ԫ���ݡ�����������*/
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateLeft_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, VmetadateRight_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			VmetadateRight_str.clear();
			VmetadateLeft_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateLeft_char[i] >= '0'&&VmetadateLeft_char[i] <= '9') || VmetadateLeft_char[i] == ']' || VmetadateLeft_char[i] == '[' || VmetadateLeft_char[i] == ',' || VmetadateLeft_char[i] == ' ') {
					VmetadateLeft_str += VmetadateLeft_char[i];
				}
				else {
					break;
				}
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if ((VmetadateRight_char[i] >= '0'&&VmetadateRight_char[i] <= '9') || VmetadateRight_char[i] == ']' || VmetadateRight_char[i] == '[' || VmetadateRight_char[i] == ',' || VmetadateRight_char[i] == ' ') {
					VmetadateRight_str += VmetadateRight_char[i];
				}
				else {
					break;
				}
			}

			write_metadate << New_ord << endl;

			write_Vmetadate << VmetadateLeft_str << endl;
			write_Vmetadate << VmetadateRight_str << endl;

			/*������������������BEGIN����������������*/
			/*�����������������������Ŀ顪������������*/
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, CipherLen_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			CiphertextLen_str.clear();
			for (int i = 0; i < MSGSIZE; i++) {
				if (CipherLen_char[i] == '!') {
					break;
				}
				CiphertextLen_str += CipherLen_char[i];
			}
			int CipherLen = atoi(CiphertextLen_str.c_str());
			cout << CipherLen << endl;

			int times = ceil(double(CipherLen)/double(MSGSIZE));
			string Ciphertext_path = "XXXXX";
			fstream write_ciphertext;
			write_ciphertext.open(Ciphertext_path, ios::out);
			for (int i = 0; i < times; i++) {
				listen(sListen, 1);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recv(sClient, Cipher_char, MSGSIZE, NULL);
				
				closesocket(sClient);
				Ciphertext_str.clear();
				if (i == times - 1) {
					for (int j = 0; j < (CipherLen-i*MSGSIZE); j++) {
						Ciphertext_str += Cipher_char[j];
					}
				}
				else {
					for (int j = 0; j < MSGSIZE; j++) {
						Ciphertext_str += Cipher_char[j];
					}
				}
				write_ciphertext << Ciphertext_str;
				cout << Ciphertext_str << endl;
			}
			write_ciphertext<< endl;
			write_ciphertext.close();
			/*��������������������END����������������������*/
			/*�����������������������Ŀ顪��������������*/

			/*����������������������BEGIN������������������*/
			/*�������������������ս�����Կ��������������*/
			/*�������������������û���Կ���ܡ�������������*/

			char SKShare_FogPart_Left_char[MSGSIZE], SKShare_FogPart_Right_char[MSGSIZE];
			string SKShare_FogPart_Left_str, SKShare_FogPart_Right_str;
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, SKShare_FogPart_Left_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recv(sClient, SKShare_FogPart_Right_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			for (int i = 0; i < MSGSIZE; i++) {
				if (SKShare_FogPart_Left_char[i] == '!') {
					break;
				}
				SKShare_FogPart_Left_str += SKShare_FogPart_Left_char[i];
			}
			for (int i = 0; i < MSGSIZE; i++) {
				if (SKShare_FogPart_Right_char[i] == '!') {
					break;
				}
				SKShare_FogPart_Right_str += SKShare_FogPart_Right_char[i];
			}
			
			string DecryptAux_INSERT_str = "INSERT INTO decrypt_aux VALUES('" + Owner_str + "','" + New_ord+ "','" + SKShare_FogPart_Left_str + "','" + SKShare_FogPart_Right_str + "');";
			const char* DecryptAux_INSERT_char = DecryptAux_INSERT_str.data();

			if (0 != mysql_query(&ceshi, DecryptAux_INSERT_char)) {
				printf("SQL���(����)ִ��ʧ��: %s\n", mysql_error(&ceshi));
			}
			
			/*����������������������END������������������*/
			/*�������������������ս�����Կ��������������*/
			/*�������������������û���Կ���ܡ�������������*/

			string ObjectName3 = "Ciphertext/" + New_ord + ".txt";
			

			
			std::shared_ptr<std::iostream> content3 = std::make_shared<std::fstream>(Ciphertext_path, std::ios::in | std::ios::binary);
			PutObjectRequest request3(BucketName, ObjectName3, content3);
			auto outcome3 = client.PutObject(request3);

			if (!outcome3.isSuccess()) {
				std::cout << "Semantically secure ciphertext upload fail" <<
					",code:" << outcome3.error().Code() <<
					",message:" << outcome3.error().Message() <<
					",requestId:" << outcome3.error().RequestId() << std::endl;

				system("pause");
			}
			

		}
		

	}
	write_metadate.close();
	write_Vmetadate.close();
	//closesocket(sClient);
	WSACleanup();


	string ObjectName1 = "XXXXXX";
	string ObjectName2 = "XXXXXX";
	
	
	std::shared_ptr<std::iostream> content1 = std::make_shared<std::fstream>(metadate_path, std::ios::in | std::ios::binary);
	std::shared_ptr<std::iostream> content2 = std::make_shared<std::fstream>(Vmetadate_path, std::ios::in | std::ios::binary);
	PutObjectRequest request1(BucketName, ObjectName1, content1);
	auto outcome1 = client.PutObject(request1);

	if (!outcome1.isSuccess()) {
		std::cout << "Semantically secure ciphertext upload fail" <<
			",code:" << outcome1.error().Code() <<
			",message:" << outcome1.error().Message() <<
			",requestId:" << outcome1.error().RequestId() << std::endl;

		system("pause");
	}
	PutObjectRequest request2(BucketName, ObjectName2, content2);
	auto outcome2 = client.PutObject(request2);

	if (!outcome2.isSuccess()) {
		std::cout << "Semantically secure ciphertext upload fail" <<
			",code:" << outcome2.error().Code() <<
			",message:" << outcome2.error().Message() <<
			",requestId:" << outcome2.error().RequestId() << std::endl;

		system("pause");
	}
	
	
	ShutdownSdk();

	system("pause");
	return 0;

}