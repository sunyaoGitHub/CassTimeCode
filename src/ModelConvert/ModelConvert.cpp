#include "stdafx.h"
#include "ModelConvert.h"
#include "ModelDigital.h"
#include "ModelParse.h"

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

ModelConvert::ModelConvert() : db_result(0)
{
}

ModelConvert::~ModelConvert()
{
}

bool ModelConvert::run(const char *_data_file, const char *_conf_file)
{
	conf_file = _conf_file;
	if (!read_configure())
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”ModelConvert.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}
	}
	else
	{
		char exe_dir[1024];
		get_exe_path(scheme_file, 1024);
		get_file_dir(exe_dir, scheme_file, 1024);
		get_absolute_path(scheme_file, "../../common/conf/DDE/Vehicle.e", exe_dir);
	}

	DDEScheme scheme;
	if (!scheme.parse_file(scheme_file))
	{
		printf("配置文件“%s”不存在或错误，异常退出！\n", scheme_file);
		return false;
	}

	db_result.scheme(&scheme);

	if (!read_base())
	{
		printf("基础数据读入失败！\n");
		return false;
	}

	data_file = _data_file;
	if (!read_data())
	{
		printf("车型数据读取失败！\n");
		return false;
	}

	{
		TimeConsume tc("数字化");
		ModelDigital md;
		if (!md.run(db_result))
		{
			printf("数字化失败！\n");
			return false;
		}
	}

	{
		TimeConsume tc("车型解析");
		ModelParse mp;
		if (!mp.run(db_result, mode, import_check))
		{
			printf("车型解析失败！\n");
			return false;
		}
	}

	if (!write_result())
	{
		printf("结果保存失败！\n");
		return false;
	}

	return true;
}

// 读取配置文件
bool ModelConvert::read_configure()
{
	try
	{
		xml_conf.parse_file<0>(conf_file);
	}
	catch (parse_error &e)
	{
		printf("配置文件“%s”解析错误: %s，位置：%s，异常退出！\n", conf_file, e.what(), e.where<char>());
		return false;
	}

	root_conf = xml_conf.first_node("ModelConvert", 0, false);
	if (!root_conf)
	{
		printf("配置文件“%s”内容无效，异常退出！\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = root_conf->first_attribute("echo_time", 0, false);
	if (attribute && ((*attribute->value()) == '1'))
		TimeConsume::s_mode = 1;
	else
		TimeConsume::s_mode = 0;

	import_check = false;
	attribute = root_conf->first_attribute("import_check", 0, false);
	if (attribute && ((*attribute->value()) == '1'))
		import_check = true;

	mode = 1;
	attribute = root_conf->first_attribute("2", 0, false);
	if (attribute && ((*attribute->value()) == '2'))
		mode = 2;

	get_file_dir(conf_dir, conf_file, 1024);
	return true;
}

bool ModelConvert::read_base()
{
	TimeConsume tc("读取基础数据");
	xml_node<> *base_data = root_conf->first_node("BaseData", 0, false);
	if (!base_data)
		return false;

	char file_name[1024];
	xml_attribute<> *attribute = base_data->first_attribute("file", 0, false);
	if (!attribute)
		return false;

	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("配置文件“%s”ModelConvert.BaseData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	if (!doc.parse_file(file_name))
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_result.scheme());
	efile.parse(db_result, doc, e_dde_parse_combine);
	return true;
}

bool ModelConvert::read_data()
{
	TimeConsume tc("读取车型数据");
	e_document_ex doc;
	if (!doc.parse_file(data_file))
		return false;

	E2DDE efile(db_result.scheme());
	efile.parse(db_result, doc, e_dde_parse_combine);
	return true;
}

bool ModelConvert::write_result()
{
	xml_node<> *result_data = root_conf->first_node("ResultData", 0, false);
	if (!result_data)
		return true;

	xml_attribute<> *attribute = result_data->first_attribute("write", 0, false);
	if (!attribute || *(attribute->value()) != '1')
		return true;

	char file_name[1024];
	attribute = result_data->first_attribute("file", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(file_name, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”ModelConvert.ResultData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(conf_dir, file_name, 1024);
		create_dir(conf_dir, 0755);
	}
	else
	{
		sprintf(file_name, "%s/VehicleData.e", conf_dir);
	}

	DDETable *tb = db_result.get("LevelModelData");
	db_result.remove(tb);

	tb = db_result.get("Level2YunData");
	db_result.remove(tb);

	if (!E2DDE::print_file(db_result, file_name))
	{
		printf("Write result file fail!\n");
		return false;
	}

	return true;
}

