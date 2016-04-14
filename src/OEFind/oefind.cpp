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
bool OEFind::run(const char *find_oe_string,const char *brand_choice,const bool all_brand_flag,const bool match_flag)
{
	if(!all_brand_flag)//指定品牌
	{
		const_char_map<OEData *> &oedata_map = brand_oedata_map[brand_choice];
		if(match_flag)//完全匹配
		{
			auto it = oedata_map.find(find_oe_string);
			if(it!= oedata_map.end())
				OEData_find_result.push_back(it->second);
		}
		else
		{
			for (auto it = oedata_map.begin(); it != oedata_map.end(); ++it)
			{
				const char *oe_code = it->first;
				if(strstr(oe_code,find_oe_string))
					OEData_find_result.push_back(it->second);
			}
		}
	}
	else //未指定品牌
	{
		if(match_flag)//完全匹配
		{
			for(auto it=brand_oedata_map.begin();it != brand_oedata_map.end();++it)
			{
				const_char_map<OEData *> &oedata_map =it->second;
				auto it1 = oedata_map.find(find_oe_string);
				if(it1!= oedata_map.end())
					OEData_find_result.push_back(it1->second);
			}
		}
		else
		{
			for(auto it=brand_oedata_map.begin();it != brand_oedata_map.end();++it)
			{
				const_char_map<OEData *> &oedata_map =it->second;
				for(auto it1 = oedata_map.begin();it1 != oedata_map.end();++it1)
				{
					const char *oe_code = it1->first;
					if(strstr(oe_code,find_oe_string))
						OEData_find_result.push_back(it1->second);
				}
			}
		}
	}
	if(!OEData_find_result.size())
		return false;

	return true;
}
bool OEFind::read_oedata()
{
	e_document_ex doc;
	char filename[1024];
	char confname[1024];
	bool exist_flag=false;

	sprintf(filename,"%s/OEData.e",conf_dir);
	QString filename_CN = filename_CN.fromLocal8Bit(filename);//中文路径无法识别

	QFile OEData_file(filename_CN);
	if(!OEData_file.open(QIODevice::ReadOnly))
	{
		exist_flag = true;
		sprintf(confname,"%s/conf2",conf_dir);
		QString confname_CN = confname_CN.fromLocal8Bit(confname);

		QFile conf(confname_CN);
		if(!conf.open(QIODevice::ReadOnly))
			return false;

		QByteArray OEData_Array=QByteArray::fromBase64(conf.readAll());

		if(!OEData_file.open(QIODevice::WriteOnly))
			return false;
		OEData_file.write(OEData_Array);
		OEData_file.close();
		conf.close();
	}
	OEData_file.close();
	if(!doc.parse_file(filename))
		return false;
	E2DDE efile_BaseData(db_read.scheme());
	efile_BaseData.parse(db_read, doc, e_dde_parse_combine);
	if(exist_flag)
	{
		QString filename_CN = filename_CN.fromLocal8Bit(filename);
		QFile::remove(filename_CN);
	}

	DDETable *OEData_tb = db_read.get("OEData");
	if (!OEData_tb)
		return false;
	size_t size = OEData_tb->size();
	if(!size)
		return false;


	for(size_t i=0;i<size;i++) //建立映射关系
	{
		OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
		char *code = OEData_tb->allocate_string(data->oe_code);
		trim_space(code);
		brand_oedata_map[data->brand][code] = data;
	}

	return true;
}
bool OEFind::parse_database()
{
	char filename[1024];
	char confname[1024];
	bool exist_flag=false;

	sprintf(filename, "%s/Vehicle.e", conf_dir);
	QString filename_CN = filename_CN.fromLocal8Bit(filename);

	QFile Vehicle(filename_CN);  
	if(!Vehicle.open(QIODevice::ReadOnly))   //配置文件不存在
	{
		exist_flag =true;
		sprintf(confname,"%s/conf1",conf_dir);
		QString confname_CN= confname_CN.fromLocal8Bit(confname);

		QFile conf(confname_CN);
		if(!conf.open(QIODevice::ReadOnly))
			return false;

		QByteArray Vehicle_Array =QByteArray::fromBase64(conf.readAll());  

		if(!Vehicle.open(QIODevice::WriteOnly)) 
			return false;

		Vehicle.write(Vehicle_Array);
		Vehicle.close();
		conf.close();
	}
	Vehicle.close();
	DDEScheme scheme; 

	scheme.parse_file(filename);
	if(!scheme.tables().size())
		return false;
	//printf("DDEScheme %lu\n", scheme.tables().size());

	db_read.scheme(&scheme);
	db_read.m_name = "read";
	if(exist_flag)
	{
		QString filename_CN = filename_CN.fromLocal8Bit(filename);
		QFile::remove(filename_CN);
	}
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
	if (!get_absolute_path(conf_dir, "../conf/OEFind/", dir))
		return false;

	return true;
}

