#include "stdafx.h"
#include "VendorConvert.h"
#include "VendorParse.h"

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

void trim_unnecessary_space(char *buffer)
{
	char *pos = buffer;
	char *pos2 = buffer;
	while (*pos != '\0')
	{
		if (pos != pos2)
			*pos2 = *pos;

		pos2++;

		if (*pos == ' ')
		{
			while (*pos == ' ')
				pos++;
		}
		else
			pos++;
	}

	while (pos2 != pos)
	{
		*pos2 = '\0';
		pos2++;
	}

	//*pos2 = '\0';
}

VendorParse::VendorParse()
{
}

VendorParse::~VendorParse()
{
}

bool VendorParse::initialize()
{
	ExchangeData_tb = vc->db_result.get("ExchangeData");
	if (!ExchangeData_tb)
		return false;

	DDETable *OEData_tb = vc->db_result.get("OEData");
	if (!OEData_tb)
		return false;

	size_t size = OEData_tb->size();
	if (vc->trim_space)
	{
		for (size_t i = 0; i < size; ++i)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			char *buffer = OEData_tb->allocate_string(0, 23);
			memcpy(buffer, data->oe_code, 24);
			trim_space(buffer);
			OEData_map[data->brand][buffer] = data;
		}
	}
	else
	{
		for (size_t i = 0; i < size; ++i)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			OEData_map[data->brand][data->oe_code] = data;
		}
	}

	for (auto it = OEData_map.begin(); it != OEData_map.end(); ++it)
		printf("\t%s %lu\n", it->first, it->second.size());

	DDETable *Brand_tb = vc->db_result.get("Brand");
	if (!Brand_tb)
		return false;

	size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_map[data->brand_code] = data;
		Brand_cn_map[data->brand_name_cn] = data;
	}

	return true;
}

bool VendorParse::run(VendorConvert *_vc, const char *data_dir)
{
	vc = _vc;

	if (!initialize())
		return false;

	parse_dir(data_dir);

	printf("未归类品牌名称：\n");
	for (auto it = Brand_not_find_map.begin(); it != Brand_not_find_map.end(); ++it)
	{
		printf("\t%s\t%s\n", it->first.c_str(), it->second.c_str());
	}

	return true;
}

bool VendorParse::parse_dir(const char *data_dir)
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
				parse_dir(filename);
			}
		}
		else if (strstr(file.name, ".e"))
		{
			printf("\t开始处理文件：%s\n", file.name);
			sprintf(filename, "%s/%s", data_dir, file.name);
			parse_file(filename);
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
				parse_dir(filename);
			}
		}
		else
		{
			if (!strstr(entry->d_name, ".e"))
				continue;

			printf("\t开始处理文件：%s\n", entry->d_name);
			sprintf(filename, "%s/%s", data_dir, entry->d_name);
			parse_file(filename);
			printf("\n");
		}
	}

	closedir(dp);
#endif

	return true;
}

bool VendorParse::parse_file(const char *data_file)
{
	char name[256];
	get_file_name(name, data_file);
	char *pos = strrchr(name, '.');
	if (pos)
		*pos = '\0';

	pos = strchr(name, ' ');
	if (pos)
	{
		*pos = '\0';
		pos += 1;
	}
	else
		pos = name;

	Configure conf;
	auto it = vc->Configure_map.find(name);
	if (it != vc->Configure_map.end())
		conf = it->second;
	else
	{
		conf.run = 1;
		conf.type = 1;
		conf.mode = 1;
	}

	if (!conf.run)
		return true;

	e_document_ex doc;
	e_node *node = 0;
	if (conf.type == 2)
		node = vc->db_read.scheme()->create_e_node(doc, "OEData");
	else
		node = vc->db_read.scheme()->create_e_node(doc, "VendorData");

	if (!node)
		return false;

	if (!doc.parse_node_file(node, data_file, 0, 1))
		return false;

	vc->db_read.clear();
	E2DDE efile(vc->db_read.scheme());
	efile.parse(vc->db_read, doc, e_dde_parse_combine);

	if (conf.type == 2)
		parse_huali(conf, name);
	else
		parse(conf, pos, name);

	return true;
}

bool VendorParse::parse_huali(Configure &conf, const char *vendor_name)
{
	DDETable *OEData_tb = vc->db_read.get("OEData");
	if (!OEData_tb)
		return false;

	const_char_map<ExchangeData*> exchange_map;
	Brand *bmw = Brand_cn_map["宝马"];
	Brand *mini = Brand_cn_map["迷你"];
	Brand *benz = Brand_cn_map["奔驰"];
	char buffer[24];
	size_t size = OEData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
		size_t len = strlen(data->oe_code);
		if (len < 3)
		{
			printf("\t\t无效配件：%s %s\n", data->oe_code, data->oe_name);
			continue;
		}

		Brand *oe_brand = benz;
		if (data->oe_code[2] == ' ')
			oe_brand = bmw;

		Brand *brand = 0;
		ExchangeData *exchange = reinterpret_cast<ExchangeData*>(ExchangeData_tb->allocate());
		strncpy_ex(exchange->oe_code, data->oe_code, 24);
		trim_unnecessary_space(exchange->oe_code);
		if (isalpha(data->oe_code[len-1]))
		{
			auto it2 = Brand_cn_map.find(data->brand);
			if (it2 == Brand_cn_map.end())
			{
				strupr(data->brand);
				auto it3 = Brand_map.find(data->brand);
				if (it3 != Brand_map.end())
					brand = it3->second;
			}
			else
				brand = it2->second;

			if (!brand)
			{
				printf("\t\t配件“%s”品牌名称无效：%s\n", data->oe_name, data->brand);
				Brand_not_find_map[data->brand] = data->brand;
				continue;
			}

			strncpy_ex(exchange->oe_code, data->oe_code, 24);
			len -= 2;
			while (len)
			{
				if (isalpha(exchange->oe_code[len]))
					len--;
				else
				{
					if (exchange->oe_code[len] == ' ')
						exchange->oe_code[len] = '\0';
					else
						exchange->oe_code[len+1] = '\0';
					break;
				}
			}
		}
		else
		{
			brand = oe_brand;
		}

		auto it = exchange_map.find(data->oe_code);
		if (it != exchange_map.end())
		{
			ExchangeData *exchange2 = it->second;
			if (strcmp(brand->brand_name_cn, exchange2->brand_name_cn) == 0)
			{
				if (vc->repeat_warning)
					printf("\t\t重复配件：%s %s %s\n", data->oe_code, brand->brand_name_cn, data->oe_name);
				continue;
			}
		}

		bool trim_space2 = vc->trim_space;
		if (oe_brand == benz)
		{
			if (!isalpha(exchange->oe_code[0]))
			{
				memmove(&(exchange->oe_code[2]), exchange->oe_code, 22);
				exchange->oe_code[0] = 'A';
				exchange->oe_code[1] = ' ';
			}
		}
		else
			trim_space2 = true;

		char *code = exchange->oe_code;
		if (trim_space2)
		{
			memcpy(buffer, exchange->oe_code, 24);
			trim_space(buffer);
			code = buffer;
		}

		bool find = false;
		const_char_map<OEData*> &oedatas = OEData_map[oe_brand->brand_name_cn];
		auto itoe = oedatas.find(code);
		if (itoe == oedatas.end())
		{
			if (oe_brand == bmw)
			{
				const_char_map<OEData*> &oedatas2 = OEData_map[mini->brand_name_cn];
				itoe = oedatas2.find(code);
				if (itoe != oedatas2.end())
				{
					find = true;
					oe_brand = mini;
					if (brand == bmw)
						brand = mini;
				}
			}
		}
		else
			find = true;

		if (!find)
		{
			if (vc->oe_warning)
				printf("\t\t配件对应的OE#不存在：'%s' '%s' %s %s\n", data->oe_code, exchange->oe_code, data->brand, data->oe_name);
			continue;
		}

		exchange->price_4s = itoe->second->price;
		strncpy_ex(exchange->code, data->oe_code, 24);
		strncpy_ex(exchange->brand_name_cn, brand->brand_name_cn, 16);
		exchange_map[exchange->code] = exchange;
		if (conf.mode == 2)
		{
			if (oe_brand != brand)
				continue;
		}
		else if (conf.mode == 3)
		{
			if (oe_brand == brand)
				continue;
		}

		strncpy_ex(exchange->brand_name_cn, brand->brand_name_cn, 16);
		strncpy_ex(exchange->am_code, data->oe_code, 24);
		strncpy_ex(exchange->name, data->oe_name, 64);
		strncpy_ex(exchange->oe_brand_name_cn, oe_brand->brand_name_cn, 16);
		strncpy_ex(exchange->oe_brand_code, oe_brand->brand_code, 16);
		strcpy(exchange->vendor, vendor_name);
		exchange->price = data->price;

		ExchangeData_tb->push_back(reinterpret_cast<char*>(exchange));
	}

	return true;
}

bool VendorParse::parse(Configure &conf, const char *brand_name, const char *vendor_name)
{
	DDETable *VendorData_tb = vc->db_read.get("VendorData");
	if (!VendorData_tb)
		return false;

	auto it = Brand_cn_map.find(brand_name);
	if (it == Brand_cn_map.end())
	{
		printf("\t\t品牌名称无效：%s\n", brand_name);
		return false;
	}

	Brand *oe_brand = it->second;
	it = Brand_cn_map.find("奔驰");
	if (it == Brand_cn_map.end())
	{
		printf("\t\t品牌名称无效：奔驰\n");
		return false;
	}

	Brand *benz = it->second;

	char buffer[24];
	const_char_map<ExchangeData*> exchange_map;
	size_t size = VendorData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		VendorData *data = reinterpret_cast<VendorData*>(VendorData_tb->at(i));
		const char *code = data->code;
		if (*code == '\0')
			code = data->oe_code;

		auto it = exchange_map.find(code);
		if (it != exchange_map.end())
		{
			ExchangeData *exchange2 = it->second;
			const char *brand_cn = data->brand_name_cn;
			if (*brand_cn == '\0')
				brand_cn = brand_name;

			if (strcmp(brand_cn, exchange2->brand_name_cn) == 0)
			{
				if (vc->repeat_warning)
					printf("\t\t重复配件：%s %s %s\n", code, brand_cn, data->name);
				continue;
			}
		}

		Brand *brand = 0;
		if (strcmp(data->type, "原厂") == 0)
		{
			if (vc->import_check)
			{
				if (*(data->brand_name_cn) != '\0' && strcmp(data->brand_name_cn, brand_name) != 0)
				{
					printf("\t\t原厂件“%s”品牌名称不一致：%s %s\n", data->name, data->brand_name_cn, brand_name);
					continue;
				}
			}

			brand = oe_brand;
		}
		else/* if (strcmp(data->type, "原厂配套品牌") == 0)*/
		{
			if (vc->import_check)
			{
				if (*(data->brand_name_cn) != '\0' && strcmp(data->brand_name_cn, brand_name) == 0)
				{
					printf("\t\t%s件“%s”品牌与原厂品牌相同：%s %s\n", data->type, data->name, data->brand_name_cn, brand_name);
					continue;
				}
			}

			auto it = Brand_cn_map.find(data->brand_name_cn);
			if (it == Brand_cn_map.end())
			{
				strupr(data->brand_code);
				auto it2 = Brand_map.find(data->brand_code);
				if (it2 != Brand_map.end())
					brand = it2->second;
			}
			else
				brand = it->second;

			if (!brand)
			{
				printf("\t\t配件“%s”品牌名称无效：%s %s\n", data->name, data->brand_name_cn, data->brand_code);
				Brand_not_find_map[data->brand_name_cn] = data->brand_code;
				continue;
			}
		}

		ExchangeData *exchange = reinterpret_cast<ExchangeData*>(ExchangeData_tb->allocate());
		if (*(data->oe_code) != '\0')
			strcpy(exchange->oe_code, data->oe_code);
		else
			strcpy(exchange->oe_code, data->code);

		strupr(exchange->oe_code);
		char *pos = strchr(exchange->oe_code, '*');
		if (pos)
			*pos = '\0';

		//if (brand != oe_brand)
		//{
		//	pos = strchr(exchange->oe_code, '-');
		//	if (pos)
		//		*pos = '\0';
		//}

		pos = strchr(exchange->oe_code, '-');
		if (pos)
		{
			if (brand == oe_brand)
			{
				auto it = Brand_map.find(pos+1);
				if (it != Brand_map.end())
				{
					brand = it->second;
					*pos = '\0';
				}
			}
			else
				*pos = '\0';
		}

		if (oe_brand == benz)
		{
			if (!isalpha(exchange->oe_code[0]))
			{
				memmove(&(exchange->oe_code[2]), exchange->oe_code, 22);
				exchange->oe_code[0] = 'A';
				exchange->oe_code[1] = ' ';
			}
			else
			{
				if (exchange->oe_code[0] == 'A')
				{
					if (exchange->oe_code[1] != ' ')
					{
						memmove(&(exchange->oe_code[2]), &(exchange->oe_code[1]), 22);
						exchange->oe_code[1] = ' ';
					}
				}
			}
		}

		if (*(data->code) == '\0')
			strcpy(exchange->code, data->oe_code);
		else
			strcpy(exchange->code, data->code);

		exchange_map[exchange->code] = exchange;
		if (conf.mode == 2)
		{
			if (strcmp(brand_name, brand->brand_name_cn) != 0)
				continue;
		}
		else if (conf.mode == 3)
		{
			if (strcmp(brand_name, brand->brand_name_cn) == 0)
				continue;
		}

		trim_unnecessary_space(exchange->oe_code);

		if (strcmp(data->type, "保养件") != 0)
		{
			bool trim_space2 = vc->trim_space;
			if (strcmp(oe_brand->brand_code, "BMW") == 0)
				trim_space2 = true;

			const_char_map<OEData*> &oedatas = OEData_map[oe_brand->brand_name_cn];
			code = exchange->oe_code;
			if (trim_space2)
			{
				memcpy(buffer, exchange->oe_code, 24);
				trim_space(buffer);
				code = buffer;
			}

			auto itoe = oedatas.find(code);
			if (itoe == oedatas.end())
			{
				if (vc->oe_warning)
					printf("\t\t配件对应的OE#不存在\t%s\t%s\t%s\t%s\n", exchange->code, exchange->oe_code, data->brand_name_cn, data->name);
				continue;
			}

			exchange->price_4s = itoe->second->price;
			if (*(data->name) == '\0')
				strncpy_ex(exchange->name, itoe->second->oe_name, 64);
		}

		//if (data->price_4s)
		//	exchange->price_4s = data->price_4s;

		strncpy_ex(exchange->brand_name_cn, brand->brand_name_cn, 16);
		strncpy_ex(exchange->brand_code, brand->brand_code, 16);
		if (*(data->am_code) == '\0')
			strncpy_ex(exchange->am_code, exchange->oe_code, 24);
		else
			strncpy_ex(exchange->am_code, data->am_code, 24);

		if (*(data->name) != '\0')
			strncpy_ex(exchange->name, data->name, 64);
		strncpy_ex(exchange->oe_brand_name_cn, brand_name, 16);
		strncpy_ex(exchange->oe_brand_code, oe_brand->brand_code, 16);
		strcpy(exchange->vendor, vendor_name);
		exchange->price = data->price;
		if (!exchange->price)
			exchange->price = data->price_cost;
		exchange->stock = data->stock;
		if (vc->price_warning && exchange->price_4s && exchange->price_4s < exchange->price)
			printf("\t\t4S价格低于销售价格\t%s\t%s\t%s\t%f\t%f\n", vendor_name, exchange->code, exchange->name, exchange->price_4s, exchange->price);

		ExchangeData_tb->push_back(reinterpret_cast<char*>(exchange));
	}

	return true;
}

