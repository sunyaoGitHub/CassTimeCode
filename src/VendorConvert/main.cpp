// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VendorConvert.h"

int main(int argc, char *argv[])
{
	if (argc < 3 || argc > 4)
	{
		printf("usage: %s \"OE Data File\" \"Vendor Data Path\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}

	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 4)
	{
		full_path(conf, argv[3], 1024);
	}
	else
	{
		get_exe_path(filename, 1024);
		get_file_dir(dir, filename, 1024);
		strcpy(filename, dir);
		strcat(filename, "/VendorConvert.ini");

		// ��ȡ�����������ļ�·��
		FILE *fp = fopen(filename, "rb");
		if (NULL == fp)
		{
			printf("��ִ�г�������·���²�����VendorConvert.ini�ļ����޶�ȡȨ�ޣ��쳣�˳���\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))
		{
			fclose(fp);
			printf("VendorConvert.ini��ȡʧ�ܣ��쳣�˳���\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);
		if (!get_absolute_path(conf, filename, dir))
		{
			printf("VendorConvert.ini���ݴ����쳣�˳���\n");
			return 1;
		}
	}

	TimeConsume all("����������ת��");
	VendorConvert vc;
	full_path(filename, argv[1], 1024);
	full_path(dir, argv[2], 1024);
	if (vc.run(filename, dir, conf))
		return 0;

	return 1;
}

