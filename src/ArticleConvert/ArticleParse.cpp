#include "stdafx.h"
#include "ArticleParse.h"

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

ArticleParse::ArticleParse() : size_cat2(0)
{
}

ArticleParse::~ArticleParse()
{
	printf("二级目录最大长度：%lu\n", size_cat2);
}

bool ArticleParse::initialize(DDEDatabase &db_result)
{
	OE_tb = db_result.get("OE");
	if (!OE_tb)
		return false;

	OE2Model_tb = db_result.get("OE2Model");
	if (!OE2Model_tb)
		return false;

	Catalog_tb = db_result.get("Catalog");
	if (!Catalog_tb)
		return false;

	OE2Catalog_tb = db_result.get("OE2Catalog");
	if (!OE2Catalog_tb)
		return false;

	DDETable *Brand_tb = db_result.get("Brand");
	if (!Brand_tb)
		return false;

	size_t size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_map[data->brand_name_cn] = data;
		Brand_id_map[data->brand_id] = data;
	}

	size = OE_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE *data = reinterpret_cast<OE*>(OE_tb->at(i));
		Brand_OE_map[data->brand_id][data->oe_code] = data;
		OE_id_map[data->oe_id] = data;
	}

	size = OE2Model_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE2Model *data = reinterpret_cast<OE2Model*>(OE2Model_tb->at(i));
		OE2Model_id_map[data->oe_id].insert(data);
	}

	size = Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Catalog *data = reinterpret_cast<Catalog*>(Catalog_tb->at(i));
		if (data->parent_cat_id)
			Catalog2_map[data->cat_name].insert(data);
		else
			Catalog_map[data->cat_name] = data;
		Catalog_id_map[data->cat_id] = data;
	}

	size = OE2Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE2Catalog *data = reinterpret_cast<OE2Catalog*>(OE2Catalog_tb->at(i));
		OE2Catalog_id_map[data->oe_id].insert(data);
	}

	DDETable *EPCModel_tb = db_result.get("EPCModel");
	if (!EPCModel_tb)
		return false;

	size = EPCModel_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		EPCModel *data = reinterpret_cast<EPCModel*>(EPCModel_tb->at(i));
		EPCModel_map[data->epc_model_code] = data;
	}

	return true;
}

bool ArticleParse::parse_cat1()
{
	char *name = yunarticle->cat_name;
	if (*name == '\0')
		return false;

	Catalog *old = catalog;
	auto it = Catalog_map.find(name);
	if (it != Catalog_map.end())
	{
		catalog = it->second;
		if (catalog != old)
		{
			first_oe = true;
			option_code_prefix.clear();
			option_code.clear();
		}

		return true;
	}

	first_oe = true;
	option_code_prefix.clear();
	option_code.clear();
	catalog = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
	Catalog_tb->push_back(reinterpret_cast<char*>(catalog));
	strncpy_ex(catalog->cat_name, name, 48);
	Catalog_map[catalog->cat_name] = catalog;
	catalog->sort_order = atoi(yunarticle->cat_id);
	catalog->cat_id = static_cast<int32_t>(Catalog_tb->size());
	Catalog_id_map[catalog->cat_id] = catalog;

	//printf("\t创建新目录“%s”。\n", catalog->cat_name);
	return true;
}

bool ArticleParse::parse_cat2()
{
	char *name = yunarticle->cat_name2;
	if (*name == '\0')
	{
		printf("parse_cat2 fail 1\n");
		return false;
	}

	size_t size = strlen(yunarticle->cat_name2);
	if (size > size_cat2)
		size_cat2 = size;

	char buffer[48];
	strncpy_ex(buffer, name, 48);

	Catalog *old = catalog2;
	auto it = Catalog2_map.find(buffer);
	if (it != Catalog2_map.end())
	{
		set<Catalog*> &catalogs = it->second;
		for (auto it2 = catalogs.begin(); it2 != catalogs.end(); ++it2)
		{
			catalog2 = *it2;
			if (catalog2->parent_cat_id == catalog->cat_id)
			{
				if (catalog2 != old)
				{
					first_oe = true;
					option_code_prefix.clear();
					option_code.clear();
				}
				return true;
			}
		}
	}

	first_oe = true;
	option_code_prefix.clear();
	option_code.clear();
	catalog2 = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
	Catalog_tb->push_back(reinterpret_cast<char*>(catalog2));
	//strncpy_ex(catalog2->cat_name, name, 48);
	strcpy(catalog2->cat_name, buffer);
	Catalog2_map[catalog2->cat_name].insert(catalog2);
	char *pos = strchr(yunarticle->cat_id2, '_');
	if (pos)
		catalog2->sort_order = atoi(pos+1);
	else
		catalog2->sort_order = atoi(yunarticle->cat_id2);
	catalog2->cat_id = static_cast<int32_t>(Catalog_tb->size());
	catalog2->parent_cat_id = catalog->cat_id;
	Catalog_id_map[catalog2->cat_id] = catalog2;

	//printf("\t创建新目录“%s”。\n", catalog2->cat_name);
	return true;
}

bool ArticleParse::parse_article()
{
	first_oe = false;
	const_char_map<OE*> &OE_map = Brand_OE_map[model->brand_id];
	auto it = OE_map.find(yunarticle->code2);
	if (it != OE_map.end())
	{
		oe = it->second;
		if (check)
		{
			//if (oe->brand_id != model->brand_id)
			//	printf("\t配件“%s”品牌ID不同：%d %d。\n", oe->oe_code, oe->brand_id, model->brand_id);

			if (strcmp(oe->oe_name, yunarticle->descr) != 0)
				printf("\t配件“%s”名称不同：%s %s。\n", oe->oe_code, oe->oe_name, yunarticle->descr);

			if (strcmp(oe->info, yunarticle->type) != 0)
				printf("\t配件“%s”附加信息不同：%s %s。\n", oe->oe_code, oe->info, yunarticle->type);

			//if (strcmp(oe->image_url_1, yunarticle->remark9) != 0)
			//	printf("\t配件“%s”图片url不同：%s %s。\n", oe->oe_code, oe->image_url_1, yunarticle->remark9);

			//if (oe->item_num_1 != atoi(yunarticle->graph_num))
			//	printf("\t配件“%s”图编号不同：%d %d。\n", oe->oe_code, oe->item_num_1, atoi(yunarticle->graph_num));
		}

		return true;
	}

	oe = reinterpret_cast<OE*>(OE_tb->allocate());
	OE_tb->push_back(reinterpret_cast<char*>(oe));
	oe->oe_id = static_cast<int32_t>(OE_tb->size());
	oe->brand_id = model->brand_id;
	strncpy_ex(oe->oe_code, yunarticle->code2, 24);
	strncpy_ex(oe->oe_name, yunarticle->descr, 64);
	strncpy_ex(oe->info, yunarticle->type, 48);
	//strncpy_ex(oe->image_url_1, yunarticle->remark9, 80);
	//oe->item_num_1 = static_cast<uint16_t>(atoi(yunarticle->graph_num));
	OE_map[oe->oe_code] = oe;
	OE_id_map[oe->oe_id] = oe;
	//printf("\t创建新配件 %d “%s”“%s”。\n", oe->oe_id, oe->oe_code, oe->oe_name);

	return true;
}

bool ArticleParse::parse_oe2model()
{
	string full_option_code;
	if (!option_code_prefix.empty())
	{
		if (option_code.empty() || option_code_prefix == option_code)
		{
			full_option_code = option_code_prefix;
		}
		else
		{
			full_option_code = "(";
			full_option_code += option_code_prefix;
			full_option_code += ")&(";
			full_option_code += option_code;
			full_option_code += ")";
		}
	}
	else
		full_option_code = option_code;

	char buffer[160];
	strncpy(buffer, full_option_code.c_str(), 160);
	if (full_option_code.size() >= 160)
	{
		printf("%s;\t%s full_option_code >= 160;\t%lu\n", oe->oe_code, model->epc_model_code, full_option_code.size());
		buffer[159] = '\0';
		char *pos = strrchr(buffer, '&');
		if (pos)
			*pos = '\0';
		else
		{
			pos = strrchr(buffer, '|');
			if (pos)
				*pos = '\0';
		}
	}

	auto it = OE2Model_id_map.find(oe->oe_id);
	if (it != OE2Model_id_map.end())
	{
		set<OE2Model*> &oe2models = it->second;
		for (auto it2 = oe2models.begin(); it2 != oe2models.end(); ++it2)
		{
			oe2model = *it2;
			if (oe2model->epc_model_id == model->epc_model_id)
			{
				//if (full_option_code != oe2model->option_code)
				//	printf("%s;\t%s;\t%s;\t%s\n", oe->oe_code, model->epc_model_code, oe2model->option_code, full_option_code.c_str());
				if (strcmp(buffer, oe2model->option_code) == 0)
					return true;
			}
		}
	}

	oe2model = reinterpret_cast<OE2Model*>(OE2Model_tb->allocate());
	OE2Model_tb->push_back(reinterpret_cast<char*>(oe2model));
	oe2model->oe_id = oe->oe_id;
	oe2model->epc_model_id = model->epc_model_id;
	//strncpy_ex(oe2model->option_code, option_code.c_str(), 160);
	//strncpy_ex(oe2model->option_code, full_option_code.c_str(), 160);
	strcpy(oe2model->option_code, buffer);
	OE2Model_id_map[oe->oe_id].insert(oe2model);
	return true;
}

bool ArticleParse::parse_oe2catalog()
{
	auto it = OE2Catalog_id_map.find(oe->oe_id);
	if (it != OE2Catalog_id_map.end())
	{
		set<OE2Catalog*> &oe2catalogs = it->second;
		for (auto it2 = oe2catalogs.begin(); it2 != oe2catalogs.end(); ++it2)
		{
			oe2catalog = *it2;
			if (oe2catalog->cat_id == catalog2->cat_id)
				return true;
		}
	}

	oe2catalog = reinterpret_cast<OE2Catalog*>(OE2Catalog_tb->allocate());
	OE2Catalog_tb->push_back(reinterpret_cast<char*>(oe2catalog));
	oe2catalog->oe_id = oe->oe_id;
	oe2catalog->cat_id = catalog2->cat_id;
	OE2Catalog_id_map[oe->oe_id].insert(oe2catalog);
	return true;
}

bool ArticleParse::run(DDEDatabase &db_data, DDEDatabase &db_read, DDEDatabase &db_result, const char *data_dir, bool _check)
{
	check = _check;

	if (!initialize(db_result))
		return false;

	parse_dir(db_read, data_dir);
	read_catalog_map(db_data);
	read_oename(db_data);

	return true;
}

bool ArticleParse::parse_dir(DDEDatabase &db_read, const char *data_dir)
{
	char filename[1024];

	printf("开始处理目录：%s\n", data_dir);

#ifndef __linux
	struct _finddata_t file;	// 定义结构体变量
	sprintf(filename, "%s/*", data_dir);
	long handle=_findfirst(filename, &file);	// 查找所有文件
	if (handle == -1)
		return false;

	do
	{
		if (file.attrib & _A_SUBDIR)
		{
			if(strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
			{
				sprintf(filename, "%s/%s", data_dir, file.name);
				parse_dir(db_read, filename);
			}
		}
		else if (strstr(file.name, ".e"))
		{
			printf("开始处理文件：%s\n", file.name);
			sprintf(filename, "%s/%s", data_dir, file.name);
			parse_file(db_read, filename);
			printf("\n");
		}
	} while(!(_findnext(handle,&file)));

	_findclose(handle);
#else
	struct dirent *entry;
	struct stat statbuf;
	DIR *dp = opendir(data_dir);
	if (!dp)
		return false;

	while ((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				sprintf(filename, "%s/%s", data_dir, entry->d_name);
				parse_dir(db_read, filename);
			}
		}
		else
		{
			if (!strstr(entry->d_name, ".e"))
				continue;

			printf("开始处理文件：%s\n", entry->d_name);
			sprintf(filename, "%s/%s", data_dir, entry->d_name);
			parse_file(db_read, filename);
			printf("\n");
		}
	}

	closedir(dp);
#endif

	return true;
}

bool ArticleParse::parse_file(DDEDatabase &db_read, const char *data_file)
{
	e_document_ex doc;
	//DDEScheme *scheme = db_read.scheme();
	//DDETableScheme *table_scheme = scheme->find("YunArticle");
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

	e_node *node = db_read.scheme()->create_e_node(doc, "YunArticle");
	if (!node)
		return false;

	if (!doc.parse_node_file(node, data_file, 0, 1))
		return false;

	char model_code[256];
	get_file_name(model_code, data_file);
	char *pos = strrchr(model_code, '.');
	if (pos)
		*pos = '\0';

	pos = strchr(model_code, ' ');
	if (pos)
	{
		pos += 1;
		char *pos2 = strchr(pos, ' ');
		if (pos2)
			*pos2 = '\0';
	}
	else
		pos = model_code;

	auto it = EPCModel_map.find(pos);
	if (it == EPCModel_map.end())
	{
		printf("云车配车型查找失败: %s\n", pos);
		return false;
	}

	model = it->second;
	//Brand *brand = Brand_id_map[model->brand_id];
	//bool bBMW = (strcmp(brand->brand_code, "BM") == 0 || strcmp(brand->brand_code, "BMW") == 0);

	db_read.clear();
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);

	YunArticle_tb = db_read.get("YunArticle");
	if (!YunArticle_tb)
		return false;

	size_t size = YunArticle_tb->size();
	if (!size)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		yunarticle = reinterpret_cast<YunArticle*>(YunArticle_tb->at(i));
		//if (bBMW)
		//{
		//	int id = atoi(yunarticle->cat_id);
		//	if (id == 1 || id == 71 || id == 83 || id == 84 || id == 85 || id == 91)
		//		continue;
		//}

		if (!parse_cat1())
		{
			printf("\t一级目录解析失败！\n");
			continue;
		}

		if (!parse_cat2())
		{
			printf("\t二级目录解析失败！\n");
			continue;
		}

		if (*(yunarticle->descr) == '\0')
		{
			if (first_oe)
				option_code_prefix = option_code;

			option_code.clear();
			continue;
		}

		if (*(yunarticle->code2) == '\0')
		{
			if (strncmp(yunarticle->descr, "用于", 4) == 0)
			{
				option_code.clear();
			}
			else
			{
				pos = strchr(yunarticle->type, '=');
				if (pos)
				{
					*pos = '\0';
					trim_space(yunarticle->type);
					++pos;
					trim_space(pos);
					if (strncmp(pos, "否", 2) == 0)
						option_code += '!';

					option_code += yunarticle->type;
				}
				else if (!option_code.empty())
				{
					if (strncmp(yunarticle->descr, "和", 2) == 0)
						option_code += '&';
					else if (strncmp(yunarticle->descr, "或者", 4) == 0)
						option_code += '|';
				}
			}

			continue;
		}

		//if (atoi(yunarticle->graph_num) == 0)
		//	continue;
		if (*(yunarticle->graph_num) == '\0')
			continue;

		if (!parse_article())
		{
			printf("\t零件解析失败！\n");
			continue;
		}

		parse_oe2model();
		parse_oe2catalog();
	}

	return true;
}

void ArticleParse::read_catalog_map(DDEDatabase &db_data)
{
	DDETable *CatalogMap_tb = db_data.find("CatalogMap");
	if (!CatalogMap_tb)
		return;

	size_t size = CatalogMap_tb->size();
	if (!size)
		return;

	TimeConsume tc("目录标准化");
	Catalog_tb->remove_all_records();
	const_char_map<Catalog *> New_Catalog_map;
	const_char_map<Catalog *> New_Catalog2_map;
	const_char_map<Catalog *> New_Catalog3_map;
	Catalog_id_map.clear();

	Catalog *new_catalog = 0;
	Catalog *new_catalog2 = 0;
	Catalog *new_catalog3 = 0;
	map<int32_t, int32_t> id_id_map;
	for (size_t i = 0; i < size; ++i)
	{
		CatalogMap *data = reinterpret_cast<CatalogMap*>(CatalogMap_tb->at(i));
		if (*(data->epc_cat_name) == '\0' || *(data->epc_cat_name2) == '\0')
			continue;

		if (*(data->cat_name) != '\0')
		{
			auto it = New_Catalog_map.find(data->cat_name);
			if (it != New_Catalog_map.end())
			{
				if (new_catalog != it->second)
				{
					new_catalog = it->second;
					new_catalog2 = 0;
					new_catalog3 = 0;
				}
			}
			else
			{
				new_catalog = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
				Catalog_tb->push_back(reinterpret_cast<char*>(new_catalog));
				strcpy(new_catalog->cat_name, data->cat_name);
				New_Catalog_map[new_catalog->cat_name] = new_catalog;
				new_catalog->cat_id = static_cast<int32_t>(Catalog_tb->size());
				Catalog_id_map[new_catalog->cat_id] = new_catalog;
				new_catalog2 = 0;
				new_catalog3 = 0;
			}
		}

		if (!new_catalog)
		{
			printf("\t一级目录为空 %s！\n", data->cat_name);
			continue;
		}

		if (*(data->cat_name2) != '\0')
		{
			auto it = New_Catalog2_map.find(data->cat_name2);
			if (it != New_Catalog2_map.end())
			{
				if (new_catalog2 != it->second)
				{
					new_catalog2 = it->second;
					new_catalog3 = 0;
					if (new_catalog2->parent_cat_id != new_catalog->cat_id)
						printf("\t二级级目录“%s”所属的一级目录不同：%s %s。\n", data->cat_name2, Catalog_id_map[new_catalog2->parent_cat_id]->cat_name, new_catalog->cat_name);
				}
			}
			else
			{
				new_catalog2 = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
				Catalog_tb->push_back(reinterpret_cast<char*>(new_catalog2));
				strcpy(new_catalog2->cat_name, data->cat_name2);
				New_Catalog2_map[new_catalog2->cat_name] = new_catalog2;
				new_catalog2->cat_id = static_cast<int32_t>(Catalog_tb->size());
				Catalog_id_map[new_catalog2->cat_id] = new_catalog2;
				new_catalog2->parent_cat_id = new_catalog->cat_id;
				new_catalog3 = 0;
			}
		}

		if (!new_catalog2)
		{
			printf("\t二级目录为空或不存在：%s %s %s %s %s！\n", catalog->cat_name, data->cat_name2, data->cat_name3, data->epc_cat_name, data->epc_cat_name2);
			continue;
		}

		if (*(data->cat_name3) != '\0')
		{
			auto it = New_Catalog3_map.find(data->cat_name3);
			if (it != New_Catalog3_map.end())
			{
				new_catalog3 = it->second;
				if (new_catalog3->parent_cat_id != new_catalog2->cat_id)
					printf("\t三级级目录“%s”所属的二级目录不同：%s %s。\n", data->cat_name3, Catalog_id_map[new_catalog3->parent_cat_id]->cat_name, new_catalog2->cat_name);
			}
			else
			{
				new_catalog3 = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
				Catalog_tb->push_back(reinterpret_cast<char*>(new_catalog3));
				strcpy(new_catalog3->cat_name, data->cat_name3);
				New_Catalog_map[new_catalog3->cat_name] = new_catalog3;
				new_catalog3->cat_id = static_cast<int32_t>(Catalog_tb->size());
				new_catalog3->parent_cat_id = new_catalog2->cat_id;
				Catalog_id_map[new_catalog3->cat_id] = new_catalog3;
			}
		}

		auto it = Catalog_map.find(data->epc_cat_name);
		if (it == Catalog_map.end())
		{
			printf("\tEPC一级目录不存在：%s\n", data->epc_cat_name);
			continue;
		}

		catalog = it->second;

		auto it2 = Catalog2_map.find(data->epc_cat_name2);
		if (it2 == Catalog2_map.end())
		{
			printf("\tEPC二级目录不存在：%s\n", data->epc_cat_name2);
			continue;
		}

		catalog2 = 0;
		set<Catalog*> &catalogs = it2->second;
		for (auto it3 = catalogs.begin(); it3 != catalogs.end(); ++it3)
		{
			catalog2 = *it3;
			if (catalog2->parent_cat_id == catalog->cat_id)
				break;
			else
				catalog2 = 0;
		}

		if (!catalog2)
		{
			printf("\tEPC二级目录不存在：%s\t\t%s\n", data->epc_cat_name, data->epc_cat_name2);
			continue;
		}

		if (new_catalog3)
			id_id_map[catalog2->cat_id] = new_catalog3->cat_id;
		else
			id_id_map[catalog2->cat_id] = new_catalog2->cat_id;
	}

	size = OE2Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE2Catalog *data = reinterpret_cast<OE2Catalog*>(OE2Catalog_tb->at(i));
		auto it = id_id_map.find(data->cat_id);
		if (it == id_id_map.end())
			printf("目录“%d”未映射\n", data->cat_id);
		else
			data->cat_id = it->second;
	}
}

void ArticleParse::read_oename(DDEDatabase &db_data)
{
	DDETable *tb = db_data.find("OEData");
	if (!tb)
		return;

	size_t size = tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OEData *data = reinterpret_cast<OEData*>(tb->at(i));
		Brand *brand = Brand_map[data->brand];
		if (!brand)
			continue;

		const_char_map<OE*> &OE_map = Brand_OE_map[brand->brand_id];
		auto it = OE_map.find(data->oe_code);
		if (it != OE_map.end())
		{
			oe = it->second;
			strcpy(oe->oe_name, data->oe_name);
		}
	}
}

