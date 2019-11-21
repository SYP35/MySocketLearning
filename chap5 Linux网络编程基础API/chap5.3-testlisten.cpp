/***********************
��Linux�����ܷ�������̡���˫  ��

������ ����12345�˿ڣ�������������ʱ����Ҫ�������� ./a.out 192.168.12.10 12345 2 

�ͻ��ˣ������� telnet 192.168.12.10 12345 ����������˿�
Ȼ���� ������ �鿴�˿ڵ�������� netstat  -nt | grep 12345

����������3��������IP��ַ���˿ںź�backlogֵ���ڿͻ��˶��telnet�����Է���ֻ�У�backlog+1��3���ǽ������ӵģ���������SYN_RECV״̬��

�����嵥5-3��testlisten
*******************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static bool stop = false;

/* SIGTERM �źŵĴ�����������ʱ�����������е�ѭ�� */
static void handle_term(int sig)
{
	stop = true;
}

int main(int argc, char* argv[])
{
	signal(SIGTERM, handle_term);
	if (argc <= 3)
	{
		printf("usage: %s ip_address port_number backlog.\r\n", basename(argv[0]));
		return 1;
	}

	const char* ip = argv[1];
	int port = atoi(argv[2]);
	int backlog = atoi(argv[3]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(socket >= 0);

	// create a ipv4 socket address
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);

	ret = listen(sock, backlog);
	assert(ret != -1);


	// wait for SIGTERM
	while (!stop)
	{
		sleep(1);
	}

	// close socket
	close(sock);

	printf("hello socket...");
	return 0;
}

