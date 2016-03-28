// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JMConvert.h"

int main(int argc, char *argv[])
{
	if (argc < 5 || argc > 6)
	{
		printf("usage: %s \"Article Data File\" \"JM Data Path\" \"Catalog Map File\" \"Vendor Data File\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}

	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 6)
	{
		full_path(conf, argv[5], 1024);
	}
	else
	{
		get_exe_path(filename, 1024);
		get_file_dir(dir, filename, 1024);
		strcpy(filename, dir);
		strcat(filename, "/JMConvert.ini");

		// 获取真正的配置文件路径
		FILE *fp = fopen(filename, "rb");
		if (NULL == fp)
		{
			printf("可执行程序所在路径下不存在JMConvert.ini文件或无读取权限，异常退出！\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))
		{
			fclose(fp);
			printf("JMConvert.ini读取失败，异常退出！\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);
		if (!get_absolute_path(conf, filename, dir))
		{
			printf("JMConvert.ini内容错误，异常退出！\n");
			return 1;
		}
	}

	TimeConsume all("目录词库转换");
	JMConvert jmc;
	full_path(filename, argv[1], 1024);
	full_path(dir, argv[2], 1024);
	char mapfile[1024];
	full_path(mapfile, argv[3], 1024);
	char vendorfile[1024];
	full_path(vendorfile, argv[4], 1024);
	if (jmc.run(filename, dir, mapfile, vendorfile, conf))
		return 0;

	return 1;
}

