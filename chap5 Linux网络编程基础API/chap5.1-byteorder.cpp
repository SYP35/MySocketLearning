/***********************
��Linux�����ܷ�������̡���˫  ��



�����嵥5-1���жϻ����ֽ���
*******************************/

#include <stdio.h>
void byteorder()
{
	union MyUnion
	{
		short value;
		char union_bytes[sizeof(short)];
	} test;

	test.value = 0x0102;
	if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2))
	{
		printf("Big  endian. \r\n");//�����ֽ��򣬴�˶��루��λ��ǰ�棩
	}
	else if ((test.union_bytes[1] == 1) && (test.union_bytes[0] == 2))
	{
		printf("little  endian. \r\n");//�����ֽ���С�˶���
	}
	else
	{
		printf("unkonwn...\r\n");
	}

}

void main()
{
	byteorder();

	getchar();
}

