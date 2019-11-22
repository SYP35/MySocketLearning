#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFFER_SIZE  1024

//��������http״̬���״̬��Ϣ
static const char* status_line[2] = {"200 OK", "500 Internal server error"};


//�ͻ���telnet���÷������ϼ��ɻ�ø��ļ�

int main(int argc, char* argv[])
{   
    if(argc<=3)
    {
        printf("usage:%s ip_address port_number filename.\r\n", basename(argv[0]));
        return 1;
    }
    
    char* ip = argv[1];
    int port = atoi(argv[2]);
    //��Ŀ���ļ���Ϊ����ĵ�������������
    const char* file_name = argv[3];
    
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET,ip, &address.sin_addr);
    
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd>=0);
    
    int ret = bind(sockfd,(struct sockaddr*)&address, sizeof(address));
    assert(ret!=-1);
    
    ret = listen(sockfd,5);
    assert(ret!=-1);
    
    struct sockaddr_in client;
    socklen_t client_addlength = sizeof(client);
    int connfd = accept(sockfd, (struct sockaddr*)&client, &client_addlength);
    if(connfd < 0)
    {
        printf("errno is : %d\r\n", errno );
    }
    else
    {
        //���ڱ��� http Ӧ���״̬�С�ͷ���ֶκ�һ�����еĻ�����            
        char header_buf[BUFFER_SIZE];
        memset(header_buf, '\0', sizeof(header_buf));
        
        //���ڴ��Ŀ���ļ����ݵ�Ӧ�ó��򻺴�
        char* file_buf;
        
        //���ڻ�ȡĿ���ļ������ԣ������Ƿ�Ϊ Ŀ¼���ļ���С��
        struct stat file_stat;
        //��¼Ŀ���ļ��Ƿ�����Ч�ļ�
        bool valid=true;
        //������ header_buf Ŀǰ�Ѿ�ʹ���˶����ֽڿռ�
        int len = 0;
        if(stat(file_name, &file_stat) < 0)
        {
            //not exist��Ŀ���ļ�������
            valid = false;
        }
        else
        {
            if(S_ISDIR(file_stat.st_mode))
            {
                valid = false;//Ŀ���ļ���һ��Ŀ¼
            }
			//��ǰ�û��ж�ȡĿ���ļ���Ȩ��
            else if(file_stat.st_mode & S_IROTH)
            {
				//��̬���仺���� fiel_buf����ָ�����СΪĿ���ļ��Ĵ�С +1��Ȼ��Ŀ���ļ����뻺����file_buf
                int fd = open(file_name, O_RDONLY);
                file_buf = new char[file_stat.st_size + 1];
                memset(file_buf, '\0', file_stat.st_size +1);
                if(read(fd, file_buf, file_stat.st_size)< 0)
                {
                    valid = false;
                }
            }
            else
            {
                valid = false;
            }        
        }
        ////���Ŀ���ļ���Ч������������httpӦ��
        if(valid)
        {
			//�����ⲿ�����ݽ�httpӦ���״̬�С�"Content-length"ͷ���ֶκ�һ���������μ���header_buf��
            ret = snprintf(header_buf, BUFFER_SIZE -1, "%s %s \r\n", "HTTP/1.1", status_line[0]);
            len += ret;
            ret = snprintf(header_buf+len, BUFFER_SIZE-1-len, "Content-length:%d\r\n", file_stat.st_size);
            len += ret;
            ret = snprintf(header_buf+len, BUFFER_SIZE-1-len,"%s", "\r\n");
            
            /////���� writev �� header_buf��file_buf ������һ��д��
            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = file_stat.st_size;
            ret = writev(connfd, iv, 2);
        }
        else
        {
			//���Ŀ���ļ���Ч����֪ͨ�ͻ��ˣ��������������ڲ�����
            ret = snprintf(header_buf, BUFFER_SIZE-1, "%s %s \r\n","HTTP/1.1",status_line[1]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n");
            send(connfd,header_buf, strlen(header_buf), 0);
        }
       
     
        close(connfd);
        delete [] file_buf;
    }  

    close(sockfd);
    
    return 0;
}

