#include "stdafx.h"
#include "OEUnify.h"

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

OEUnify::OEUnify() : db_read(0), db_result(0)
{
}

OEUnify::~OEUnify()
{
}
/************************************************************************/
/*                 data_dir:数据文件目录         _conf_file：配置文件.xml文件                                             */
/************************************************************************/
bool OEUnify::run(const char *data_dir, const char *_conf_file)//data_dir：数据文件绝对路径，_conf_file配置文件绝对路径
{
	conf_file = _conf_file;
	if (!read_configure())//读配置文件
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);//获取属性scheme值
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))//获取Vehicle.e文件绝对路径
		{
			printf("配置文件“%s”OEUnify.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}
	}
	else  //使用默认Vehicle.e文件
	{
		char exe_dir[1024];
		get_exe_path(scheme_file, 1024);
		get_file_dir(exe_dir, scheme_file, 1024);
		get_absolute_path(scheme_file, "../../common/conf/DDE/Vehicle.e", exe_dir);
	}

	DDEScheme scheme; //数据字典
	if (!scheme.parse_file(scheme_file))//解析Vehicle.e配置文件 定义数据格式
	{
		printf("配置文件“%s”不存在或错误，异常退出！\n", scheme_file);
		return false;
	}
	/*两份数据库方便处理*/
	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_base())//读基础数据
	{
		printf("基础数据读入失败！\n");
		return false;
	}

	if (!read_dir(data_dir))//读配件数据
	{
		printf("配件数据读入失败！\n");
		return false;
	}

	//unify();

	if (!write_result())//保存结果
	{
		printf("结果保存失败！\n");
		return false;
	}
	else
	{
		printf("处理完成保存结果成功！\n");
	}

	return true;
}

// 读取配置文件
bool OEUnify::read_configure()
{
	try
	{
		xml_conf.parse_file<0>(conf_file);//解析xml格式配置文件
	}
	catch (parse_error &e)
	{
		printf("配置文件“%s”解析错误: %s，位置：%s，异常退出！\n", conf_file, e.what(), e.where<char>());
		return false;
	}

	root_conf = xml_conf.first_node("OEUnify", 0, false);//获取xml根元素OEUnify
	if (!root_conf)
	{
		printf("配置文件“%s”内容无效，异常退出！\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = root_conf->first_attribute("echo_time", 0, false);//获取echo_time属性
	if (attribute && ((*attribute->value()) == '1'))
		TimeConsume::s_mode = 1;
	else
		TimeConsume::s_mode = 0;

	import_check = false;
	attribute = root_conf->first_attribute("import_check", 0, false);//获取import_check属性
	if (attribute && ((*attribute->value()) == '1'))
		import_check = true;

	get_file_dir(conf_dir, conf_file, 1024);//获取配置文件目录
	return true;
}

bool OEUnify::read_base() //读基础数据
{
	TimeConsume tc("读取基础数据");
	xml_node<> *base_data = root_conf->first_node("BaseData", 0, false);//读BaseData 元素
	if (!base_data)
		return false;

	char file_name[1024];
	xml_attribute<> *attribute = base_data->first_attribute("file", 0, false); //获取file属性值
	if (!attribute)
		return false;

	if (!get_absolute_path(file_name, attribute->value(), conf_dir))//获取BaseData.e绝对路径
	{
		printf("配置文件“%s”OEUnify.BaseData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	if (!doc.parse_file(file_name))//解析BaseData.e文件到E表中
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);//将基本数据用E表解析按配置文件中的格式合并到数据库中
	
	DDETable *Brand_tb = db_read.get("Brand");//获取数据库中Brand表
	if (!Brand_tb)
		return false;

	size_t size = Brand_tb->size();  //表大小
	for (size_t i = 0; i < size; ++i)  
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i)); //强制转换成标准结构
		Brand_code_map[data->brand_code] = data;		//建立每一组数据用品牌编码索引
		Brand_name_map[data->brand_name_cn] = data;  //建立每一组数据用品牌中文名索引
	}

	return true;
}

bool OEUnify::read_dir(const char *data_dir)
{
	char filename[1024];

	printf("开始处理目录：%s\n", data_dir);

#ifndef __linux
	struct _finddata_t file;	// 定义结构体变量
	sprintf(filename, "%s/*", data_dir);
	long handle=_findfirst(filename, &file);	// 查找所有数据文件
	if (handle == -1)
	{
		printf("未找到数据文件!\n");
		return false;
	}

	do
	{
		if (file.attrib & _A_SUBDIR)  //如果搜到的文件是一个目录
		{
			if(strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
			{
				sprintf(filename, "%s/%s", data_dir, file.name);
				read_dir(filename);
			}
		}
		else if (strstr(file.name, ".e"))//如果存在.e文件
		{
			printf("开始处理文件：%s\n文件处理中请等待......", file.name);
			sprintf(filename, "%s/%s", data_dir, file.name);//保存目录及文件名
			read_file(filename);
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
				read_dir(filename);
			}
		}
		else
		{
			if (!strstr(entry->d_name, ".e"))
				continue;

			printf("开始处理文件：%s\n", entry->d_name);
			sprintf(filename, "%s/%s", data_dir, entry->d_name);
			read_file(filename);
			printf("\n");
		}
	}

	closedir(dp);
#endif

	return true;
}

bool OEUnify::read_file(const char *data_file) //读文件
{
	e_document_ex doc;
	//const_char_map<OEData*> oedata_map;
	//DDEScheme *scheme = db_read.scheme();
	//DDETableScheme *table_scheme = scheme->find("OEData");
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

	e_node *node = db_read.scheme()->create_e_node(doc, "OEData");//创建E表OEData 并将该表加入doc
	if (!node)
		return false;
	if (!doc.parse_node_file(node, data_file, 0, 1))//根据给定的标志解析文件到指定的E表中
		return false;

	char name[128];
	get_file_name(name, data_file);//获取数据文件名称
	char *pos = strrchr(name, ' ');//在末尾增加\0 字符串结束符
	if (pos)
		*pos = '\0';

	DDETable *OEData_tb = db_read.get("OEData");//获取数据表
	if (!OEData_tb)
		return false;

	//DDETable *OEDataEx_tb = db_result.get("OEDataEx");
	//if (!OEDataEx_tb)
	//	return false;

	DDETable *OEData2_tb = db_result.get("OEData");//获取数据表2
	if (!OEData2_tb)
		return false;

	OEData_tb->clear_record();   //清除记录
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);  //将E表合并到数据库中

	size_t size = OEData_tb->size();
	if (!size)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i)); //获取每一项
		trim_space(data->oe_code);  //去除空白字符
		if (*(data->oe_code) == '\0')
			continue;

		Brand *brand = 0;
		auto it = Brand_name_map.find(data->brand);//根据品牌名字在基础数据文件表中查找
		if (it == Brand_name_map.end()) //未找到
		{
			strupr(data->brand); //将品牌编码转换成大写再找一次
			auto it2 = Brand_code_map.find(data->brand); 
			if (it2 == Brand_code_map.end())//未找到则无法识别
			{
				printf("\t\t品牌名称无法识别：%s\n", data->brand);
				continue;
			}

			brand = it2->second;
		}
		else  //找到了则获取该条在基础数据文件中对应的数据(指针，原存储的为指针)
			brand = it->second;

		strupr(data->oe_code);//将OE号改成大写
		//const_char_map<OEDataEx*> &oedata_map = brand_oedata_map[brand];



		const_char_map<OEData*> &oedata_map = brand_oedata_map[brand];//建立引用 每次往MAP中添加数据 确保不重复
		auto it3 = oedata_map.find(data->oe_code);//查找OE号
		//OEDataEx *data_ex = 0;
		OEData *data2 = 0;
		if (it3 == oedata_map.end())//未找到OE号
		{
			//data_ex = reinterpret_cast<OEDataEx*>(OEDataEx_tb->allocate());
			//OEDataEx_tb->push_back(reinterpret_cast<char*>(data_ex));
			//strcpy(data_ex->code, data->code);
			//strcpy(data_ex->brand, brand->name_cn);
			//oedata_map[data_ex->code] = data_ex;
	
			data2 = reinterpret_cast<OEData*>(OEData2_tb->allocate());//分配一个数据表结构
			OEData2_tb->push_back(reinterpret_cast<char*>(data2)); //添加一个新元素到OEData2_tb中
			strcpy(data2->oe_code, data->oe_code); //复制OE号到表中
			strcpy(data2->brand, brand->brand_name_cn);//复制品牌到表中
			oedata_map[data2->oe_code] = data2;//存储数据
		}
		else //找到OE号
		{
			//data_ex = it3->second;
			data2 = it3->second;  
		}

		//if (strcmp(name, "ERP") == 0)
		//{
		//	strcpy(data_ex->erp_name, data->name);
		//}
		//else if (strcmp(name, "TB") == 0)
		//{
		//	strcpy(data_ex->tb_name, data->name);
		//	data_ex->tb_price = data->price;
		//}
		//else if (strcmp(name, "JOIN") == 0)
		//{
		//	strcpy(data_ex->join_name, data->name);
		//	data_ex->join_price = data->price;
		//}

		if (strcmp(name, "ERP") == 0) //如果是ERP
		{
			if (*(data->oe_name) != '\0')
				strcpy(data2->oe_name, data->oe_name);
		}
		else //
		{
			if (*(data->oe_name) != '\0' && *(data2->oe_name) == '\0')
				strcpy(data2->oe_name, data->oe_name);  //更改原厂件名称

			//if (data->price)
			//{
			//	if (data2->price)
			//	{
			//		if (data->price < data2->price)
			//			data2->price = data->price;
			//	}
			//	else
			//		data2->price = data->price;
			//}

			if (data->price)
			{
				if (strcmp(name, "JOIN") == 0 || !(data2->price))//更改原厂件价格
					data2->price = data->price;
			}
		}
	}
	return true;
}


bool OEUnify::write_result()
{

	DDETable *OEData_tb = db_result.get("OEData");//获取数据字典中OEData 部分
	const_char_map<OEData*> datas;   //数据量太大时 函数不返回 定义需放前面???????????
	size_t size=0;
	if (OEData_tb)
	{
	//	const_char_map<OEData*> datas;
		size = OEData_tb->size();
		for (size_t i = 0; i < size; ++i)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			auto it = datas.find(data->oe_code);
			if (it != datas.end())
			{
				OEData *data2 = it->second;
				printf("重复编码\t%s\t%s\t%s\t%s\t%s\n", data->oe_code, data->brand, data->oe_name, data2->brand, data2->oe_name);
			}
			else
				datas[data->oe_code] = data;
	
		}

		for (size_t i = 0; i < size; ++i)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			if (strchr(data->oe_code, '-'))
			{
				printf("编码有误\t%s\t%s\t%s\n", data->oe_code, data->brand, data->oe_name);
			}
		}
	}



	unify();

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
		if (!get_absolute_path(file_name, attribute->value(), conf_dir)) //获取OEData.e绝对地址
		{
			printf("配置文件“%s”OEUnify.ResultData.file内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(conf_dir, file_name, 1024);//获取文件目录
		create_dir(conf_dir, 0755);//创建目录
	}
	else
	{
		sprintf(file_name, "%s/OEData.e", conf_dir);
	}

	db_read.clear();
	if (!E2DDE::print_file(db_result, file_name))
	{
		printf("Write result file fail!\n");
		return false;
	}

	return true;
}

void OEUnify::unify()
{
	for (auto it = brand_oedata_map.begin(); it != brand_oedata_map.end(); ++it)
	{
		Brand *brand = it->first;
		switch (brand->brand_id)
		{
		case 1:
			assert(strcmp(brand->brand_code, "AUDI") == 0);
			unify_audi(it->second);
			break;

		case 2:
			assert(strcmp(brand->brand_code, "BENZ") == 0);
			unify_benz(it->second);
			break;

		case 3:
			assert(strcmp(brand->brand_code, "BMW") == 0);
			unify_bmw(it->second);
			break;

		case 4:
			assert(strcmp(brand->brand_code, "JAGUAR") == 0);
			unify_jaguar(it->second);
			break;

		case 5:
			assert(strcmp(brand->brand_code, "LANDROVER") == 0);
			unify_landrover(it->second);
			break;

		case 6:
			assert(strcmp(brand->brand_code, "PORSCHE") == 0);
			unify_porsche(it->second);
			break;

		case 7:
			assert(strcmp(brand->brand_code, "VOLVO") == 0);
			unify_volvo(it->second);
			break;

		case 8:
			assert(strcmp(brand->brand_code, "VW") == 0);
			unify_vw(it->second);
			break;
		}
	}
}

void OEUnify::unify_audi(const_char_map<OEData*> &oedatas)
{
	char buffer[24];
	memset(buffer, 0, 24);
	for (auto it = oedatas.begin(); it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		size_t offset = 0;
		if (*(data->oe_code) == 'L')//L 开头
			offset = 1;

		size_t size = strlen(data->oe_code);
		if(!check_OE_format(data->oe_code))  //SY  ADD   检查OE号是否含有特殊字符
		{
			printf("编号错误 %s %s\n", data->brand, data->oe_code);
			continue;
		}//END
		size -= offset;
		if (data->oe_code[offset] == 'N')  //开头带N
		{
			if (size < 8 || size > 9)
			{
				printf("编号错误 %s %s\n", data->brand, data->oe_code);
				continue;
			}
			data->oe_code[13 + offset] = data->oe_code[8 + offset];
			data->oe_code[12 + offset] = data->oe_code[7 + offset];
			data->oe_code[11 + offset] = ' ';
			data->oe_code[10 + offset] = data->oe_code[6 + offset];
			data->oe_code[9 + offset] = data->oe_code[5 + offset];
			data->oe_code[8 + offset] = data->oe_code[4 + offset];
			data->oe_code[7 + offset] = ' ';
			data->oe_code[6 + offset] = data->oe_code[3 + offset];
			data->oe_code[5 + offset] = data->oe_code[2 + offset];
			data->oe_code[4 + offset] = data->oe_code[1 + offset];
			data->oe_code[3 + offset] = ' ';
			data->oe_code[2 + offset] = ' ';
			data->oe_code[1 + offset] = ' ';
		}
		else
		{

			if (size < 5 || size > 14 )
			{
				printf("编号错误 %s %s\n", data->brand, data->oe_code);
				continue;
			}

			if (size >= 12)
			{
				if (size+offset==15)//SY  ADD L 开头的第15位要将L去掉
				{
					for (size_t i=0;i<size;i++)
					{
						data->oe_code[i]=data->oe_code[i+1];
					}
					data->oe_code[size]='0';
					offset=0;
				}		//END
				data->oe_code[17 + offset] = data->oe_code[size + offset - 1];
				data->oe_code[16 + offset] = data->oe_code[size + offset - 2];
				data->oe_code[15 + offset] = data->oe_code[size + offset - 3];
				data->oe_code[14 + offset] = ' ';
				if (size == 14)
				{
					data->oe_code[13 + offset] = data->oe_code[size + offset - 4];
					data->oe_code[12 + offset] = data->oe_code[size + offset - 5];
				}
				else if (size == 13)
				{
					data->oe_code[13 + offset] = ' ';
					data->oe_code[12 + offset] = data->oe_code[size + offset - 4];
				}
				else
				{
					data->oe_code[13 + offset] = ' ';
					data->oe_code[12 + offset] = ' ';
				}
				
				data->oe_code[11 + offset] = ' ';
			} 
			else if (size >= 10)
			{
				data->oe_code[13 + offset] = data->oe_code[10 + offset];
				data->oe_code[12 + offset] = data->oe_code[9 + offset];
				data->oe_code[11 + offset] = ' ';
			}
			else if(size < 9)//SY  ADD 小于9位 需要在前面补0成9位
			{
				for (size_t i=1;i<=size+offset;i++)
				{
					data->oe_code[9-i] = data->oe_code[size+offset-i];
				}
				for (size_t i=0;i<9-size-offset;i++)
				{
					data->oe_code[i]='0';
				}
			}//END

			data->oe_code[10 + offset] = data->oe_code[8 + offset];
			data->oe_code[9 + offset] = data->oe_code[7 + offset];
			data->oe_code[8 + offset] = data->oe_code[6 + offset];
			data->oe_code[7 + offset] = ' ';
			data->oe_code[6 + offset] = data->oe_code[5 + offset];
			data->oe_code[5 + offset] = data->oe_code[4 + offset];
			data->oe_code[4 + offset] = data->oe_code[3 + offset];
			data->oe_code[3 + offset] = ' ';
		}
	}
}

void OEUnify::unify_benz(const_char_map<OEData*> &oedatas)
{
	for (auto it = oedatas.begin();it != oedatas.end();++it)
	{
		OEData *data = it->second;
		size_t size = strlen(data->oe_code);

		if (!check_OE_format(data->oe_code))
		{
			printf("编号错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if (*(data->oe_code) == 'A') //A开头
		{
			if ( size != 11 && size != 15)
			{
				printf("编号错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			if (size > 11)
			{
				data->oe_code[19] = data->oe_code[14];
				data->oe_code[18] = data->oe_code[13];
				data->oe_code[17] = data->oe_code[12];
				data->oe_code[16] = data->oe_code[11];
				data->oe_code[15] = ' ';
			}
			data->oe_code[14] = data->oe_code[10];
			data->oe_code[13] = data->oe_code[9];
			data->oe_code[12] = ' ';
			data->oe_code[11] = data->oe_code[8];
			data->oe_code[10] = data->oe_code[7];
			data->oe_code[9] = ' ';
			data->oe_code[8] = data->oe_code[6];
			data->oe_code[7] = data->oe_code[5];
			data->oe_code[6] = data->oe_code[4];
			data->oe_code[5] = ' ';
			data->oe_code[4] = data->oe_code[3];
			data->oe_code[3] = data->oe_code[2];
			data->oe_code[2] = data->oe_code[1];
			data->oe_code[1] = ' ';
		}
		else if (*(data->oe_code) == 'B' && data->oe_code[1] == '6') //B6开头
		{
			if (size != 9)
			{
				printf("编号错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			else
			{
				data->oe_code[11] = data->oe_code[8];
				data->oe_code[10] = data->oe_code[7];
				data->oe_code[9] = data->oe_code[6];
				data->oe_code[8] = data->oe_code[5];
				data->oe_code[7] = ' ';
				data->oe_code[6] = data->oe_code[4];
				data->oe_code[5] = data->oe_code[3];
				data->oe_code[4] = ' ';
				data->oe_code[3] = data->oe_code[2];
				data->oe_code[2] = ' ';
			}
		}
		else if (*(data->oe_code) == 'N')
		{
			if (size != 13)
			{
				printf("编码错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			else
			{
				for (size_t i=size+1;i>0;i--)
				{
					if(i == 8 || i == 1)
						data->oe_code[i] = ' ';
					else
						data->oe_code[i] = data->oe_code[i-i/8-1]; 
				}
			}
		}
		else if(*(data->oe_code) == 'Q')
		{
			if (size != 13 && size != 18)
			{
				printf("编码错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			else
			{
				for (size_t i=1;i<size;i++)
				{
					data->oe_code[size+1-i] = data->oe_code[size-i];
				}
				data->oe_code[1]=' ';
			}
		}
		else if(*(data->oe_code) == 'X')
		{
			if (size != 11)
			{
				printf("编码错误 %s %s\n",data->brand,data->oe_code);
				continue;
			} 
			else
			{
				data->oe_code[14] = data->oe_code[10];
				data->oe_code[13] = ' ';
				data->oe_code[12] = data->oe_code[9];
				data->oe_code[11] = data->oe_code[8];
				data->oe_code[10] = data->oe_code[7];
				data->oe_code[9] = ' ';
				data->oe_code[8] = data->oe_code[6];
				data->oe_code[7] = data->oe_code[5];
				data->oe_code[6] = data->oe_code[4];
				data->oe_code[5] = ' ';
				data->oe_code[4] = data->oe_code[3];
				data->oe_code[3] = data->oe_code[2];
				data->oe_code[2] = data->oe_code[1];
				data->oe_code[1] = ' ';
			}

		}
	}
}

void OEUnify::unify_bmw(const_char_map<OEData*> &oedatas)
{
	char buffer[16];
	bool number_error=false;  //判断出现非数字
	memset(buffer, 0, 16);
	for (auto it = oedatas.begin(); it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		size_t size = strlen(data->oe_code);
		
		number_error=false;			//SY  ADD 
		for(size_t i=0;i<size;i++)   
		{
			if((data->oe_code[i]<'0' || data->oe_code[i] >'9'))
			{
				number_error = true;
				break;
			}
		}						//END

		if (size != 11 || number_error)
		{
			printf("编号错误 %s %s\n", data->brand, data->oe_code);
			continue;
		}

		/*for (size_t i = 0; i < size; ++i)
		{
			if (isalpha(data->oe_code[i]))
			{
				printf("编号错误 %s %s\n", data->brand, data->oe_code);
				break;
			}
		}*/

		data->oe_code[14] = data->oe_code[10];
		data->oe_code[13] = data->oe_code[9];
		data->oe_code[12] = data->oe_code[8];
		data->oe_code[11] = ' ';
		data->oe_code[10] = data->oe_code[7];
		data->oe_code[9] = data->oe_code[6];
		data->oe_code[8] = data->oe_code[5];
		data->oe_code[7] = ' ';
		data->oe_code[6] = data->oe_code[4];
		data->oe_code[5] = ' ';
		data->oe_code[4] = data->oe_code[3];
		data->oe_code[3] = data->oe_code[2];
		data->oe_code[2] = ' ';
	}
}

void OEUnify::unify_jaguar(const_char_map<OEData*> &oedatas)
{
	for (auto it = oedatas.begin();it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		if(!check_OE_format(data->oe_code))
		{
			printf("编号错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}
	}
}

void OEUnify::unify_landrover(const_char_map<OEData*> &oedatas)
{
	for (auto it = oedatas.begin(); it != oedatas.end(); ++it)
	{
		OEData *data = it->second;

		if(!check_OE_format(data->oe_code))
		{
			printf("编号错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}
		//if (!isalpha(data->oe_code[0]))
		//	printf("编号错误 %s %s\n", data->brand, data->oe_code);
	}
}

void OEUnify::unify_porsche(const_char_map<OEData*> &oedatas)
{
	char buffer[16];
	memset(buffer, 0, 16);
	for (auto it = oedatas.begin(); it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		size_t size = strlen(data->oe_code);
		//if (size != 11)
		//	printf("编号错误 %s %s\n", data->brand, data->oe_code);
		if (!check_OE_format(data->oe_code))
		{
			printf("编号错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if(size < 8 || size >14)
		{
			printf("编码错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}
		else
		{
			if(size == 8)
			{
				data->oe_code[10] = data->oe_code[7];
				data->oe_code[9] = data->oe_code[6];
				data->oe_code[8] = data->oe_code[5];
				data->oe_code[7] = ' ';
				data->oe_code[6] = data->oe_code[4];
				data->oe_code[5] = data->oe_code[3];
				data->oe_code[4] = ' ';
				data->oe_code[3] = ' ';
			}
			else 
			{
				if(size >= 12)
				{
					if(size == 14)
					{
						data->oe_code[17] = data->oe_code[13];
						data->oe_code[16] = data->oe_code[12];
					}
					else if(size == 13)
						data->oe_code[16] = data->oe_code[12];

					data->oe_code[15] = data->oe_code[11];
					data->oe_code[14] = ' ';
					data->oe_code[13] = data->oe_code[10];
					data->oe_code[12] = data->oe_code[9];
					data->oe_code[11] = ' ';
				}
				else if(size >9)
				{
					if(size == 11)
					{
						data->oe_code[13] = data->oe_code[10];
						data->oe_code[12] = data->oe_code[9];
					}
					else if(size == 10)
						data->oe_code[12] = data->oe_code[9];

					data->oe_code[11] = ' ';
				}
				
				data->oe_code[10] = data->oe_code[8];
				data->oe_code[9] = data->oe_code[7];
				data->oe_code[8] = data->oe_code[6];
				data->oe_code[7] = ' ';
				data->oe_code[6] = data->oe_code[5];
				data->oe_code[5] = data->oe_code[4];
				data->oe_code[4] = data->oe_code[3];
				data->oe_code[3] = ' ';
			}
		}
	}
}

void OEUnify::unify_volvo(const_char_map<OEData*> &oedatas)
{
	bool number_error=false;

	for (auto it = oedatas.begin(); it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		size_t size = strlen(data->oe_code);

		number_error=false;
		for (size_t i = 0; i < size; ++i)
		{
			if(data->oe_code[i] < '0' || data->oe_code[i] > '9')
			{
				number_error=true;
				break;
			}
			/*if (isalpha(data->oe_code[i]))
			{
				//printf("编号错误 %s %s\n", data->brand, data->oe_code);
				break;;
			}*/
		}
		if (number_error)
		{
			printf("编号错误 %s %s\n", data->brand, data->oe_code);
			continue;
		}
	}
}

void OEUnify::unify_vw(const_char_map<OEData*> &oedatas)
{
	for (auto it = oedatas.begin();it != oedatas.end(); ++it)
	{
		OEData *data = it->second;
		size_t size = strlen(data->oe_code);

		size_t offset = 0;
		if(*(data->oe_code) == 'L')
			offset = 1;

		size -=offset;
		if (!check_OE_format(data->oe_code))
		{
			printf("编号错误 %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if(*(data->oe_code) == 'N') //N开头
		{
			if (size < 8 || size > 9)
			{
				printf("编号错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			else
			{
				if(size == 9)
					data->oe_code[13] = data->oe_code[8];
				data->oe_code[12] = data->oe_code[7];
				data->oe_code[11] = ' ';
				data->oe_code[10] = data->oe_code[6];
				data->oe_code[9] = data->oe_code[5];
				data->oe_code[8] = data->oe_code[4];
				data->oe_code[7] = ' ';
				data->oe_code[6] = data->oe_code[3];
				data->oe_code[5] = data->oe_code[2];
				data->oe_code[4] = data->oe_code[1];
				data->oe_code[3] = ' ';
				data->oe_code[2] = ' ';
				data->oe_code[1] = ' ';
			}
		}
		else
		{
			if(size < 9 || size >14)
			{
				printf("编号错误 %s %s\n",data->brand,data->oe_code);
				continue;
			}
			else
			{

				if ( size == 14)
				{
					data->oe_code[17] = data->oe_code[13];
					data->oe_code[16] = data->oe_code[12];
					data->oe_code[15] = data->oe_code[11];
					data->oe_code[14] = ' ';
					data->oe_code[13] = data->oe_code[10];
					data->oe_code[12] = data->oe_code[9];
					data->oe_code[11] = ' ';
				}
				else if(size == 13)
				{
					if(data->oe_code[9+offset] == 'A')
					{
						data->oe_code[16+offset] = data->oe_code[12+offset];
						data->oe_code[15+offset] = data->oe_code[11+offset];
						data->oe_code[14+offset] = data->oe_code[10+offset];

					}
					else
					{
						data->oe_code[17] = data->oe_code[12];
						data->oe_code[16] = data->oe_code[11];
						data->oe_code[15] = data->oe_code[10];
						data->oe_code[14] = ' ';
					}
					data->oe_code[13+offset] = ' ';
					data->oe_code[12+offset] = data->oe_code[9];
					data->oe_code[11+offset] = ' ';
				}
				else if(size == 12)
				{
					data->oe_code[17] = data->oe_code[11];
					data->oe_code[16] = data->oe_code[10];
					data->oe_code[15] = data->oe_code[9];
					data->oe_code[14] = ' ';
					data->oe_code[13] = ' ';
					data->oe_code[12] = ' ';
					data->oe_code[11] = ' ';
				}

				else if(size > 9)
				{
					if (size == 11)
						data->oe_code[13] = data->oe_code[10];
					data->oe_code[12] = data->oe_code[9];
					data->oe_code[11] = ' ';
				}
				data->oe_code[10+offset] = data->oe_code[8+offset];
				data->oe_code[9+offset] = data->oe_code[7+offset];
				data->oe_code[8+offset] = data->oe_code[6+offset];
				data->oe_code[7+offset] = ' ';
				data->oe_code[6+offset] = data->oe_code[5+offset];
				data->oe_code[5+offset] = data->oe_code[4+offset];
				data->oe_code[4+offset] = data->oe_code[3+offset];
				data->oe_code[3+offset] = ' ';
			}
		}

	}
}

bool OEUnify::check_OE_format(const char *str_) //SY  ADD 检测OE号是否含有特殊字符
{
	if(str_ == NULL)
		return false;
	while(*str_!= '\0')
	{
		if(!((*str_ <='Z'&&*str_>='A')||
			(*str_>='a'&&*str_<='z')||
			(*str_>='0'&&*str_<='9')))
			return false;
		str_++;
	}
	return true;
}					//END
