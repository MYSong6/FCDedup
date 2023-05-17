
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

#define CLOUD_SERVER_ADDRESS "X.X.X.X"
#define LOC_PORT XX
#define PORT_CLOUD_SERVER XX
#define PORT_CLOUD_SERVERDETECT XX
#define MSGSIZE XX
#define PAR_LENGTH XX
#define QUEUE_SIZE X

int main() {


	string FOG_ID = "XX";
	const char*  FOG_ID_char = FOG_ID.data();
	const char* Cloud_DB = "XX";
	const int BlockSize = XXX;

	/*——————从数据库中读取参数——————*/
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;
	if (mysql_real_connect(&ceshi, "XXX", "XXX", "XXX", FOG_ID_char, XX, NULL, 0)) {
		cout << "数据库链接成功" << endl;
	}
	string SELECT_SQL_str = "SELECT * FROM cloud_parameter";
	const char* SELECT_SQL_char1 = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char1)) {
		cout << "查询失败" << endl;
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
		cout << "查询失败" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	string SK_str = nextRow[0];
	string ISK_str = nextRow[1];
	string FogPK_str = nextRow[2];


	/*——————参数初始化——————*/
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

	//element_printf("g: %B\n",g);
	//element_printf("SK: %B\n", SK);
	//element_printf("FogPK: %B\n", FogPK);
	//element_printf("ISK: %B\n",ISK);
	//element_printf("CloudPK: %B\n", CloudPK);
	/*连接云端数据库*/
	MYSQL ceshi_cloud;
	mysql_init(&ceshi_cloud);
	MYSQL_ROW row_cloud;
	if (mysql_real_connect(&ceshi_cloud, CLOUD_SERVER_ADDRESS, "root", "625558889", Cloud_DB, 3306, NULL, 0)==NULL) {
		printf("错误原因：%s\n", mysql_error(&ceshi_cloud));
	}
	else {
		cout << "云端数据库链接成功" << endl;
	}
	MYSQL_RES *res_cloud;
	MYSQL_ROW nextRow_cloud;

	//system("pause");

	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR client_user;
	memset(&local,0,sizeof(local));
	WSAStartup(MAKEWORD(2,2),&wsaData);

	sListen = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "Listen过程出错" << endl;
	}
	local.sin_family = AF_INET;
	local.sin_port = htons(LOC_PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;
	listen(sListen, QUEUE_SIZE);
	char OwnerID_char[MSGSIZE],DeviceID_char[MSGSIZE],FileOrd_char[MSGSIZE],FileNum_char[MSGSIZE],BlockNum_char[MSGSIZE];
	string BlockOrd_str,FogTagLeft_str, FogTagRight_str, OwnerID_str, DeviceID_str, FileOrd_str, FileNum_str,BlockNum_str;
	char VmetadateLeft_char[MSGSIZE], VmetadateRight_char[MSGSIZE],Dup_char[MSGSIZE], NoDup_char[MSGSIZE],FLAG_char[MSGSIZE],FOG_CHAR[MSGSIZE],BlockOrd_char[MSGSIZE];
	char Ciphertext_char[2 * BlockSize], CiphertextLen_char[MSGSIZE];
	Dup_char[0] = 'D';
	Dup_char[1] = 'u';
	Dup_char[2] = 'p';
	Dup_char[3] = '!';
	NoDup_char[0] = 'N';
	NoDup_char[1] = 'o';
	NoDup_char[2] = 'D';
	NoDup_char[3] = 'u';
	NoDup_char[4] = 'p';
	NoDup_char[5] = '!';
	FLAG_char[0] = 'R';
	FLAG_char[1] = 'E';
	FLAG_char[2] = 'C';
	for (int i = 0; i < FOG_ID.size(); i++) {
		FOG_CHAR[i] = FOG_ID[i];
	}
	FOG_CHAR[FOG_ID.size()] = '!';

	/*与云交互初始化*/
	WSADATA wsaData1;
	SOCKET sClient_Fog;
	SOCKADDR_IN server1;
	WSAStartup(0x0202, &wsaData1);
	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server1, 0, sizeof(SOCKADDR_IN));
	server1.sin_family = AF_INET;
	server1.sin_port = htons(PORT_CLOUD_SERVER);
	server1.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);

	WSADATA wsaData3;
	SOCKET sClient_Fog3;
	SOCKADDR_IN server3;
	WSAStartup(0x0202, &wsaData3);
	sClient_Fog3 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server3, 0, sizeof(SOCKADDR_IN));
	server3.sin_family = AF_INET;
	server3.sin_port = htons(PORT_CLOUD_SERVERDETECT);
	server3.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);

	
	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen,(sockaddr*)&client_user,&nsize);
	recv(sClient, OwnerID_char, MSGSIZE, NULL);
	recv(sClient, DeviceID_char, MSGSIZE, NULL);
	recv(sClient, FileOrd_char, MSGSIZE, NULL);
	recv(sClient, BlockNum_char, MSGSIZE, NULL);
	closesocket(sClient);
	//WSACleanup();
	//recv(sClient, FileNum_char, MSGSIZE, NULL);
	for (int i = 0; i < MSGSIZE; i++) {
		if (OwnerID_char[i] == '!') {
			break;
		}
		OwnerID_str += OwnerID_char[i];
	}
	for (int i = 0; i < MSGSIZE; i++) {
		if (DeviceID_char[i] == '!') {
			break;
		}
		DeviceID_str += DeviceID_char[i];
	}
	cout << DeviceID_str << endl;
	for (int j = 0; j < MSGSIZE; j++) {
		if (FileOrd_char[j] == '!') {
			break;
		}
		FileOrd_str += FileOrd_char[j];
	}
	for (int j = 0; j < MSGSIZE; j++) {
		if (BlockNum_char[j] == '!') {
			break;
		}
		BlockNum_str += BlockNum_char[j];
	}
	cout << "FileOrd:"<<FileOrd_str << endl;
	cout << "BlockNum:" << BlockNum_str << endl;
	string Index = OwnerID_str +"-"+ DeviceID_str +"-"+ FileOrd_str;
	int BlockOrd = 1;
	cout << "Index:" << Index << endl;
	int BlockNum = atoi(BlockNum_str.c_str());
	cout << BlockNum << endl;
	
	


	while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
	}
	
	send(sClient_Fog, FOG_CHAR, MSGSIZE, NULL);
	
	closesocket(sClient_Fog);

	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
	}
	
	send(sClient_Fog, BlockNum_char, MSGSIZE, NULL);
	
	closesocket(sClient_Fog);

	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
	}
	
	send(sClient_Fog, OwnerID_char, MSGSIZE, NULL);
	
	closesocket(sClient_Fog);

	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
	}
	
	send(sClient_Fog, DeviceID_char, MSGSIZE, NULL);
	
	closesocket(sClient_Fog);

	sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
	}
	
	send(sClient_Fog, FileOrd_char, MSGSIZE, NULL);
	
	closesocket(sClient_Fog);

	/*——————读取客户端设备的注册标签————*/
	
	SELECT_SQL_str.clear();
	SELECT_SQL_str = "SELECT RT FROM register_ticket WHERE Owner='" + OwnerID_str + "' and Sensor='" + DeviceID_str + "';";
	const char* SELECT_SQL_char3 = SELECT_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECT_SQL_char3)) {
		cout << "查询失败" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	string RT_str = nextRow[0];
	const char* RT_char = RT_str.data();
	element_t RT;
	element_init_G1(RT, pairing);
	element_set_str(RT, RT_char, 10);
	
	while (BlockNum--) {
		/*——————与数据收集设备交互获取文件标签——————————*/
		//listen(sListen, QUEUE_SIZE);
		char FogTagLeft_char[MSGSIZE], FogTagRight_char[MSGSIZE];
		int recvNo1, recvNo2, sendNo;

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		
		recvNo1=recv(sClient, FogTagLeft_char, MSGSIZE, NULL);
		
		closesocket(sClient);

		listen(sListen, QUEUE_SIZE);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		
		recvNo2=recv(sClient, FogTagRight_char, MSGSIZE, NULL);
		
		closesocket(sClient);

		cout << "FogTagLeft_char" << recvNo1 << endl;
		cout << "FogTagRight_char"<<recvNo2 << endl;
		FogTagLeft_str.clear();
		for (int j = 0; j < MSGSIZE; j++) {
			if (FogTagLeft_char[j] == '!') {
				break;
			}
			FogTagLeft_str += FogTagLeft_char[j];
		}
		FogTagRight_str.clear();
		for (int j = 0; j < MSGSIZE; j++) {
			if (FogTagRight_char[j] == '!') {
				break;
			}
			FogTagRight_str += FogTagRight_char[j];
		}

				//cout << "FogTagLeft_str:" << FogTagLeft_str << endl;
				//cout << "FogTagRight_str:" << FogTagRight_str << endl;
		
		const char* FogTagRight_ptr = FogTagRight_str.data();
		const char* FogTagLeft_ptr = FogTagLeft_str.data();
		element_t FogTag,FogTagLeft,FogTagRight;
		element_init_G1(FogTag, pairing);
		element_init_G1(FogTagLeft, pairing);
		element_init_G1(FogTagRight, pairing);
		element_set_str(FogTagLeft, FogTagLeft_ptr,10);
		element_set_str(FogTagRight, FogTagRight_ptr, 10);
				//element_printf("FogTagLeft: %B\n", FogTagLeft);
				//element_printf("FogTagRight: %B\n", FogTagRight);
		element_pow_zn(FogTagLeft, FogTagLeft,ISK);
		element_mul(FogTagRight, FogTagRight, RT);
		element_div(FogTag, FogTagRight, FogTagLeft);
				//element_printf("FogTag: %B\n", FogTag);
				/*——————与数据收集设备交互获取文件标签——————————*/


		/*——————查重——————*/
		char FogTag_char[MSGSIZE];
		element_snprint(FogTag_char,MSGSIZE, FogTag);
		string FogTag_str;
		for (int j = 0; j < MSGSIZE; j++) {
			if ((FogTag_char[j] <= '9'&&FogTag_char[j] >= '0') || FogTag_char[j] == ']' || FogTag_char[j] == '[' || FogTag_char[j] == ',' || FogTag_char[j] == ' ') {
				FogTag_str += FogTag_char[j];
			}
			else {
				break;
			}
		}
				//cout << FogTag_str << endl;
		
		SELECT_SQL_str.clear();
		SELECT_SQL_str = "SELECT Link FROM index_db WHERE Owner='" + OwnerID_str + "' and FogTag='" + FogTag_str + "';";
		const char* SELECT_SQL_char4 = SELECT_SQL_str.data();
		if (0 != mysql_query(&ceshi, SELECT_SQL_char4)) {
			cout << "查询失败" << endl;
			system("pause");
			return 0;
		}
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);
		
		if (nextRow != NULL) {// 雾级就出现了重复
			/*—————向数据收集设备发送"Dup"通知———————*/
			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			sendNo = send(sClient, Dup_char, MSGSIZE, NULL);
			
			closesocket(sClient);


			/*————————接收验证元数据————————*/
			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recvNo1 = recv(sClient, VmetadateLeft_char, MSGSIZE, NULL);
			
			closesocket(sClient);

			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recvNo1 = recv(sClient, VmetadateRight_char, MSGSIZE, NULL);
			
			closesocket(sClient);
			//cout << "111" << endl;

			/*————————向云传递雾级重复信号,块Link————————*/
			string Link_str = nextRow[0];
			//cout << Link_str << endl;
			//system("pause");
			char FogDup_char[MSGSIZE];
			FogDup_char[0] = 'F';
			FogDup_char[1] = 'o';
			FogDup_char[2] = 'g';
			FogDup_char[3] = 'D';
			FogDup_char[4] = 'u';
			FogDup_char[5] = 'p';
			FogDup_char[6] = '!';
			char Link_char[MSGSIZE];
			for (int j = 0; j < Link_str.size(); j++) {
				Link_char[j] = Link_str[j];
			}
			Link_char[Link_str.size()] = '!';
			//cout << "222" << endl;

			sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
			}
			
			send(sClient_Fog, FogDup_char, MSGSIZE, NULL);
			
			closesocket(sClient_Fog);

			sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
			}
			
			send(sClient_Fog, Link_char, MSGSIZE, NULL);
			
			closesocket(sClient_Fog);


			/*————————发送验证元数据————————*/

			sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
			}
			
			send(sClient_Fog, VmetadateLeft_char, MSGSIZE, NULL);
			
			closesocket(sClient_Fog);

			sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
			}
			
			send(sClient_Fog, VmetadateRight_char, MSGSIZE, NULL);
			
			closesocket(sClient_Fog);



		}
		else{ //块在Owner内部不重复
			/*—————向数据收集设备发送"NoDup"通知———————*/
			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			sendNo=send(sClient, NoDup_char, MSGSIZE, NULL);
			
			closesocket(sClient);
			cout <<"NoDup_char"<< sendNo << endl;
			/*—————————盲签名——————————*/
			char x_char[MSGSIZE], y_char[MSGSIZE],tau_char[MSGSIZE],sh_char[MSGSIZE];
			char SKFogPart1_char[MSGSIZE], SKFogPart2_char[MSGSIZE];

			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recvNo1=recv(sClient, x_char, MSGSIZE, NULL);
			
			closesocket(sClient);
			cout<<"x_char" << recvNo1 << endl;

			string x_str, y_str,tau_str,sh_str;
			for (int j = 0; j < MSGSIZE; j++) {
				if (x_char[j] == '!') {
					break;
				}
				x_str += x_char[j];
			}
			const char* x_ptr = x_str.data();
			element_t x,y;
			element_init_G1(x, pairing);
			element_init_G1(y, pairing);
			element_set_str(x, x_ptr, 10);
			//element_printf("ISK: %B\n",ISK);
			
			element_pow_zn(y,x,SK);
			
			element_snprint(y_char, MSGSIZE, y);
			for (int j = 0; j < MSGSIZE; j++) {
				if ((y_char[j] <= '9'&&y_char[j] >= '0') || y_char[j] == ']' || y_char[j] == '[' || y_char[j] == ',' || y_char[j] == ' ') {
					y_str += y_char[j];
				}
				else {
					break;
				}
			}
			y_char[y_str.size()] = '!';


			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			sendNo=send(sClient, y_char, MSGSIZE, NULL);
			closesocket(sClient);
			cout << "y_char" << sendNo << endl;
			/*—————————云查重——————————*/

			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			recvNo1=recv(sClient, tau_char, MSGSIZE, NULL);
			closesocket(sClient);

			listen(sListen, QUEUE_SIZE);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			
			recvNo2=recv(sClient, sh_char, MSGSIZE, NULL);
			
			cout << "tau_char" << recvNo1 << endl;
			cout << "sh_char" << recvNo2 << endl;
			for (int j = 0; j < MSGSIZE; j++) {
				if (tau_char[j] == '!') {
					break;
				}
				tau_str += tau_char[j];
			}
			for (int j = 0; j < MSGSIZE; j++) {
				if (sh_char[j] == '!') {
					break;
				}
				sh_str += sh_char[j];
			}
			const char* tau_ptr = tau_str.data();
			element_t tau;
			element_init_G1(tau, pairing);
			element_set_str(tau,tau_ptr,10);
					//send(sClient, y_char, MSGSIZE, NULL);
			cout <<"sh:"<< sh_str << endl;
			//element_printf("tau: %B\n", tau);
			
			SELECT_SQL_str.clear();
			SELECT_SQL_str = "SELECT * FROM block_index WHERE sh='" + sh_str + "';";
			const char* SELECT_SQL_char5 = SELECT_SQL_str.data();
			if (0 != mysql_query(&ceshi_cloud, SELECT_SQL_char5)) {
				cout << "查询失败" << endl;
				system("pause");
				return 0;
			}
			res_cloud = mysql_store_result(&ceshi_cloud);
			nextRow_cloud = mysql_fetch_row(res_cloud);
			
			int result = 1;
			string Result_Link;
			while (nextRow_cloud != NULL) { //短哈希碰撞成功 
				Result_Link.clear();
				string Tag_str = nextRow_cloud[0];
				string Fog_str = nextRow_cloud[1];
				string Link_str = nextRow_cloud[3];
				cout << "Link"<<Link_str << endl;
				cout << "Fog" << Fog_str << endl; /*如果短哈希碰撞到的雾节点还是自己，				     */
                
				Result_Link += Link_str;


				

				string TargetFogJPK_path = "XXXXX";
				fstream readTargetFogJPK_ptr;
				readTargetFogPK_ptr.open(TargetFogJPK_path);
				getline(readTargetFogJPK_ptr, TagetFogJPK_str);
				readTargetFogPK_ptr.close();

				element_t TargetFogJPK,F_res;
				element_init_G1(TargetFogJPK, pairing);
                element_init_GT(F_res, pairing);
            
				const char* TargetFogPK_ptr = TagetFogJPK_str.data();
				element_set_str(TargetFogJPK, TargetFogJPK_ptr,10);
                pairing_apply(F_res,tau,TargetFogJPK,pairing);
				

				
				string F_res_str,RESULT_str;
				char F_res_char[MSGSIZE],RESULT_char[MSGSIZE];
				element_snprint(F_res_char, MSGSIZE, F_res);
				for (int j = 0; j < MSGSIZE; j++) {
					if ((F_res_char[j] <= '9'&&F_res_char[j] >= '0') || F_res_char[j] == ']' || F_res_char[j] == '[' || F_res_char[j] == ',' || F_res_char[j] == ' ') {
						F_res_str += F_res_char[j];
					}
					else {
						break;
					}
				}


				

				if (F_res_char== Tag_str) {

					result = 2;

					sClient_Fog2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					while (connect(sClient_Fog2, (sockaddr*)&server2, sizeof(SOCKADDR)) == -1) {
					}
					
					send(sClient_Fog2, OwnerID_char, MSGSIZE, NULL);
					
					closesocket(sClient_Fog2);

					sClient_Fog2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					while (connect(sClient_Fog2, (sockaddr*)&server2, sizeof(SOCKADDR)) == -1) {
					}
					
					recv(sClient_Fog2, SKFogPart1_char, MSGSIZE, NULL);
					
					closesocket(sClient_Fog2);

					sClient_Fog2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					while (connect(sClient_Fog2, (sockaddr*)&server2, sizeof(SOCKADDR)) == -1) {
					}
					
					recv(sClient_Fog2, SKFogPart2_char, MSGSIZE, NULL);
					
					
					closesocket(sClient_Fog2);

					break;
				}
				//system("pause");

			}
			closesocket(sClient);
			if (result == 2) { //云端重复存在
				/*——————通知设备端——————*/
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				sendNo = send(sClient, Dup_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				/*——————通知云端——————*/
				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, Dup_char, MSGSIZE, NULL);
				
				
				closesocket(sClient_Fog);

				char Link_char[MSGSIZE];
				for (int j = 0; j < Result_Link.size(); j++) {
					Link_char[j] = Result_Link[j];
				}
				Link_char[Result_Link.size()] = '!';

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, Link_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				/*————————    BEGIN		————————*/
				/*————————传递验证元数据————————*/
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, VmetadateLeft_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, VmetadateRight_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, VmetadateLeft_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, VmetadateRight_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				/*—————————END————————*/
				/*——————传递验证元数据——————*/

				/*——————传递解密共享———————*/
				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, SKFogPart1_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, SKFogPart2_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);


			}
			else if (result == 1) {//云端查重不存在
				/*——————通知设备端——————*/
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				sendNo = send(sClient, NoDup_char, MSGSIZE, NULL);
				
				closesocket(sClient);


				char SKToCloud_char[MSGSIZE], SKToFog_char[MSGSIZE],g_a1_char[MSGSIZE],g_a2_char[MSGSIZE];
				string SKToCloud_str, SKToFog_str,g_a1_str,g_a2_str;
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, SKToCloud_char, MSGSIZE, NULL);
				
				closesocket(sClient);
				//cout << "SKToCloud：" << recvNo1 << endl;

				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, g_a1_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo2 = recv(sClient, SKToFog_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo2 = recv(sClient, g_a2_char, MSGSIZE, NULL);
				
				closesocket(sClient);
	
				for (int j = 0; j < MSGSIZE; j++) {
					if (SKToFog_char[j] == '!') {
						break;
					}
					SKToFog_str += SKToFog_char[j];
				}
				for (int j = 0; j < MSGSIZE; j++) {
					if (g_a2_char[j] == '!') {
						break;
					}
					g_a2_str += g_a2_char[j];
				}
				//cout << "SKToCloud:" << SKToCloud_str << endl;
				//cout << "g_a1:" << g_a1_str << endl;
				//cout << "SKToFog:" << SKToFog_str << endl;
				//cout << "g_a2:" << g_a2_str << endl;

				
				const char* SKToFog_ptr = SKToFog_str.data();
				const char* g_a2_ptr = g_a2_str.data();
				element_t SKToFog, g_a2,g_a2_ISK;
				element_init_G1(SKToFog, pairing);
				element_init_G1(g_a2, pairing);
				element_init_G1(g_a2_ISK, pairing);
				element_set_str(SKToFog,SKToFog_ptr,10);
				element_set_str(g_a2, g_a2_ptr, 10);
				element_pow_zn(g_a2_ISK, g_a2, ISK);
				element_div(SKToFog, SKToFog, g_a2_ISK);
				
				SKToFog_str.clear();
				element_snprint(SKToFog_char,MSGSIZE, SKToFog);
				for (int j = 0; j < MSGSIZE; j++) {
					if ((SKToFog_char[j] <= '9'&&SKToFog_char[j] >= '0') || SKToFog_char[j] == ']' || SKToFog_char[j] == '[' || SKToFog_char[j] == ',' || SKToFog_char[j] == ' ') {
						SKToFog_str += SKToFog_char[j];
					}
					else {
						break;
					}
				}


				//element_div(SKToFog, SKToFog, g);
				//element_printf("g_a2: %B\n", g_a2);
				element_printf("SKToFog: %B\n", SKToFog);

				/*——————————BEGIN—————————*/
				/*——————通知云端数据不重复——————*/
				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog,NoDup_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, sh_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);


				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, SKToCloud_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, g_a1_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);
				/*——————————END—————————*/
				/*——————通知云端数据不重复——————*/


				/*——————————BEGIN————————————*/
				/*————————接收云生成的块ord————————*/
				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				recv(sClient_Fog, BlockOrd_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				BlockOrd_str.clear();
				for (int j = 0; j < MSGSIZE; j++) {
					if (BlockOrd_char[j] == '!') {
						break;
					}
					BlockOrd_str += BlockOrd_char[j];
				}
				/*———————————END————————————*/
				/*————————接收云生成的块ord————————*/



				/*————————————BEGIN————————————*/
				/*————————新块索引存入本地数据库————————*/
				
	
				
				/*————————————END——————————————*/
				/*————————新块索引存入本地数据库————————*/

				/*————————    BEGIN		————————*/
				/*————————传递验证元数据————————*/
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, VmetadateLeft_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, VmetadateRight_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, VmetadateLeft_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, VmetadateRight_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				/*—————————END————————*/
				/*——————传递验证元数据——————*/

				
				/*————————BEGIN———————*/
				/*———————传递密文——————*/
				listen(sListen, QUEUE_SIZE);
				sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
				
				recvNo1 = recv(sClient, CiphertextLen_char, MSGSIZE, NULL);
				
				closesocket(sClient);

				string CipherLen_str;
				for (int j = 0; j < MSGSIZE; j++) {
					if (CiphertextLen_char[j] == '!') {
						break;
					}
					CipherLen_str += CiphertextLen_char[j];
				}
				int CipherLen = atoi(CipherLen_str.c_str());
				int times = ceil(double(CipherLen)/double(MSGSIZE));

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, CiphertextLen_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				for (int j = 0; j < times; j++) {
					listen(sListen, QUEUE_SIZE);
					sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
					
					recvNo1 = recv(sClient, Ciphertext_char, MSGSIZE, NULL);
					
					closesocket(sClient);


					sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
					}
					
					send(sClient_Fog, Ciphertext_char,MSGSIZE, NULL);
					
					closesocket(sClient_Fog);
				}
				/*————————END———————*/
				/*———————传递密文——————*/

				/*——————————BEGIN—————————*/
				/*———————传递解密辅助数据——————*/
				
				string SELECTOwnerPK_SQL_str = "";
				const char* SELECTOwnerPK_SQL_char = SELECTOwnerPK_SQL_str.data();
				if (0 != mysql_query(&ceshi, SELECTOwnerPK_SQL_char)) {
					cout << "查询失败" << endl;
					system("pause");
					return 0;
				}
				res = mysql_store_result(&ceshi);
				nextRow = mysql_fetch_row(res);
				string OwnerPK_str = nextRow[0];
				const char* OwnerPK_ptr = OwnerPK_str.data();
				element_t OwnerPK,r,g_r,OwnerSK_share, OwnerPK_r;
				element_init_G1(OwnerPK,pairing);
				element_init_Zr(r, pairing);
				element_init_G1(g_r, pairing);	
				element_init_G1(OwnerSK_share, pairing);
				element_init_G1(OwnerPK_r, pairing);

				element_set_str(OwnerPK, OwnerPK_ptr, 10);
				element_random(r);
				element_pow_zn(g_r, g, r);
				element_pow_zn(OwnerPK_r, OwnerPK, r);
				element_mul(OwnerSK_share, SKToFog, OwnerPK_r);
				

				char OwnerSK_share_char[MSGSIZE], g_r_char[MSGSIZE];
				string OwnerSK_share_str, g_r_str;
				element_snprint(OwnerSK_share_char,MSGSIZE, OwnerSK_share);
				element_snprint(g_r_char, MSGSIZE, g_r);

				for (int j = 0; j < MSGSIZE; j++) {
					if ((OwnerSK_share_char[j] <= '9'&&OwnerSK_share_char[j] >= '0') || OwnerSK_share_char[j] == ']' || OwnerSK_share_char[j] == '[' || OwnerSK_share_char[j] == ',' || OwnerSK_share_char[j] == ' ') {
						OwnerSK_share_str += OwnerSK_share_char[j];
					}
					else {
						break;
					}
				}
				OwnerSK_share_char[OwnerSK_share_str.size()] = '!';
				for (int j = 0; j < MSGSIZE; j++) {
					if ((g_r_char[j] <= '9'&&g_r_char[j] >= '0') || g_r_char[j] == ']' || g_r_char[j] == '[' || g_r_char[j] == ',' || g_r_char[j] == ' ') {
						g_r_str += g_r_char[j];
					}
					else {
						break;
					}
				}
				g_r_char[g_r_str.size()] = '!';

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, OwnerSK_share_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);

				sClient_Fog = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				while (connect(sClient_Fog, (sockaddr*)&server1, sizeof(SOCKADDR)) == -1) {
				}
				
				send(sClient_Fog, g_r_char, MSGSIZE, NULL);
				
				closesocket(sClient_Fog);


				/*——————————END—————————*/
				/*———————传递解密辅助数据——————*/

			}

			//system("pause");
		}
		
	}

	string SELECTODN_SQL_str = "SELECT FileNum FROM owner_device_num WHERE Owner='" + OwnerID_str + "' and Device = '" + DeviceID_str + "';";
	const char* SELECTODN_SQL_char = SELECTODN_SQL_str.data();
	if (0 != mysql_query(&ceshi, SELECTODN_SQL_char)) {
		cout << "查询失败" << endl;
		system("pause");
		return 0;
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	string DeviceFileNum = nextRow[0];

	string New_DeviceFileNum = to_string(atoi(DeviceFileNum.c_str())+1);
	//cout << New_DeviceFileNum << endl;

	string UPDATEODN_SQL_str = "update owner_device_num set FileNum='" + New_DeviceFileNum + "' WHERE Owner='" + OwnerID_str + "' and Device='" + DeviceID_str + "';";
	const char* UPDATEODN_SQL_char = UPDATEODN_SQL_str.data();
	if (0 != mysql_query(&ceshi, UPDATEODN_SQL_char)) {
		cout << "更新失败" << endl;
		system("pause");
		return 0;
	}

	system("pause");
	return 0;
}


	
