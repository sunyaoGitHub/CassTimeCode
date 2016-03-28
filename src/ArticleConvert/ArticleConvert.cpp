#include "stdafx.h"
#include "ArticleConvert.h"
#include "ArticleParse.h"

ArticleConvert::ArticleConvert() : db_data(0), db_read(0), db_result(0)
{
}

ArticleConvert::~ArticleConvert()
{
}

/************************************************************************/
/*                data_dir:数据文件目录           _conf_file：配置文件全路径                                              */
/************************************************************************/
bool ArticleConvert::run(const char *data_dir, const char *_conf_file)
{
	conf_file = _conf_file;
	if (!read_configure())//读配置文件完成一些初始化工作 及获取配置文件目录
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”ArticleConvert.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
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
	if (!scheme.parse_file(scheme_file))//解析Vehicle.e格式文件
	{
		printf("配置文件“%s”不存在或错误，异常退出！\n", scheme_file);
		return false;
	}
	/*分别指定数据字典并命名*/
	db_data.scheme(&scheme);
	db_data.m_name = "data";
	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_base())//读基础数据 到db_result中
	{
		printf("基础数据读入失败！\n");
		return false;
	}

	read_catalog_map();//获取目录映射
	read_oename();//获取OE名字

	if (!read_model_map())
	{
		printf("车型映射数据读入失败！\n");
		return false;
	}

	if (!read_dir(data_dir))
	{
		printf("配件数据读入失败！\n");
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
bool ArticleConvert::read_configure()
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

	root_conf = xml_conf.first_node("ArticleConvert", 0, false);
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

	get_file_dir(conf_dir, conf_file, 1024);//获取配置文件所在目录
	return true;
}

bool ArticleConvert::read_base()
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
	if (!doc.parse_file(file_name))//解析基础数据文件
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_result.scheme());//使用数据字典初始化
	efile.parse(db_result, doc, e_dde_parse_combine);//将解析后的E表合并到库中
	return true;
}

bool ArticleConvert::read_model_map()
{
	TimeConsume tc("读取车型映射数据");

	xml_node<> *node = root_conf->first_node("ModelMap", 0, false);
	if (!node)
	{
		printf("配置文件“%s”ArticleConvert.ModelMap缺失，异常退出！\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = node->first_attribute("file", 0, false);
	if (!attribute)
	{
		printf("配置文件“%s”ArticleConvert.ModelMap.file缺失，异常退出！\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("配置文件“%s”ArticleConvert.ModelMap.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	e_node *data = db_read.scheme()->create_e_node(doc, "Level2YunData");
	if (!data)
		return false;
	if (!doc.parse_node_file(data, file_name, 0, 1))
		return false;

	db_read.clear();
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);

	DDETable *Level2YunData_tb = db_read.find("Level2YunData");
	if (!Level2YunData_tb)
		return false;

	DDETable *Brand_tb = db_result.get("Brand");
	if (!Brand_tb)
		return false;

	const_char_map<Brand *> Brand_map;
	size_t size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_map[data->brand_code] = data;//使用品牌编码建立映射
	}

	DDETable *EPCModel_tb = db_result.get("EPCModel");
	if (!EPCModel_tb)
		return false;

	const_char_map<EPCModel *> EPCModel_map;
	size = EPCModel_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		EPCModel *data = reinterpret_cast<EPCModel*>(EPCModel_tb->at(i));
		EPCModel_map[data->epc_model_code] = data;//使用车型编码建立映射
	}

	DDETable *ModelTransfer_tb = db_result.get("ModelTransfer");
	if (!ModelTransfer_tb)
		return false;

	const_char_map<ModelTransfer *> ModelTransfer_map;
	size = ModelTransfer_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		ModelTransfer *data = reinterpret_cast<ModelTransfer*>(ModelTransfer_tb->at(i));
		ModelTransfer_map[data->model_code] = data;//使用车型编码建立映射
	}

	size = Level2YunData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Level2YunData *data = reinterpret_cast<Level2YunData*>(Level2YunData_tb->at(i));
		//printf("正在处理力洋车型：%s\n", data->level_code);

		if (*(data->yun_code) == '\0')
		{
			printf("\tEPC车型为空：%s\n", data->level_code);
			continue;
		}

		auto it = Brand_map.find(data->brand_code);
		if (it == Brand_map.end())
			continue;

		Brand *brand = it->second;

		EPCModel *yunmodel = nullptr;
		auto it2 = EPCModel_map.find(data->yun_code);
		if (it2 == EPCModel_map.end())
		{
			yunmodel = reinterpret_cast<EPCModel*>(EPCModel_tb->allocate());
			EPCModel_tb->push_back(reinterpret_cast<char*>(yunmodel));
			strncpy_ex(yunmodel->epc_model_code, data->yun_code, 20);
			strncpy_ex(yunmodel->epc_model_name, data->name, 40);
			EPCModel_map[yunmodel->epc_model_code] = yunmodel;
			yunmodel->epc_model_id = static_cast<int32_t>(EPCModel_tb->size());
			yunmodel->brand_id = brand->brand_id;
		}
		else
		{
			yunmodel = it2->second;
			if (import_check)
			{
				if (yunmodel->brand_id != brand->brand_id)
					printf("\t云车配车型“%s”品牌不同：%d %d。\n", data->yun_code, yunmodel->brand_id, brand->brand_id);

				if (strcmp(yunmodel->epc_model_name, data->name) != 0)
					printf("\t云车配车型“%s”名称不同：%s %s。\n", data->yun_code, yunmodel->epc_model_name, data->name);
			}
		}

		auto it3 = ModelTransfer_map.find(data->level_code);
		if (it3 == ModelTransfer_map.end())
		{
			ModelTransfer *level2yun = reinterpret_cast<ModelTransfer*>(ModelTransfer_tb->allocate());
			ModelTransfer_tb->push_back(reinterpret_cast<char*>(level2yun));
			strncpy_ex(level2yun->model_code, data->level_code, 20);
			strncpy_ex(level2yun->epc_model_code, data->yun_code, 20);
			ModelTransfer_map[level2yun->model_code] = level2yun;
			level2yun->epc_model_id = yunmodel->epc_model_id;
		}
		else
		{
			ModelTransfer *level2yun = it3->second;
			if (import_check)
			{
				if (level2yun->epc_model_id != yunmodel->epc_model_id)
				{
					level2yun->epc_model_id = yunmodel->epc_model_id;
					printf("\t力洋车型“%s”对应的云车配车型不同：%d %d。\n", data->level_code, level2yun->epc_model_id, yunmodel->epc_model_id);
				}
			}
		}
	}

	return true;
}

bool ArticleConvert::read_dir(const char *data_dir)
{
	TimeConsume tc("配件解析");
	ArticleParse ap;
	if (!ap.run(db_data, db_read, db_result, data_dir, import_check))
	{
		printf("配件解析失败！\n");
		return false;
	}

	return true;
}

bool ArticleConvert::write_result()
{
	xml_node<> *result_data = root_conf->first_node("ResultData", 0, false);
	if (!result_data)
		return true;

	xml_attribute<> *attribute = result_data->first_attribute("run", 0, false);
	if (!attribute || *(attribute->value()) != '1')
		return true;

	char file_name[1024];
	attribute = result_data->first_attribute("file", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(file_name, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”ArticleConvert.ResultData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(conf_dir, file_name, 1024);
		create_dir(conf_dir, 0755);
	}
	else
	{
		sprintf(file_name, "%s/ArticleData.e", conf_dir);
	}

	if (!E2DDE::print_file(db_result, file_name))
	{
		printf("Write result file fail!\n");
		return false;
	}

	return true;
}

bool ArticleConvert::read_catalog_map()
{
	xml_node<> *node = root_conf->first_node("CatalogMap", 0, false);
	if (!node)
		return true;

	xml_attribute<> *attribute = node->first_attribute("run", 0, false);
	if (!attribute || *(attribute->value()) == '0')
		return true;
	
	attribute = node->first_attribute("file", 0, false);
	if (!attribute)
	{
		printf("配置文件“%s”ArticleConvert.CatalogMap.file缺失，异常退出！\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("配置文件“%s”ArticleConvert.CatalogMap.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	e_node *node2 = db_read.scheme()->create_e_node(doc, "CatalogMap");
	if (!node2)
		return false;
	if (!doc.parse_node_file(node2, file_name, 0, 1))
		return false;

	E2DDE efile(db_data.scheme());
	efile.parse(db_data, doc, e_dde_parse_combine);

	return true;
}

bool ArticleConvert::read_oename()
{
	xml_node<> *node = root_conf->first_node("OEName", 0, false);
	if (!node)
		return true;

	xml_attribute<> *attribute = node->first_attribute("run", 0, false);
	if (!attribute || *(attribute->value()) == '0')
		return true;

	attribute = node->first_attribute("file", 0, false);
	if (!attribute)
	{
		printf("配置文件“%s”ArticleConvert.OEName.file缺失，异常退出！\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("配置文件“%s”ArticleConvert.OEName.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	e_node *node2 = db_read.scheme()->create_e_node(doc, "OEData");
	if (!node2)
		return false;
	if (!doc.parse_node_file(node2, file_name, 0, 1))
		return false;

	E2DDE efile(db_data.scheme());
	efile.parse(db_data, doc, e_dde_parse_combine);

	return true;
}
