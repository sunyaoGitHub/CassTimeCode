// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OEUnify.h"

int main(int argc, char *argv[])
{
	char conf[1024];
	char dir[1024];
	char filename[1024];
	if (argc == 1 || argc > 3)//参数为1个或者2个，参数1：数据文件.E格式  参数2：配置文件
	{
		printf("usage: %s \"OE Data Path\" [\"Configure.xml\"]\n", argv[0]);
		return 1;
	}


	if (argc == 3)
	{
		full_path(conf, argv[2], 1024);//获取配置文件全路径
	}
	else
	{
		get_exe_path(filename, 1024);//获取当前执行文件的全路径包含文件名
		get_file_dir(dir, filename, 1024);//获取文件所在目录
		strcpy(filename, dir);
		strcat(filename, "/OEUnify.ini");//获取初始化文件全路径

		// 获取真正的配置文件路径
		FILE *fp = fopen(filename, "rb");//只读方式打开初始化文件
		if (NULL == fp)
		{
			printf("可执行程序所在路径下不存在E2DB.ini文件或无读取权限，异常退出！\n");
			return 1;
		}

		if(!fgets(filename, 1024, fp))//获取文件内容
		{
			fclose(fp);
			printf("E2DB.ini读取失败，异常退出！\n");
			return 1;
		}

		fclose(fp);

		truncate_cr_lf(filename);//去除末尾回车换行
		if (!get_absolute_path(conf, filename, dir))//获取配置文件绝对路径
		{
			printf("E2DB.ini内容错误，异常退出！\n");
			return 1;
		}
	}

	TimeConsume all("配件标准化");
	OEUnify ou;
	full_path(dir, argv[1], 1024);//获取数据文件的绝对路径
	if (ou.run(dir, conf))
		return 0;

	return 1;
}

