// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "E2DB.h"

int main(int argc, char *argv[])
{
	if (argc == 1 || argc > 3)
	{
		printf("usage: %s \"ArticleData.QS\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}

	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 3)
	{
		full_path(conf, argv[2], 1024);
	}
	else
	{
		get_exe_path(filename, 1024);
		get_file_dir(dir, filename, 1024);
		strcpy(filename, dir);
		strcat(filename, "/E2DB.ini");

		// 获取真正的配置文件路径
		FILE *fp = fopen(filename, "rb");
		if (NULL == fp)
		{
			printf("可执行程序所在路径下不存在E2DB.ini文件或无读取权限，异常退出！\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))
		{
			fclose(fp);
			printf("E2DB.ini读取失败，异常退出！\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);
		if (!get_absolute_path(conf, filename, dir))
		{
			printf("E2DB.ini内容错误，异常退出！\n");
			return 1;
		}
	}

	TimeConsume all("入库");
	E2DB e2db;
	full_path(filename, argv[1], 1024);
	if (e2db.run(filename, conf))
		return 0;

	return 1;
}

