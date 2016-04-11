// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OEUnify.h"

int main(int argc, char *argv[])
{
	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 1 || argc > 3)//����Ϊ1������2��������1�������ļ�.E��ʽ  ����2�������ļ�
	{
		printf("usage: %s \"OE Data Path\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}


	if (argc == 3)
	{
		full_path(conf, argv[2], 1024);//��ȡ�����ļ�ȫ·��
	}
	else
	{
		get_exe_path(filename, 1024);//��ȡ��ǰִ���ļ���ȫ·�������ļ���
		get_file_dir(dir, filename, 1024);//��ȡ�ļ�����Ŀ¼
		strcpy(filename, dir);
		strcat(filename, "/OEUnify.ini");//��ȡ��ʼ���ļ�ȫ·��

		// ��ȡ�����������ļ�·��
		FILE *fp = fopen(filename, "rb");//ֻ����ʽ�򿪳�ʼ���ļ�
		if (NULL == fp)
		{
			printf("��ִ�г�������·���²�����E2DB.ini�ļ����޶�ȡȨ�ޣ��쳣�˳���\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))//��ȡ�ļ�����
		{
			fclose(fp);
			printf("E2DB.ini��ȡʧ�ܣ��쳣�˳���\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);//ȥ��ĩβ�س�����
		if (!get_absolute_path(conf, filename, dir))//��ȡ�����ļ�����·��
		{
			printf("E2DB.ini���ݴ����쳣�˳���\n");
			return 1;
		}
	}

	TimeConsume all("�����׼��");
	OEUnify ou;
	full_path(dir, argv[1], 1024);//��ȡ�����ļ��ľ���·��
	if (ou.run(dir, conf))
		return 0;

	return 1;
}

