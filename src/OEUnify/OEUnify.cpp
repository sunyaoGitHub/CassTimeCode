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
/*                 data_dir:�����ļ�Ŀ¼         _conf_file�������ļ�.xml�ļ�                                             */
/************************************************************************/
bool OEUnify::run(const char *data_dir, const char *_conf_file)//data_dir�������ļ�����·����_conf_file�����ļ�����·��
{
	conf_file = _conf_file;
	if (!read_configure())//�������ļ�
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);//��ȡ����schemeֵ
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))//��ȡVehicle.e�ļ�����·��
		{
			printf("�����ļ���%s��OEUnify.scheme���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
			return false;
		}
	}
	else  //ʹ��Ĭ��Vehicle.e�ļ�
	{
		char exe_dir[1024];
		get_exe_path(scheme_file, 1024);
		get_file_dir(exe_dir, scheme_file, 1024);
		get_absolute_path(scheme_file, "../../common/conf/DDE/Vehicle.e", exe_dir);
	}

	DDEScheme scheme; //�����ֵ�
	if (!scheme.parse_file(scheme_file))//����Vehicle.e�����ļ� �������ݸ�ʽ
	{
		printf("�����ļ���%s�������ڻ�����쳣�˳���\n", scheme_file);
		return false;
	}
	/*�������ݿⷽ�㴦��*/
	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_base())//����������
	{
		printf("�������ݶ���ʧ�ܣ�\n");
		return false;
	}

	if (!read_dir(data_dir))//���������
	{
		printf("������ݶ���ʧ�ܣ�\n");
		return false;
	}

	//unify();

	if (!write_result())//������
	{
		printf("�������ʧ�ܣ�\n");
		return false;
	}
	else
	{
		printf("������ɱ������ɹ���\n");
	}

	return true;
}

// ��ȡ�����ļ�
bool OEUnify::read_configure()
{
	try
	{
		xml_conf.parse_file<0>(conf_file);//����xml��ʽ�����ļ�
	}
	catch (parse_error &e)
	{
		printf("�����ļ���%s����������: %s��λ�ã�%s���쳣�˳���\n", conf_file, e.what(), e.where<char>());
		return false;
	}

	root_conf = xml_conf.first_node("OEUnify", 0, false);//��ȡxml��Ԫ��OEUnify
	if (!root_conf)
	{
		printf("�����ļ���%s��������Ч���쳣�˳���\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = root_conf->first_attribute("echo_time", 0, false);//��ȡecho_time����
	if (attribute && ((*attribute->value()) == '1'))
		TimeConsume::s_mode = 1;
	else
		TimeConsume::s_mode = 0;

	import_check = false;
	attribute = root_conf->first_attribute("import_check", 0, false);//��ȡimport_check����
	if (attribute && ((*attribute->value()) == '1'))
		import_check = true;

	get_file_dir(conf_dir, conf_file, 1024);//��ȡ�����ļ�Ŀ¼
	return true;
}

bool OEUnify::read_base() //����������
{
	TimeConsume tc("��ȡ��������");
	xml_node<> *base_data = root_conf->first_node("BaseData", 0, false);//��BaseData Ԫ��
	if (!base_data)
		return false;

	char file_name[1024];
	xml_attribute<> *attribute = base_data->first_attribute("file", 0, false); //��ȡfile����ֵ
	if (!attribute)
		return false;

	if (!get_absolute_path(file_name, attribute->value(), conf_dir))//��ȡBaseData.e����·��
	{
		printf("�����ļ���%s��OEUnify.BaseData.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	if (!doc.parse_file(file_name))//����BaseData.e�ļ���E����
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);//������������E������������ļ��еĸ�ʽ�ϲ������ݿ���
	
	DDETable *Brand_tb = db_read.get("Brand");//��ȡ���ݿ���Brand��
	if (!Brand_tb)
		return false;

	size_t size = Brand_tb->size();  //���С
	for (size_t i = 0; i < size; ++i)  
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i)); //ǿ��ת���ɱ�׼�ṹ
		Brand_code_map[data->brand_code] = data;		//����ÿһ��������Ʒ�Ʊ�������
		Brand_name_map[data->brand_name_cn] = data;  //����ÿһ��������Ʒ������������
	}

	return true;
}

bool OEUnify::read_dir(const char *data_dir)
{
	char filename[1024];

	printf("��ʼ����Ŀ¼��%s\n", data_dir);

#ifndef __linux
	struct _finddata_t file;	// ����ṹ�����
	sprintf(filename, "%s/*", data_dir);
	long handle=_findfirst(filename, &file);	// �������������ļ�
	if (handle == -1)
	{
		printf("δ�ҵ������ļ�!\n");
		return false;
	}

	do
	{
		if (file.attrib & _A_SUBDIR)  //����ѵ����ļ���һ��Ŀ¼
		{
			if(strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
			{
				sprintf(filename, "%s/%s", data_dir, file.name);
				read_dir(filename);
			}
		}
		else if (strstr(file.name, ".e"))//�������.e�ļ�
		{
			printf("��ʼ�����ļ���%s\n�ļ���������ȴ�......", file.name);
			sprintf(filename, "%s/%s", data_dir, file.name);//����Ŀ¼���ļ���
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

			printf("��ʼ�����ļ���%s\n", entry->d_name);
			sprintf(filename, "%s/%s", data_dir, entry->d_name);
			read_file(filename);
			printf("\n");
		}
	}

	closedir(dp);
#endif

	return true;
}

bool OEUnify::read_file(const char *data_file) //���ļ�
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

	e_node *node = db_read.scheme()->create_e_node(doc, "OEData");//����E��OEData �����ñ����doc
	if (!node)
		return false;
	if (!doc.parse_node_file(node, data_file, 0, 1))//���ݸ����ı�־�����ļ���ָ����E����
		return false;

	char name[128];
	get_file_name(name, data_file);//��ȡ�����ļ�����
	char *pos = strrchr(name, ' ');//��ĩβ����\0 �ַ���������
	if (pos)
		*pos = '\0';

	DDETable *OEData_tb = db_read.get("OEData");//��ȡ���ݱ�
	if (!OEData_tb)
		return false;

	//DDETable *OEDataEx_tb = db_result.get("OEDataEx");
	//if (!OEDataEx_tb)
	//	return false;

	DDETable *OEData2_tb = db_result.get("OEData");//��ȡ���ݱ�2
	if (!OEData2_tb)
		return false;

	OEData_tb->clear_record();   //�����¼
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);  //��E��ϲ������ݿ���

	size_t size = OEData_tb->size();
	if (!size)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i)); //��ȡÿһ��
		trim_space(data->oe_code);  //ȥ���հ��ַ�
		if (*(data->oe_code) == '\0')
			continue;

		Brand *brand = 0;
		auto it = Brand_name_map.find(data->brand);//����Ʒ�������ڻ��������ļ����в���
		if (it == Brand_name_map.end()) //δ�ҵ�
		{
			strupr(data->brand); //��Ʒ�Ʊ���ת���ɴ�д����һ��
			auto it2 = Brand_code_map.find(data->brand); 
			if (it2 == Brand_code_map.end())//δ�ҵ����޷�ʶ��
			{
				printf("\t\tƷ�������޷�ʶ��%s\n", data->brand);
				continue;
			}

			brand = it2->second;
		}
		else  //�ҵ������ȡ�����ڻ��������ļ��ж�Ӧ������(ָ�룬ԭ�洢��Ϊָ��)
			brand = it->second;

		strupr(data->oe_code);//��OE�Ÿĳɴ�д
		//const_char_map<OEDataEx*> &oedata_map = brand_oedata_map[brand];



		const_char_map<OEData*> &oedata_map = brand_oedata_map[brand];//�������� ÿ����MAP��������� ȷ�����ظ�
		auto it3 = oedata_map.find(data->oe_code);//����OE��
		//OEDataEx *data_ex = 0;
		OEData *data2 = 0;
		if (it3 == oedata_map.end())//δ�ҵ�OE��
		{
			//data_ex = reinterpret_cast<OEDataEx*>(OEDataEx_tb->allocate());
			//OEDataEx_tb->push_back(reinterpret_cast<char*>(data_ex));
			//strcpy(data_ex->code, data->code);
			//strcpy(data_ex->brand, brand->name_cn);
			//oedata_map[data_ex->code] = data_ex;
	
			data2 = reinterpret_cast<OEData*>(OEData2_tb->allocate());//����һ�����ݱ�ṹ
			OEData2_tb->push_back(reinterpret_cast<char*>(data2)); //���һ����Ԫ�ص�OEData2_tb��
			strcpy(data2->oe_code, data->oe_code); //����OE�ŵ�����
			strcpy(data2->brand, brand->brand_name_cn);//����Ʒ�Ƶ�����
			oedata_map[data2->oe_code] = data2;//�洢����
		}
		else //�ҵ�OE��
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

		if (strcmp(name, "ERP") == 0) //�����ERP
		{
			if (*(data->oe_name) != '\0')
				strcpy(data2->oe_name, data->oe_name);
		}
		else //
		{
			if (*(data->oe_name) != '\0' && *(data2->oe_name) == '\0')
				strcpy(data2->oe_name, data->oe_name);  //����ԭ��������

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
				if (strcmp(name, "JOIN") == 0 || !(data2->price))//����ԭ�����۸�
					data2->price = data->price;
			}
		}
	}
	return true;
}


bool OEUnify::write_result()
{

	DDETable *OEData_tb = db_result.get("OEData");//��ȡ�����ֵ���OEData ����
	const_char_map<OEData*> datas;   //������̫��ʱ ���������� �������ǰ��???????????
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
				printf("�ظ�����\t%s\t%s\t%s\t%s\t%s\n", data->oe_code, data->brand, data->oe_name, data2->brand, data2->oe_name);
			}
			else
				datas[data->oe_code] = data;
	
		}

		for (size_t i = 0; i < size; ++i)
		{
			OEData *data = reinterpret_cast<OEData*>(OEData_tb->at(i));
			if (strchr(data->oe_code, '-'))
			{
				printf("��������\t%s\t%s\t%s\n", data->oe_code, data->brand, data->oe_name);
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
		if (!get_absolute_path(file_name, attribute->value(), conf_dir)) //��ȡOEData.e���Ե�ַ
		{
			printf("�����ļ���%s��OEUnify.ResultData.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
			return false;
		}

		get_file_dir(conf_dir, file_name, 1024);//��ȡ�ļ�Ŀ¼
		create_dir(conf_dir, 0755);//����Ŀ¼
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
		if (*(data->oe_code) == 'L')//L ��ͷ
			offset = 1;

		size_t size = strlen(data->oe_code);
		if(!check_OE_format(data->oe_code))  //SY  ADD   ���OE���Ƿ��������ַ�
		{
			printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
			continue;
		}//END
		size -= offset;
		if (data->oe_code[offset] == 'N')  //��ͷ��N
		{
			if (size < 8 || size > 9)
			{
				printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
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
				printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
				continue;
			}

			if (size >= 12)
			{
				if (size+offset==15)//SY  ADD L ��ͷ�ĵ�15λҪ��Lȥ��
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
			else if(size < 9)//SY  ADD С��9λ ��Ҫ��ǰ�油0��9λ
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
			printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if (*(data->oe_code) == 'A') //A��ͷ
		{
			if ( size != 11 && size != 15)
			{
				printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
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
		else if (*(data->oe_code) == 'B' && data->oe_code[1] == '6') //B6��ͷ
		{
			if (size != 9)
			{
				printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
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
				printf("������� %s %s\n",data->brand,data->oe_code);
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
				printf("������� %s %s\n",data->brand,data->oe_code);
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
				printf("������� %s %s\n",data->brand,data->oe_code);
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
	bool number_error=false;  //�жϳ��ַ�����
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
			printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
			continue;
		}

		/*for (size_t i = 0; i < size; ++i)
		{
			if (isalpha(data->oe_code[i]))
			{
				printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
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
			printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
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
			printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
			continue;
		}
		//if (!isalpha(data->oe_code[0]))
		//	printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
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
		//	printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
		if (!check_OE_format(data->oe_code))
		{
			printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if(size < 8 || size >14)
		{
			printf("������� %s %s\n",data->brand,data->oe_code);
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
				//printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
				break;;
			}*/
		}
		if (number_error)
		{
			printf("��Ŵ��� %s %s\n", data->brand, data->oe_code);
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
			printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
			continue;
		}

		if(*(data->oe_code) == 'N') //N��ͷ
		{
			if (size < 8 || size > 9)
			{
				printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
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
				printf("��Ŵ��� %s %s\n",data->brand,data->oe_code);
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

bool OEUnify::check_OE_format(const char *str_) //SY  ADD ���OE���Ƿ��������ַ�
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
