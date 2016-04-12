#include "stdafx.h"

bool Encryption_Vehicle(const char *conf_dir)
{
	char file_name_char[1024];
	char conf_name_char[1024];

	sprintf(file_name_char,"%s/Vehicle.e",conf_dir);
	sprintf(conf_name_char,"%s/conf1",conf_dir);

	QString file_name =file_name.fromLocal8Bit(file_name_char);
	QString conf_name =file_name.fromLocal8Bit(conf_name_char);

	QFile Vehicle(file_name);  
	if(!Vehicle.open(QIODevice::ReadOnly)) 
	{  
		return false;
	} 
	QByteArray  Vehicle_Array = Vehicle.readAll().toBase64();  

	QFile conf(conf_name);  
	if(!conf.open(QIODevice::WriteOnly)) 
	{  
		return false;
	}  
	conf.write(Vehicle_Array); 

	Vehicle.close();  
	conf.close();

	return true;
}
bool Encryption_BaseData(const char *conf_dir)
{
	char file_name_char[1024];
	char conf_name_char[1024];

	sprintf(file_name_char,"%s/BaseData.e",conf_dir);
	sprintf(conf_name_char,"%s/conf2",conf_dir);

	QString file_name =file_name.fromLocal8Bit(file_name_char);
	QString conf_name =file_name.fromLocal8Bit(conf_name_char);

	QFile BaseData(file_name);  
	if(!BaseData.open(QIODevice::ReadOnly)) 
	{  
		return false;
	} 
	QByteArray  BaseData_Array = BaseData.readAll().toBase64();  

	QFile conf(conf_name);  
	if(!conf.open(QIODevice::WriteOnly)) 
	{  
		return false;
	}  
	conf.write(BaseData_Array); 

	BaseData.close();  
	conf.close();
	return true;
}
bool Encryption_OEData(const char *conf_dir)
{
	char file_name_char[1024];
	char conf_name_char[1024];

	sprintf(file_name_char,"%s/OEData.e",conf_dir);
	sprintf(conf_name_char,"%s/conf3",conf_dir);

	QString file_name =file_name.fromLocal8Bit(file_name_char);
	QString conf_name =file_name.fromLocal8Bit(conf_name_char);

	QFile OEData(file_name);  
	if(!OEData.open(QIODevice::ReadOnly)) 
	{  
		return false;
	} 
	QByteArray  OEData_Array = OEData.readAll().toBase64();  

	QFile conf(conf_name);  
	if(!conf.open(QIODevice::WriteOnly)) 
	{  
		return false;
	}  
	conf.write(OEData_Array); 

	OEData.close();  
	conf.close();
	return true;
}

int main(int argc, char *argv[])
{
	char file_name[1024];
	char conf_dir[1024];

	if(argc <= 2) //指定配置文件路径
	{
		get_exe_path(file_name,1024);
		get_file_dir(conf_dir,conf_dir,1024);
		if(argc == 2)
		{
			if(!get_absolute_path(conf_dir,argv[1],conf_dir))
			{
				printf("Enter the path error\n");
				return false;
			}
		}

		if(Encryption_Vehicle(conf_dir))
			printf("Encryption Vehicle.e success！\n");
		else 
			printf("Encryption Vehicle.e failed,please confirm file exist\n");

		if(Encryption_BaseData(conf_dir))
			printf("Encryption BaseData.e success！\n");
		else
			printf("Encryption BaseData.e failed,please confirm file exist！\n");

		if(Encryption_OEData(conf_dir))
			printf("Encryption OEData.e success！\n");
		else
			printf("Encryption OEData.e failed,please confirm file exist！\n");
		return true;
	}
	else 
	{

		printf("Enter the param error!\n");
		return false;
	}
}
