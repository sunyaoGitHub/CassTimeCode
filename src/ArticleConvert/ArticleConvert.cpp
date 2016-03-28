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
/*                data_dir:�����ļ�Ŀ¼           _conf_file�������ļ�ȫ·��                                              */
/************************************************************************/
bool ArticleConvert::run(const char *data_dir, const char *_conf_file)
{
	conf_file = _conf_file;
	if (!read_configure())//�������ļ����һЩ��ʼ������ ����ȡ�����ļ�Ŀ¼
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))
		{
			printf("�����ļ���%s��ArticleConvert.scheme���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
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
	if (!scheme.parse_file(scheme_file))//����Vehicle.e��ʽ�ļ�
	{
		printf("�����ļ���%s�������ڻ�����쳣�˳���\n", scheme_file);
		return false;
	}
	/*�ֱ�ָ�������ֵ䲢����*/
	db_data.scheme(&scheme);
	db_data.m_name = "data";
	db_read.scheme(&scheme);
	db_read.m_name = "read";
	db_result.scheme(&scheme);
	db_result.m_name = "result";

	if (!read_base())//���������� ��db_result��
	{
		printf("�������ݶ���ʧ�ܣ�\n");
		return false;
	}

	read_catalog_map();//��ȡĿ¼ӳ��
	read_oename();//��ȡOE����

	if (!read_model_map())
	{
		printf("����ӳ�����ݶ���ʧ�ܣ�\n");
		return false;
	}

	if (!read_dir(data_dir))
	{
		printf("������ݶ���ʧ�ܣ�\n");
		return false;
	}

	if (!write_result())
	{
		printf("�������ʧ�ܣ�\n");
		return false;
	}

	return true;
}

// ��ȡ�����ļ�
bool ArticleConvert::read_configure()
{
	try
	{
		xml_conf.parse_file<0>(conf_file);
	}
	catch (parse_error &e)
	{
		printf("�����ļ���%s����������: %s��λ�ã�%s���쳣�˳���\n", conf_file, e.what(), e.where<char>());
		return false;
	}

	root_conf = xml_conf.first_node("ArticleConvert", 0, false);
	if (!root_conf)
	{
		printf("�����ļ���%s��������Ч���쳣�˳���\n", conf_file);
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

	get_file_dir(conf_dir, conf_file, 1024);//��ȡ�����ļ�����Ŀ¼
	return true;
}

bool ArticleConvert::read_base()
{
	TimeConsume tc("��ȡ��������");
	xml_node<> *base_data = root_conf->first_node("BaseData", 0, false);
	if (!base_data)
		return false;

	char file_name[1024];
	xml_attribute<> *attribute = base_data->first_attribute("file", 0, false);
	if (!attribute)
		return false;

	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("�����ļ���%s��ArticleConvert.BaseData.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
		return false;
	}

	e_document_ex doc;
	if (!doc.parse_file(file_name))//�������������ļ�
	{
		printf("parse_file fail\n");
		return false;
	}

	E2DDE efile(db_result.scheme());//ʹ�������ֵ��ʼ��
	efile.parse(db_result, doc, e_dde_parse_combine);//���������E��ϲ�������
	return true;
}

bool ArticleConvert::read_model_map()
{
	TimeConsume tc("��ȡ����ӳ������");

	xml_node<> *node = root_conf->first_node("ModelMap", 0, false);
	if (!node)
	{
		printf("�����ļ���%s��ArticleConvert.ModelMapȱʧ���쳣�˳���\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = node->first_attribute("file", 0, false);
	if (!attribute)
	{
		printf("�����ļ���%s��ArticleConvert.ModelMap.fileȱʧ���쳣�˳���\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("�����ļ���%s��ArticleConvert.ModelMap.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
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
		Brand_map[data->brand_code] = data;//ʹ��Ʒ�Ʊ��뽨��ӳ��
	}

	DDETable *EPCModel_tb = db_result.get("EPCModel");
	if (!EPCModel_tb)
		return false;

	const_char_map<EPCModel *> EPCModel_map;
	size = EPCModel_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		EPCModel *data = reinterpret_cast<EPCModel*>(EPCModel_tb->at(i));
		EPCModel_map[data->epc_model_code] = data;//ʹ�ó��ͱ��뽨��ӳ��
	}

	DDETable *ModelTransfer_tb = db_result.get("ModelTransfer");
	if (!ModelTransfer_tb)
		return false;

	const_char_map<ModelTransfer *> ModelTransfer_map;
	size = ModelTransfer_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		ModelTransfer *data = reinterpret_cast<ModelTransfer*>(ModelTransfer_tb->at(i));
		ModelTransfer_map[data->model_code] = data;//ʹ�ó��ͱ��뽨��ӳ��
	}

	size = Level2YunData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Level2YunData *data = reinterpret_cast<Level2YunData*>(Level2YunData_tb->at(i));
		//printf("���ڴ��������ͣ�%s\n", data->level_code);

		if (*(data->yun_code) == '\0')
		{
			printf("\tEPC����Ϊ�գ�%s\n", data->level_code);
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
					printf("\t�Ƴ��䳵�͡�%s��Ʒ�Ʋ�ͬ��%d %d��\n", data->yun_code, yunmodel->brand_id, brand->brand_id);

				if (strcmp(yunmodel->epc_model_name, data->name) != 0)
					printf("\t�Ƴ��䳵�͡�%s�����Ʋ�ͬ��%s %s��\n", data->yun_code, yunmodel->epc_model_name, data->name);
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
					printf("\t�����͡�%s����Ӧ���Ƴ��䳵�Ͳ�ͬ��%d %d��\n", data->level_code, level2yun->epc_model_id, yunmodel->epc_model_id);
				}
			}
		}
	}

	return true;
}

bool ArticleConvert::read_dir(const char *data_dir)
{
	TimeConsume tc("�������");
	ArticleParse ap;
	if (!ap.run(db_data, db_read, db_result, data_dir, import_check))
	{
		printf("�������ʧ�ܣ�\n");
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
			printf("�����ļ���%s��ArticleConvert.ResultData.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
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
		printf("�����ļ���%s��ArticleConvert.CatalogMap.fileȱʧ���쳣�˳���\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("�����ļ���%s��ArticleConvert.CatalogMap.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
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
		printf("�����ļ���%s��ArticleConvert.OEName.fileȱʧ���쳣�˳���\n", conf_file);
		return false;
	}

	char file_name[512];
	if (!get_absolute_path(file_name, attribute->value(), conf_dir))
	{
		printf("�����ļ���%s��ArticleConvert.OEName.file���ݡ�%s�������쳣�˳���\n", conf_file, attribute->value());
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
