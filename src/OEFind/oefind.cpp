#include "stdafx.h"
#include "OEFind.h"

#pragma warning(disable:4996)


class e_document_ex: public e_document
{
public:
	e_document_ex();
	virtual ~e_document_ex();

	virtual void parse_error(const char *what, void *where) override;
};

e_document_ex::e_document_ex() : e_document()
{
}

e_document_ex::~e_document_ex()
{
}

void e_document_ex::parse_error(const char *what, void *where)
{
	e_document::parse_error(what, where);
	if (where)
	{
		char buffer[100];
		strncpy_ex(buffer, (char *)where, 100);
		printf("parse error: %s\n\tnear: %s\n", what, buffer);
	} 
	else
	{
		printf("parse error: %s\n", what);
	}
}

OEFind::OEFind() : db_read(0)
{

}

OEFind::~OEFind()
{

}
bool OEFind::run(const char *find_oe_string,const int brand_choice,const bool all_brand_flag,const bool match_flag)
{
	if (!all_brand_flag)//指定品牌
	{
		DDETable *Brand_tb = db_read.get("Brand");
		Brand *data=0;
		data =reinterpret_cast<Brand*>(Brand_tb->at(brand_choice)) ;//品牌
		vector<OEData*> &brand_oedatas =brand_oedata_map[data];


		if(match_flag) //完全匹配
		{
			for (size_t i=0;i<brand_oedatas.size();i++)
			{
				if(!strcmp(brand_oedatas[i]->oe_code,find_oe_string))
					OEData_find_result.push_back(brand_oedatas[i]);
			}
		}
		else
		{
			for (size_t i=0;i<brand_oedatas.size();i++)
			{
				if(strstr(brand_oedatas[i]->oe_code,find_oe_string))
					OEData_find_result.push_back(brand_oedatas[i]);
			}
		}

	}
	else  //未指定品牌
	{
		DDETable *OEData_tb = db_read.get("OEData");

		if(match_flag)//完全匹配
		{
			for(size_t i =0;i<OEData_tb->size();i++)
			{
				OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
				if(!strcmp(data->oe_code,find_oe_string))
					OEData_find_result.push_back(data);
			}
		}
		else
		{
			for(size_t i =0;i<OEData_tb->size();i++)
			{
				OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
				if(strstr(data->oe_code,find_oe_string))
					OEData_find_result.push_back(data);
			}
		}
		
	}

#if 0
	QString brand_choice_string;
	DDETable *OEData_tb = db_read.get("OEData");
	size_t size = OEData_tb->size();

	switch(brand_choice)
	{
		case 1:
			brand_choice_string =brand_choice_string.fromLocal8Bit("奥迪");
			break;
		case 2:
			brand_choice_string =brand_choice_string.fromLocal8Bit("奔驰");
			break;
		case 3:
			brand_choice_string =brand_choice_string.fromLocal8Bit("宝马");
			break;
		case 4:
			brand_choice_string =brand_choice_string.fromLocal8Bit("捷豹");
			break;
		case 5:
			brand_choice_string =brand_choice_string.fromLocal8Bit("路虎");
			break;
		case 6:
			brand_choice_string =brand_choice_string.fromLocal8Bit("保时捷");
			break;
		case 7:
			brand_choice_string =brand_choice_string.fromLocal8Bit("沃尔沃");
			break;
		case 8:
			brand_choice_string =brand_choice_string.fromLocal8Bit("大众");
			break;
		case 9:
			brand_choice_string =brand_choice_string.fromLocal8Bit("玛莎拉蒂");
			break;
		default:
			break;
	}

	if(match_flag)//全部匹配
	{
		for(size_t i =0;i<size;i++)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			trim_space(data->oe_code);
			if(!strcmp(find_oe_string,data->oe_code))
			{
				if(!brand_choice)
					OEData_find_result.push_back(data);
				else
				{
					QString brand=brand.fromLocal8Bit(data->brand);
					if(!QString::compare(brand_choice_string,brand,Qt::CaseInsensitive))
						OEData_find_result.push_back(data);
				}
			}
		}
	}
	else//部分匹配
	{
		for(size_t i =0;i<size;i++)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			trim_space(data->oe_code);
			if(strstr(data->oe_code,find_oe_string))
			{
				if(!brand_choice)
					OEData_find_result.push_back(data);
				else
				{
					QString brand=brand.fromLocal8Bit(data->brand);
					if(!QString::compare(brand_choice_string,brand,Qt::CaseInsensitive))
						OEData_find_result.push_back(data);
				}
			}
		}
	}
#endif
	if(!OEData_find_result.size())
		return false;

	return true;
}

bool OEFind::read_basedata()
{
	e_document_ex doc;
	char filename[1024];
	char confname[1024];
	bool exist_flag=false;
	if(!check_config_BaseData())
	{
		exist_flag = true;
		sprintf(filename,"%s/conf2",conf_dir);
		sprintf(confname,"%s/BaseData.e",conf_dir);
		QFile conf2(filename);  
		if(!conf2.open(QIODevice::ReadOnly)) 
			return false;
		QByteArray BaseData_Array =QByteArray::fromBase64(conf2.readAll());  

		QFile BaseData(confname);  
		if(!BaseData.open(QIODevice::WriteOnly))   
			return false;
		BaseData.write(BaseData_Array); 

		conf2.close();
		BaseData.close(); 

		memset(filename,0,sizeof(filename));
		memset(confname,0,sizeof(confname));
	}

	sprintf(filename, "%s/BaseData.e", conf_dir);
	if(!doc.parse_file(filename))
		return false;
	E2DDE efile_BaseData(db_read.scheme());
	efile_BaseData.parse(db_read, doc, e_dde_parse_combine);
	if(exist_flag)
		QFile::remove(filename);

	DDETable *Brand_tb = db_read.get("Brand");
	if(!Brand_tb)
		return false;
	size_t size= Brand_tb->size();
	for(size_t i=0;i<size;i++)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_name_map[data->brand_name_cn] = data;
	}
	return true;
}
bool OEFind::read_oedata()
{
	e_document_ex doc;
	char filename[1024];
	char confname[1024];
	bool exist_flag=false;
	if(!check_config_OEData())
	{
		exist_flag = true;
		sprintf(filename,"%s/conf3",conf_dir);
		sprintf(confname,"%s/OEData.e",conf_dir);
		QFile conf3(filename);  
		if(!conf3.open(QIODevice::ReadOnly)) 
			return false;
		QByteArray OEData_Array =QByteArray::fromBase64(conf3.readAll());  

		QFile OEData(confname);  
		if(!OEData.open(QIODevice::WriteOnly))  
			return false;
		OEData.write(OEData_Array); 

		conf3.close();
		OEData.close();  
	}
	sprintf(filename, "%s/OEData.e", conf_dir);

	if(!doc.parse_file(filename))
		return false;
	E2DDE efile_BaseData(db_read.scheme());
	efile_BaseData.parse(db_read, doc, e_dde_parse_combine);
	if(exist_flag)
		QFile::remove(filename);

	DDETable *OEData_tb = db_read.get("OEData");
	if (!OEData_tb)
		return false;
	size_t size = OEData_tb->size();
	if(!size)
		return false;


	for(size_t i=0;i<size;i++) //建立映射关系
	{
		Brand *brand = 0;
		OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
		trim_space(data->oe_code);
		auto it = Brand_name_map.find(data->brand); 
		if(it == Brand_name_map.end())
			continue;
		else
			brand = it->second;

		vector <OEData*> &brand_oedatas = brand_oedata_map[brand];
		brand_oedatas.push_back(data);
	}

	return true;
}
bool OEFind::parse_database()
{
	char filename[1024];
	char confname[1024];
	bool exist_flag=false;

	if(!check_config_Vehicle()) //配置文件不存在
	{
		exist_flag=true;
		sprintf(filename,"%s/conf1",conf_dir);
		sprintf(confname,"%s/Vehicle.e",conf_dir);
		QFile conf1(filename);  
		if(!conf1.open(QIODevice::ReadOnly)) 
			return false;
		QByteArray Vehicle_Array =QByteArray::fromBase64(conf1.readAll());  


		QFile Vehicle(confname);  
		if(!Vehicle.open(QIODevice::WriteOnly)) 
			return false;
		Vehicle.write(Vehicle_Array); 
		conf1.close();
		Vehicle.close();  
		memset(filename,0,sizeof(filename));
		memset(confname,0,sizeof(confname));
	}
	sprintf(filename, "%s/Vehicle.e", conf_dir);
	DDEScheme scheme; 

	scheme.parse_file(filename);
	if(!scheme.tables().size())
		return false;
	//printf("DDEScheme %lu\n", scheme.tables().size());

	db_read.scheme(&scheme);
	db_read.m_name = "read";
	if(exist_flag)
		QFile::remove(filename);

	if(!read_basedata())
		return false;
	if(!read_oedata())
		return false;

	return true;
}
bool OEFind::get_conf_dir()
{
	char filename[1024];
	char dir[1024];

	get_exe_path(filename, 1024);
	get_file_dir(dir, filename, 1024);
	strcpy(filename, dir);
	strcat(filename, "/OEFind.ini");

	FILE *fp = fopen(filename, "rb");
	if(NULL == fp)
		return false;
	if(!fgets(filename, 1024, fp))
		return false;

	fclose(fp);

	truncate_cr_lf(filename);
	if (!get_absolute_path(conf_dir, filename, dir))
		return false;

	return true;
}

bool OEFind::check_config_Vehicle()
{
	char filename[1024];
	struct _finddata_t file;	// 定义结构体变量

	sprintf(filename, "%s/*", conf_dir);
	long handle=_findfirst(filename, &file);	// 查找所有数据文件
	if (handle == -1)
		return false;
	do
	{
		if (!strcmp(file.name,"Vehicle.e"))
			return true;
	} while(!(_findnext(handle,&file)));
	_findclose(handle);

	return false;
}
bool OEFind::check_config_OEData()
{
	char filename[1024];
	struct _finddata_t file;	// 定义结构体变量

	sprintf(filename, "%s/*", conf_dir);
	long handle=_findfirst(filename, &file);	// 查找所有数据文件
	if (handle == -1)
		return false;
	do
	{
		if (!strcmp(file.name,"OEData.e"))
			return true;
	} while(!(_findnext(handle,&file)));
	_findclose(handle);

	return false;
}
bool OEFind::check_config_BaseData()
{
	char filename[1024];
	struct _finddata_t file;	// 定义结构体变量

	sprintf(filename, "%s/*", conf_dir);
	long handle=_findfirst(filename, &file);	// 查找所有数据文件
	if (handle == -1)
		return false;
	do
	{
		if (!strcmp(file.name,"BaseData.e"))
			return true;
	} while(!(_findnext(handle,&file)));
	_findclose(handle);

	return false;
}

bool OEFind::file_encryption() //文件加密
{
	char filename[1024];
	char confname[1024];

	sprintf(filename,"%s/Vehicle.e",conf_dir);
	sprintf(confname,"%s/conf1",conf_dir);

	QFile Vehicle(filename);  
	if(!Vehicle.open(QIODevice::ReadOnly)) 
	{  
		return false;
	}  
	QByteArray  Vehicle_Array = Vehicle.readAll().toBase64();  

	QFile conf1(confname);  
	if(!conf1.open(QIODevice::WriteOnly)) 
	{  
		return false;
	}  
	conf1.write(Vehicle_Array); 

	Vehicle.close();  
	conf1.close();
	memset(filename,0,sizeof(filename));
	memset(confname,0,sizeof(confname));

	sprintf(filename,"%s/BaseData.e",conf_dir);
	sprintf(confname,"%s/conf2",conf_dir);
	QFile BaseData(filename);  
	if(!BaseData.open(QIODevice::ReadOnly)) 
	{  

		return false;
	}  
	QByteArray BaseData_Array= BaseData.readAll().toBase64();  

	QFile conf2(confname);  
	if(!conf2.open(QIODevice::WriteOnly)) 
		return false;
	conf2.write(BaseData_Array);  

	BaseData.close();  
	conf2.close(); 


	memset(filename,0,sizeof(filename));
	memset(confname,0,sizeof(confname));

	sprintf(filename,"%s/OEData.e",conf_dir);
	sprintf(confname,"%s/conf3",conf_dir);
	QFile OEData(filename);  
	if(!OEData.open(QIODevice::ReadOnly)) 
	{  
		return false;
	}  
	QByteArray OEData_Array= OEData.readAll().toBase64();  

	QFile conf3(confname);  
	if(!conf3.open(QIODevice::WriteOnly)) 
	{
		return false;
	}  
	conf3.write(OEData_Array);  

	OEData.close();  
	conf3.close(); 

	return true;
}

