#include "stdafx.h"
#include "VendorConvert.h"
#include "VendorParse.h"

VendorConvert::VendorConvert() : db_read(0), db_result(0)
{
}

VendorConvert::~VendorConvert()
{
}

bool VendorConvert::run(const char *oe_file, const char *data_dir, const char *_conf_file)
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
			printf("配置文件“%s”VendorConvert.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
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

	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_oe(oe_file))
	{
		printf("OE#数据读入失败！\n");
		return false;
	}

	if (!read_base())
	{
		printf("品牌数据读入失败！\n");
		return false;
	}

	if (!read_vendor_data(data_dir))
	{
		printf("经销商数据读入失败！\n");
		return false;
	}

	if (!write_result())
	{
		printf("结果保存失败！\n");
		return false;
	}

	if (!write_delta())
	{
		printf("增量数据生成失败！\n");
		return false;
	}

	return true;
}

// 读取配置文件
bool VendorConvert::read_configure()
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

	root_conf = xml_conf.first_node("VendorConvert", 0, false);
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

	trim_space = false;
	attribute = root_conf->first_attribute("trim_space", 0, false);
	if (attribute && ((*attribute->value()) == '1'))
		trim_space = true;

	oe_warning = true;
	attribute = root_conf->first_attribute("oe_warning", 0, false);
	if (attribute && ((*attribute->value()) == '0'))
		oe_warning = false;

	price_warning = true;
	attribute = root_conf->first_attribute("price_warning", 0, false);
	if (attribute && ((*attribute->value()) == '0'))
		price_warning = false;

	repeat_warning = true;
	attribute = root_conf->first_attribute("repeat_warning", 0, false);
	if (attribute && ((*attribute->value()) == '0'))
		repeat_warning = false;

	get_file_dir(conf_dir, conf_file, 1024);
	Configure conf;
	for (xml_node<> *node = root_conf->first_node("Vendor", 0, false); node; node = node->next_sibling("Vendor", 0, false))
	{
		conf.run = 1;
		conf.type = 1;
		conf.mode = 1;
		attribute = node->first_attribute("run", 0, false);
		if (attribute && ((*attribute->value()) == '0'))
			conf.run = 0;

		attribute = node->first_attribute("type", 0, false);
		if (attribute && ((*attribute->value()) == '2'))
			conf.type = 2;

		attribute = node->first_attribute("mode", 0, false);
		if (attribute)
		{
			conf.mode = atoi(attribute->value());
			if (conf.mode < 1)
				conf.mode = 1;

			if (conf.mode > 3)
				conf.mode = 3;
		}

		attribute = node->first_attribute("name", 0, false);
		if (!attribute || ((*attribute->value()) == '\0'))
		{
			printf("配置文件“%s”中“VendorConvert.Vendor”无“name”属性或为空\n", conf_file);
			continue;
		}

		Configure_map[attribute->value()] = conf;
	}

	return true;
}

bool VendorConvert::read_base()
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
		printf("配置文件“%s”ArticleConvert.BaseData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	if (!doc.parse_file(file_name))
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);

	DDETable *Brand_tb = db_read.get("Brand");
	if (!Brand_tb)
		return false;

	DDETable *Brand_tb2 = db_result.get("Brand");
	if (!Brand_tb2)
		return false;

	Brand_tb2->copy_record(*Brand_tb);
	return true;
}

bool VendorConvert::read_oe(const char *oe_file)
{
	TimeConsume tc("读取OE#数据");
	e_document_ex doc;
	if (!doc.parse_file(oe_file))
	{
		printf("read_oe parse_file fail\n");
		return false;
	}

	E2DDE efile(db_result.scheme());
	efile.parse(db_result, doc, e_dde_parse_combine);

	DDETable *tb = db_result.find("OEData");
	if (!tb)
		return false;

	return true;
}

bool VendorConvert::read_vendor_data(const char *data_dir)
{
	TimeConsume tc("经销商数据解析");
	VendorParse vp;
	if (!vp.run(this, data_dir))
	{
		printf("经销商数据解析失败！\n");
		return false;
	}

	return true;
}

bool VendorConvert::write_result()
{
	xml_node<> *node = root_conf->first_node("ResultData", 0, false);
	if (!node)
		return true;

	xml_attribute<> *attribute = node->first_attribute("write", 0, false);
	if (!attribute || *(attribute->value()) != '1')
		return true;

	char file_name[512];
	char dir[512];
	attribute = node->first_attribute("file", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(file_name, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”VendorConvert.ResultData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(dir, file_name, 1024);
		create_dir(dir, 0755);
	}
	else
	{
		sprintf(file_name, "%s/VendorData.e", conf_dir);
	}

	DDETable *tb = db_result.get("OEData");
	db_result.remove(tb);

	tb = db_result.get("Brand");
	db_result.remove(tb);

	if (!E2DDE::print_file(db_result, file_name))
	{
		printf("Write result file fail!\n");
		return false;
	}

	return true;
}

bool VendorConvert::write_delta()
{
	xml_node<> *node = root_conf->first_node("DataDelta", 0, false);
	if (!node)
		return true;

	xml_attribute<> *attribute = node->first_attribute("run", 0, false);
	if (!attribute || *(attribute->value()) != '1')
		return true;

	char base[512];
	attribute = node->first_attribute("base", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(base, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”VendorConvert.DataDelta.base内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}
	}
	else
		return false;

	char delta[512];
	char dir[512];
	attribute = node->first_attribute("delta", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(delta, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”VendorConvert.DataDelta.delta内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(dir, delta, 1024);
		create_dir(dir, 0755);
	}
	else
	{
		sprintf(delta, "%s/VendorDataDelta.e", conf_dir);
	}

	e_document_ex doc;
	if (!doc.parse_file(base))
	{
		printf("parse_file fail: %s\n", base);
		return false;
	}

	db_read.clear();
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);

	DDETable *ExchangeData_tb = db_read.find("ExchangeData");
	if (!ExchangeData_tb)
		return false;

	size_t size = ExchangeData_tb->size();
	char buffer[1024];
	const_char_map<ExchangeData*> ExchangeData_map; 
	for (size_t i = 0; i < size; ++i)
	{
		ExchangeData *data = reinterpret_cast<ExchangeData*>(ExchangeData_tb->at(i));
		sprintf(buffer, "%s:%s", data->code, data->vendor);
		char *key = ExchangeData_tb->allocate_string(buffer);
		ExchangeData_map[key] = data;
	}

	DDETable *ExchangeData_tb2 = db_result.find("ExchangeData");
	if (!ExchangeData_tb2)
		return false;

	size = ExchangeData_tb2->size();
	const_char_map<ExchangeData*> ExchangeData_map2; 
	for (size_t i = 0; i < size; ++i)
	{
		ExchangeData *data = reinterpret_cast<ExchangeData*>(ExchangeData_tb2->at(i));
		sprintf(buffer, "%s:%s", data->code, data->vendor);
		char *key = ExchangeData_tb2->allocate_string(buffer);
		ExchangeData_map2[key] = data;
	}

	DDETable *delete_tb = db_result.get("Delete", 0, "ExchangeData");
	DDETable *update_tb = db_result.get("Update", 0, "ExchangeData");
	for (auto it = ExchangeData_map.begin(); it != ExchangeData_map.end(); ++it)
	{
		ExchangeData *data = it->second;
		auto it2 = ExchangeData_map2.find(it->first);
		if (it2 == ExchangeData_map2.end())
		{
			delete_tb->push_back(reinterpret_cast<char*>(data));
		} 
		else
		{
			ExchangeData *data2 = it2->second;
			if (memcmp(data, data2, sizeof(ExchangeData)) != 0)
				update_tb->push_back(reinterpret_cast<char*>(data2));
		}
	}

	DDETable *new_tb = db_result.get("New", 0, "ExchangeData");
	for (auto it = ExchangeData_map2.begin(); it != ExchangeData_map2.end(); ++it)
	{
		ExchangeData *data = it->second;
		auto it2 = ExchangeData_map.find(it->first);
		if (it2 == ExchangeData_map.end())
			new_tb->push_back(reinterpret_cast<char*>(data));
	}

	db_result.remove(ExchangeData_tb2);

	if (!E2DDE::print_file(db_result, delta))
	{
		printf("Write result file fail!\n");
		return false;
	}

	db_result.push_back(ExchangeData_tb2);

	return true;
}

