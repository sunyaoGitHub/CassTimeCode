// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ArticleConvert.h"

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3) //����1 �������ļ�  ����2�������ļ�
	{
		printf("usage: %s \"Article Data Path\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}

	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 3)
	{
		full_path(conf, argv[2], 1024);//��ȡ�����ļ���ȫ·��
	}
	else
	{
		get_exe_path(filename, 1024);//��ȡ��ִ���ļ���ȫ·��
		get_file_dir(dir, filename, 1024);//��ȡִ���ļ�Ŀ¼
		strcpy(filename, dir);
		strcat(filename, "/ArticleConvert.ini");//��ȡ��ʼ���ļ�

		// ��ȡ�����������ļ�·��
		FILE *fp = fopen(filename, "rb");//�򿪳�ʼ���ļ�
		if (NULL == fp)
		{
			printf("��ִ�г�������·���²�����ArticleConvert.ini�ļ����޶�ȡȨ�ޣ��쳣�˳���\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))//��ȡ�ļ�����
		{
			fclose(fp);
			printf("ArticleConvert.ini��ȡʧ�ܣ��쳣�˳���\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);//ȥ��ĩβ�س�����
		if (!get_absolute_path(conf, filename, dir))//��ȡĬ�������ļ�ȫ·��
		{
			printf("ArticleConvert.ini���ݴ����쳣�˳���\n");
			return 1;
		}
	}

	TimeConsume all("���ת��");
	ArticleConvert ac;
	full_path(dir, argv[1], 1024);//��ȡ�����ļ�Ŀ¼
	if (ac.run(dir, conf))
		return 0;

	return 1;
}

