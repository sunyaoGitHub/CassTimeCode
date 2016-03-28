#include "stdafx.h"
#include "JMConvert.h"
#include "JMParse.h"

JMConvert::JMConvert() : db_data(0), db_read(0), db_result(0)
{
}

JMConvert::~JMConvert()
{
}

bool JMConvert::run(const char *data_file, const char *data_dir, const char *map_file, const char *vendor_file, const char *_conf_file)
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
			printf("配置文件“%s”JMConvert.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
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

	db_data.scheme(&scheme);
	db_data.m_name = "data";
	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_data(data_file))
	{
		printf("读取配件数据失败！\n");
		return false;
	}

	if (!read_vendor(vendor_file))
	{
		printf("读取经销商数据失败！\n");
		return false;
	}

	if (!read_map(map_file))
	{
		printf("读取目录映射数据失败！\n");
		return false;
	}

	if (!read_dir(data_dir))
	{
		printf("生成目录词库失败！\n");
		return false;
	}

	if (!write_result())
	{
		printf("结果保存失败！\n");
		return false;
	}

	return true;
}

// 读取配置文件
bool JMConvert::read_configure()
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

	root_conf = xml_conf.first_node("JMConvert", 0, false);
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

	get_file_dir(conf_dir, conf_file, 1024);
	return true;
}

bool JMConvert::read_data(const char *data_file)
{
	TimeConsume tc("读取配件数据");

	e_document_ex doc;
	if (!doc.parse_file(data_file))
	{
		printf("读取数据文件失败: %s\n", data_file);
		return false;
	}

	E2DDE efile(db_data.scheme());
	efile.parse(db_data, doc, e_dde_parse_combine);

	return true;
}

bool JMConvert::read_vendor(const char *vendor_file)
{
	TimeConsume tc("读取经销商数据");

	e_document_ex doc;
	if (!doc.parse_file(vendor_file))
	{
		printf("读取经销商数据文件失败: %s\n", vendor_file);
		return false;
	}

	E2DDE efile(db_data.scheme());
	efile.parse(db_data, doc, e_dde_parse_combine);

	return true;
}

bool JMConvert::read_map(const char *map_file)
{
	e_document_ex doc;
	//DDEScheme *scheme = db_data.scheme();
	//DDETableScheme *table_scheme = scheme->find("CatalogMap");
	//if (!table_scheme)
	//	return false;

	//const memory_pool_table<DDEColumnScheme> &columns = table_scheme->columns();
	//e_node *node = doc.allocate_node(e_node_table, columns.size(), table_scheme->name());
	//doc.append_node(node);

	//for (size_t i = 0; i < columns.size(); ++i)
	//{
	//	DDEColumnScheme *column = columns[i];
	//	if (!column)
	//		continue;

	//	node->column_name(i, column->name());
	//	node->column_desc(i, column->desc());
	//}

	e_node *node = db_data.scheme()->create_e_node(doc, "CatalogMap");
	if (!node)
		return false;
	if (!doc.parse_node_file(node, map_file, 0, 1))
		return false;

	E2DDE efile(db_data.scheme());
	efile.parse(db_data, doc, e_dde_parse_combine);

	return true;
}

bool JMConvert::read_dir(const char *data_dir)
{
	TimeConsume tc("配件目录词库解析");
	JMParse jmp;
	if (!jmp.run(db_data, db_read, db_result, data_dir, import_check))
	{
		printf("配件目录词库解析失败！\n");
		return false;
	}

	return true;
}

bool JMConvert::write_result()
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
			printf("配置文件“%s”JMConvert.ResultData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(conf_dir, file_name, 1024);
		create_dir(conf_dir, 0755);
	}
	else
	{
		sprintf(file_name, "%s/JMData.e", conf_dir);
	}

	if (!E2DDE::print_file(db_result, file_name))
	{
		printf("Write result file fail!\n");
		return false;
	}

	return true;
}

