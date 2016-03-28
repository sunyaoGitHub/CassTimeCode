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

		// 获取真正的配置文件路径
		FILE *fp = fopen(filename, "rb");
		if (NULL == fp)
		{
			printf("可执行程序所在路径下不存在VendorConvert.ini文件或无读取权限，异常退出！\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))
		{
			fclose(fp);
			printf("VendorConvert.ini读取失败，异常退出！\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);
		if (!get_absolute_path(conf, filename, dir))
		{
			printf("VendorConvert.ini内容错误，异常退出！\n");
			return 1;
		}
	}

	TimeConsume all("经销商数据转换");
	VendorConvert vc;
	full_path(filename, argv[1], 1024);
	full_path(dir, argv[2], 1024);
	if (vc.run(filename, dir, conf))
		return 0;

	return 1;
}

