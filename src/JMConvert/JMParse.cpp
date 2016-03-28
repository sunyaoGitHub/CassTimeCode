#include "stdafx.h"
#include "JMParse.h"

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

JMParse::JMParse()
{
}

JMParse::~JMParse()
{
}

bool JMParse::initialize(DDEDatabase &db_result)
{
	Catalog_tb = db_result.get("Catalog");
	if (!Catalog_tb)
		return false;

	Vocabulary_tb = db_result.get("Vocabulary");
	if (!Vocabulary_tb)
		return false;

	return true;
}

void JMParse::parse_cat1()
{
	char *name = jmdata->cat_name;
	if (*name == '\0')
		return;

	auto it = Catalog_map.find(name);
	if (it != Catalog_map.end())
	{
		if (catalog != it->second)
		{
			catalog = it->second;
			catalog2 = 0;
			catalog3 = 0;
		}
		
		return;
	}

	catalog = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
	Catalog_tb->push_back(reinterpret_cast<char*>(catalog));
	strncpy_ex(catalog->cat_name, name, 48);
	Catalog_map[catalog->cat_name] = catalog;
	catalog->cat_id = static_cast<int32_t>(Catalog_tb->size());
	catalog->sort_order = catalog->cat_id * 1000 * 1000;
	Catalog_id_map[catalog->cat_id] = catalog;
	catalog2 = 0;
	catalog3 = 0;
}

void JMParse::parse_cat2()
{
	char *name = jmdata->cat_name2;
	if (*name == '\0')
		return;

	auto it = Catalog2_map.find(name);
	if (it != Catalog2_map.end())
	{
		if (catalog2 != it->second)
		{
			catalog2 = it->second;
			catalog3 = 0;
		}

		if (check)
		{
			if (catalog2->parent_cat_id != catalog->cat_id)
				printf("\t二级目录“%s”所属的一级目录不同：%s %s。\n", jmdata->cat_name2, Catalog_id_map[catalog2->parent_cat_id]->cat_name, catalog->cat_name);
		}
		return;
	}

	catalog2 = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
	Catalog_tb->push_back(reinterpret_cast<char*>(catalog2));
	strncpy_ex(catalog2->cat_name, name, 48);
	Catalog2_map[catalog2->cat_name] = catalog2;
	catalog2->cat_id = static_cast<int32_t>(Catalog_tb->size());
	catalog2->parent_cat_id = catalog->cat_id;
	catalog2->sort_order = (catalog->cat_id * 1000 + catalog2->cat_id) * 1000;
	Catalog_id_map[catalog2->cat_id] = catalog2;
	catalog3 = 0;
}

void JMParse::parse_cat3()
{
	char *name = jmdata->cat_name3;
	if (*name == '\0')
		return;

	if (!catalog2)
		return;

	auto it = Catalog3_map.find(name);
	if (it != Catalog3_map.end())
	{
		catalog3 = it->second;
		if (check)
		{
			if (catalog3->parent_cat_id != catalog2->cat_id)
				printf("\t三级级目录“%s”所属的二级目录不同：%s %s。\n", jmdata->cat_name3, Catalog_id_map[catalog3->parent_cat_id]->cat_name, catalog2->cat_name);
		}
		return;
	}

	catalog3 = reinterpret_cast<Catalog*>(Catalog_tb->allocate());
	Catalog_tb->push_back(reinterpret_cast<char*>(catalog3));
	strncpy_ex(catalog3->cat_name, name, 48);
	Catalog3_map[catalog3->cat_name] = catalog3;
	catalog3->sort_order = 3;
	catalog3->cat_id = static_cast<int32_t>(Catalog_tb->size());
	catalog3->parent_cat_id = catalog2->cat_id;
	catalog3->sort_order = (catalog->cat_id * 1000 + catalog2->cat_id) * 1000 + catalog3->cat_id;
	Catalog_id_map[catalog3->cat_id] = catalog3;
}

void JMParse::parse_vocabulary()
{
	if (*(jmdata->name) == '\0')
		return;

	auto it = Vocabulary_map.find(jmdata->name);
	if (it != Vocabulary_map.end())
	{
		//vocabulary = it->second;
		//if (check)
		//{
		//	if (catalog3)
		//	{
		//		if (vocabulary->cat_id != catalog3->id)
		//			printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->name, vocabulary->cat_id, catalog3->name, catalog3->id);
		//	}
		//	else if (catalog2)
		//	{
		//		if (vocabulary->cat_id != catalog2->id)
		//			printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->name, vocabulary->cat_id, catalog2->name, catalog2->id);
		//	}
		//	else
		//	{
		//		assert(catalog);
		//		if (vocabulary->cat_id != catalog->id)
		//			printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->name, vocabulary->cat_id, catalog->name, catalog->id);
		//	}

		//	if (strcmp(vocabulary->alias, jmdata->alias) != 0)
		//		printf("\t配件“%s”零件别名不同：%s %s。\n", jmdata->name, vocabulary->alias, jmdata->alias);

		//	if (vocabulary->number != jmdata->number)
		//		printf("\t配件“%s”零件数量不同：%d %d。\n", jmdata->name, vocabulary->number, jmdata->number);

		//	//if (strcmp(vocabulary->oe_name, jmdata->oe_name) != 0)
		//	//	printf("\t配件“%s”原厂零件名称不同：%s %s。\n", jmdata->name, vocabulary->oe_name, jmdata->oe_name);

		//	//if (strcmp(vocabulary->info, jmdata->info) != 0)
		//	//	printf("\t配件“%s”备注不同：%s %s。\n", jmdata->name, vocabulary->info, jmdata->info);
		//}

		auto it2 = it->second.find(jmdata->oe_name);
		if (it2 != it->second.end())
		{
			vocabulary = it2->second;
			if (check)
			{
				if (catalog3)
				{
					if (vocabulary->cat_id != catalog3->cat_id)
						printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->cat_name, vocabulary->cat_id, catalog3->cat_name, catalog3->cat_id);
				}
				else if (catalog2)
				{
					if (vocabulary->cat_id != catalog2->cat_id)
						printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->cat_name, vocabulary->cat_id, catalog2->cat_name, catalog2->cat_id);
				}
				else
				{
					assert(catalog);
					if (vocabulary->cat_id != catalog->cat_id)
						printf("\t配件“%s”目录不同：%s(%d)\t%s(%d)。\n", jmdata->name, Catalog_id_map[vocabulary->cat_id]->cat_name, vocabulary->cat_id, catalog->cat_name, catalog->cat_id);
				}

				if (strcmp(vocabulary->alias, jmdata->alias) != 0)
					printf("\t配件“%s”零件别名不同：%s %s。\n", jmdata->name, vocabulary->alias, jmdata->alias);

				if (vocabulary->number != jmdata->number)
					printf("\t配件“%s”零件数量不同：%d %d。\n", jmdata->name, vocabulary->number, jmdata->number);

				if (strcmp(vocabulary->oe_name, jmdata->oe_name) != 0)
					printf("\t配件“%s”原厂零件名称不同：%s %s。\n", jmdata->name, vocabulary->oe_name, jmdata->oe_name);

				//if (strcmp(vocabulary->info, jmdata->info) != 0)
				//	printf("\t配件“%s”备注不同：%s %s。\n", jmdata->name, vocabulary->info, jmdata->info);
			}

			return;
		}
	}

	vocabulary = reinterpret_cast<Vocabulary*>(Vocabulary_tb->allocate());
	Vocabulary_tb->push_back(reinterpret_cast<char*>(vocabulary));
	if (catalog3)
	{
		vocabulary->cat_id = catalog3->cat_id;
		catalog_oes.insert(catalog3);
	}
	else if (catalog2)
	{
		vocabulary->cat_id = catalog2->cat_id;
		catalog_oes.insert(catalog2);
	}
	else
	{
		vocabulary->cat_id = catalog->cat_id;
		catalog_oes.insert(catalog);
	}

	vocabulary->number = jmdata->number;

	strncpy_ex(vocabulary->name, jmdata->name, 48);
	strncpy_ex(vocabulary->alias, jmdata->alias, 64);
	strncpy_ex(vocabulary->oe_name, jmdata->oe_name, 64);
	strncpy_ex(vocabulary->info, jmdata->info, 96);
	Vocabulary_map[vocabulary->name][vocabulary->oe_name] = vocabulary;
}

class catalog_compare
{
public:
	bool operator()(const Catalog *lhs, const Catalog *rhs) const
	{
		return lhs->sort_order < rhs->sort_order;
	}
};

class vocabulary_compare
{
public:
	vocabulary_compare(map<int32_t, Catalog*> *_Catalog_id_map) : Catalog_id_map(_Catalog_id_map)
	{
	}

	bool operator()(const Vocabulary *lh, const Vocabulary *rh) const
	{
		Catalog *lhs = (*Catalog_id_map)[lh->cat_id];
		Catalog *rhs = (*Catalog_id_map)[rh->cat_id];
		return lhs->sort_order < rhs->sort_order;
	}

protected:
	map<int32_t, Catalog*> *Catalog_id_map;
};

void JMParse::generate_jmdata(DDEDatabase &db_result)
{
	Vocabulary_tb->organize();
	Catalog_tb->organize();

	sort(reinterpret_cast<Catalog**>(Catalog_tb->record()), reinterpret_cast<Catalog**>(Catalog_tb->record() + Catalog_tb->size()), catalog_compare());
	sort(reinterpret_cast<Vocabulary**>(Vocabulary_tb->record()), reinterpret_cast<Vocabulary**>(Vocabulary_tb->record() + Vocabulary_tb->size()), vocabulary_compare(&Catalog_id_map));

	JMData_tb = db_result.get("JMData");
	if (!JMData_tb)
		return;

	size_t size = Vocabulary_tb->size();
	if (!size)
		return;

	//const_char_map<Vocabulary *> Vocabulary_map2;
	//for (size_t i = 0; i < size; ++i)
	//{
	//	vocabulary = reinterpret_cast<Vocabulary*>(Vocabulary_tb->at(i));
	//	Vocabulary_map2[vocabulary->name] = vocabulary;
	//}

	//jmdata = reinterpret_cast<JMData*>(JMData_tb->allocate(Vocabulary_map2.size()));
	//JMData_tb->push_back(reinterpret_cast<char*>(jmdata), Vocabulary_map2.size());
	//Vocabulary_map2.clear();
	jmdata = reinterpret_cast<JMData*>(JMData_tb->allocate(size));
	JMData_tb->push_back(reinterpret_cast<char*>(jmdata), size);
	for (size_t i = 0; i < size; ++i)
	{
		vocabulary = reinterpret_cast<Vocabulary*>(Vocabulary_tb->at(i));
		//if (Vocabulary_map2.find(vocabulary->name) != Vocabulary_map2.end())
		//	continue;

		//Vocabulary_map2[vocabulary->name] = vocabulary;

		Catalog *tmp = Catalog_id_map[vocabulary->cat_id];
		if (tmp->parent_cat_id)
		{
			catalog3 = 0;
			catalog2 = tmp;
			catalog = Catalog_id_map[tmp->parent_cat_id];

			if (catalog->parent_cat_id)
			{
				catalog3 = tmp;
				catalog2 = catalog;
				catalog = Catalog_id_map[catalog2->parent_cat_id];
			}
		}
		else
		{
			catalog = tmp;
			catalog2 = 0;
			catalog3 = 0;
		}

		//if (*(vocabulary->name) == '\0')
		//	continue;

		if (catalog)
			strncpy_ex(jmdata->cat_name, catalog->cat_name, 40);

		if (catalog2)
			strncpy_ex(jmdata->cat_name2, catalog2->cat_name, 40);

		if (catalog3)
			strncpy_ex(jmdata->cat_name3, catalog3->cat_name, 40);

		strcpy(jmdata->name, vocabulary->name);
		strcpy(jmdata->alias, vocabulary->alias);
		jmdata->number = vocabulary->number;
		strcpy(jmdata->oe_name, vocabulary->oe_name);
		strcpy(jmdata->info, vocabulary->info);

		jmdata++;
	}
}

void JMParse::generate_unidentifyoe(DDEDatabase &db_data, DDEDatabase &db_result)
{
	DDETable *oe_tb = db_data.get("OE");
	if (!oe_tb)
		return;

	DDETable *Brand_tb = db_data.get("Brand");
	if (!Brand_tb)
		return;

	Catalog_tb = db_data.get("Catalog");
	if (!Catalog_tb)
		return;

	DDETable *OE2Catalog_tb = db_data.get("OE2Catalog");
	if (!OE2Catalog_tb)
		return;

	map<int32_t, Brand*> Brand_map;
	size_t size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_map[data->brand_id] = data;
	}

	map<int32_t, Catalog*> Catalog_id_map2;
	Catalog_id_map2.clear();
	size = Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Catalog *data = reinterpret_cast<Catalog*>(Catalog_tb->at(i));
		Catalog_id_map2[data->cat_id] = data;
	}

	map<int32_t, int32_t> OE2Catalog_map;
	size = OE2Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE2Catalog *data = reinterpret_cast<OE2Catalog*>(OE2Catalog_tb->at(i));
		auto it = OE2Catalog_map.find(data->oe_id);
		if (it == OE2Catalog_map.end())
			OE2Catalog_map[data->oe_id] = data->cat_id;
	}

	const_char_map<Vocabulary *> Vocabulary_map2;
	size = Vocabulary_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Vocabulary *data = reinterpret_cast<Vocabulary*>(Vocabulary_tb->at(i));
		if (*(data->oe_name) == '\0')
			continue;

		auto it = Vocabulary_map2.find(data->oe_name);
		if (it != Vocabulary_map2.end())
		{
			Vocabulary *data2 = it->second;
			if (data2->cat_id != data->cat_id)
			{
				if (!((strstr(data->name, "左") && strstr(data2->name, "右")) || (strstr(data->name, "右") && strstr(data2->name, "左"))))
					printf("同一EPC名称“%s”对应不同标准配件名称：%s %s\n", data->oe_name, data2->name, data->name);
			}
		}

		Vocabulary_map2[data->oe_name] = data;
	}

	DDETable *UnIdentifyOE_tb = db_result.get("UnIdentifyOE");
	if (!UnIdentifyOE_tb)
		return;

	size = oe_tb->size();
	if (!size)
		return;

	const_char_map<set<int>>oe_map;
	for (size_t i = 0; i < size; ++i)
	{
		OE *oe = reinterpret_cast<OE*>(oe_tb->at(i));
		auto it = Vocabulary_map2.find(oe->oe_name);
		if (it != Vocabulary_map2.end())
			continue;

		int cat_id = OE2Catalog_map[oe->oe_id];
		auto it2 = oe_map.find(oe->oe_name);
		if (it2 != oe_map.end())
		{
			if (it2->second.find(cat_id) != it2->second.end())
				continue;
		}

		oe_map[oe->oe_name].insert(cat_id);

		UnIdentifyOE *unidentifyoe = reinterpret_cast<UnIdentifyOE*>(UnIdentifyOE_tb->allocate());
		UnIdentifyOE_tb->push_back(reinterpret_cast<char*>(unidentifyoe));

		Brand *brand = Brand_map[oe->brand_id];
		strcpy(unidentifyoe->brand_code, brand->brand_code);
		strcpy(unidentifyoe->oe_code, oe->oe_code);
		catalog = Catalog_id_map2[cat_id];
		strcpy(unidentifyoe->cat2_id, catalog->cat_name);
		if (catalog->parent_cat_id)
		{
			catalog2 = Catalog_id_map2[catalog->parent_cat_id];
			strcpy(unidentifyoe->cat_id, catalog2->cat_name);
		}

		strcpy(unidentifyoe->oe_name, oe->oe_name);
		strcpy(unidentifyoe->info, oe->info);
	}
}

void JMParse::generate_nameconvert(DDEDatabase &db_data, DDEDatabase &db_result)
{
	DDETable *oe_tb = db_data.get("OE");
	if (!oe_tb)
		return;

	DDETable *Brand_tb = db_data.get("Brand");
	if (!Brand_tb)
		return;

	Catalog_tb = db_data.get("Catalog");
	if (!Catalog_tb)
		return;

	DDETable *OE2Catalog_tb = db_data.get("OE2Catalog");
	if (!OE2Catalog_tb)
		return;

	DDETable *CatalogMap_tb = db_data.get("CatalogMap");
	if (!CatalogMap_tb)
		return;

	DDETable *ExchangeData_tb = db_data.get("ExchangeData");
	if (!ExchangeData_tb)
		return;

	map<int32_t, Brand*> Brand_id_map;
	const_char_map<Brand*> Brand_map;
	size_t size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_id_map[data->brand_id] = data;
		Brand_map[data->brand_code] = data;
	}

	map<int32_t, Catalog*> Catalog_id_map2;
	Catalog_id_map2.clear();
	size = Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Catalog *data = reinterpret_cast<Catalog*>(Catalog_tb->at(i));
		Catalog_id_map2[data->cat_id] = data;
	}

	map<string, Catalog*> name_catalog_map;
	for (size_t i = 0; i < size; ++i)
	{
		Catalog *data = reinterpret_cast<Catalog*>(Catalog_tb->at(i));
		if (!data->parent_cat_id)
			continue;

		string name = data->cat_name;
		name += ";";
		name += Catalog_id_map2[data->parent_cat_id]->cat_name;
		name_catalog_map[name] = data;
	}

	size = oe_tb->size();
	if (!size)
		return;

	map<int32_t, OE*> OEArticle_map;
	for (size_t i = 0; i < size; ++i)
	{
		OE *oe = reinterpret_cast<OE*>(oe_tb->at(i));
		OEArticle_map[oe->oe_id] = oe;
	}

	map<Catalog*, set<OE*>> catalog_oe_map; 
	size = OE2Catalog_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		OE2Catalog *data = reinterpret_cast<OE2Catalog*>(OE2Catalog_tb->at(i));
		catalog_oe_map[Catalog_id_map2[data->cat_id]].insert(OEArticle_map[data->oe_id]);
	}

	map<Brand*, const_char_map<ExchangeData*>> brand_vendors_map; 
	size = ExchangeData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		ExchangeData *data = reinterpret_cast<ExchangeData*>(ExchangeData_tb->at(i));
		if (strcmp(data->brand_code, data->oe_brand_code) == 0)
			brand_vendors_map[Brand_map[data->oe_brand_code]][data->oe_code] = data;
	}

	const_char_map<set<Vocabulary *>> Vocabulary_map;
	size = Vocabulary_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Vocabulary *data = reinterpret_cast<Vocabulary*>(Vocabulary_tb->at(i));
		if (*(data->oe_name) == '\0')
			continue;

		Vocabulary_map[data->oe_name].insert(data);
	}

	//const_char_map<string> Vocabulary_map2;
	//string name;
	//for (auto it = Vocabulary_map.begin(); it != Vocabulary_map.end(); ++it)
	//{
	//	set<Vocabulary *> &vocabularys = it->second;
	//	name.clear();
	//	for (auto it2 = vocabularys.begin(); it2 != vocabularys.end(); ++it2)
	//	{
	//		Vocabulary *vocabulary = *it2;
	//		if (name.empty())
	//		{
	//			name = vocabulary->name;
	//		}
	//		else
	//		{
	//			name += ';';
	//			name += vocabulary->name;
	//		}
	//	}

	//	Vocabulary_map2[it->first] = name;
	//}

	DDETable *NameConvert_tb = db_result.get("NameConvert");
	if (!NameConvert_tb)
		return;

	size = CatalogMap_tb->size();
	if (!size)
		return;

	set<Catalog*> catalog_maps;
	set<Catalog*> oe_catalog_maps;
	map<Catalog *, Catalog *> catalog_map;
	catalog = 0;
	catalog2 = 0;
	catalog3 = 0;
	for (size_t i = 0; i < size; ++i)
	{
		CatalogMap *data = reinterpret_cast<CatalogMap*>(CatalogMap_tb->at(i));
		if (*(data->epc_cat_name) == '\0' || *(data->epc_cat_name2) == '\0')
			continue;

		if (*(data->cat_name) != '\0')
		{
			catalog = Catalog_map[data->cat_name];
			catalog2 = 0;
			catalog3 = 0;
		}

		if (!catalog)
		{
			printf("\t一级目录为空 %s！\n", data->cat_name);
			continue;
		}

		if (*(data->cat_name2) != '\0')
		{
			catalog2 = Catalog2_map[data->cat_name2];
			catalog3 = 0;
		}

		if (!catalog2)
		{
			printf("\t二级目录为空或不存在：%s %s %s %s %s！\n", catalog->cat_name, data->cat_name2, data->cat_name3, data->epc_cat_name, data->epc_cat_name2);
			continue;
		}

		if (*(data->cat_name3) != '\0')
			catalog3 = Catalog3_map[data->cat_name3];

		if (catalog3)
			catalog_maps.insert(catalog3);
		else
			catalog_maps.insert(catalog2);

		string name = data->epc_cat_name2;
		name += ";";
		name += data->epc_cat_name;
		auto it = name_catalog_map.find(name);
		if (it == name_catalog_map.end())
		{
			printf("\tEPC目录不存在：%s\n", name.c_str());
			continue;
		}

		Catalog *oe_catalog = it->second;
		auto it_cat = catalog_map.find(oe_catalog);
		if (it_cat != catalog_map.end())
		{
			Catalog *jm_catalog = it_cat->second;
			if (catalog3)
			{
				if (jm_catalog != catalog3)
					printf("\tEPC目录重复映射：%s -> %s.%s.%s, %s.%s.%s\n", name.c_str(), catalog->cat_name, catalog2->cat_name, catalog3->cat_name, Catalog_id_map[Catalog_id_map[jm_catalog->parent_cat_id]->parent_cat_id]->cat_name, Catalog_id_map[jm_catalog->parent_cat_id]->cat_name, jm_catalog->cat_name);
			}
			else
			{
				if (jm_catalog != catalog2)
					printf("\tEPC目录重复映射：%s -> %s.%s, %s.%s\n", name.c_str(), catalog->cat_name, catalog2->cat_name, Catalog_id_map[jm_catalog->parent_cat_id]->cat_name, jm_catalog->cat_name);
			}

			continue;
		}

		int direction = 0;
		if (catalog3)
		{
			catalog_map[oe_catalog] = catalog3;
			if (strstr(catalog3->cat_name, "左"))
				direction = 1;
			else if (strstr(catalog3->cat_name, "右"))
				direction = 2;
		}
		else
		{
			catalog_map[oe_catalog] = catalog2;
			if (strstr(catalog2->cat_name, "左"))
				direction = 1;
			else if (strstr(catalog2->cat_name, "右"))
				direction = 2;
		}

		oe_catalog_maps.insert(oe_catalog);
		set<OE*> &oes = catalog_oe_map[oe_catalog];
		for (auto it2 = oes.begin(); it2 != oes.end(); ++it2)
		{
			OE *oe = *it2;
			Brand *brand = Brand_id_map[oe->brand_id];
			if (!brand)
				continue;

			int direction2 = 0;
			if (strstr(oe->oe_name, "左"))
				direction2 = 1;
			else if (strstr(oe->oe_name, "右"))
				direction2 = 2;

			if (direction && direction2 && (direction != direction2))
				continue;

			NameConvert *nameconvert = reinterpret_cast<NameConvert*>(NameConvert_tb->allocate());
			NameConvert_tb->push_back(reinterpret_cast<char*>(nameconvert));

			set<Vocabulary*> &Vocabularys = Vocabulary_map[oe->oe_name];
			Vocabulary *vocabulary = 0;
			strcpy(nameconvert->cat_name, catalog->cat_name);
			strcpy(nameconvert->cat_name2, catalog2->cat_name);
			if (catalog3)
			{
				strcpy(nameconvert->cat_name3, catalog3->cat_name);
				for (auto it3 = Vocabularys.begin(); it3 != Vocabularys.end(); ++it3)
				{
					vocabulary = *it3;
					if (vocabulary->cat_id == catalog3->cat_id)
						break;
					vocabulary = 0;
				}
			}
			else
			{
				for (auto it3 = Vocabularys.begin(); it3 != Vocabularys.end(); ++it3)
				{
					vocabulary = *it3;
					if (vocabulary->cat_id == catalog2->cat_id)
						break;
					vocabulary = 0;
				}
			}

			strcpy(nameconvert->epc_cat_name, data->epc_cat_name);
			strcpy(nameconvert->epc_cat_name2, data->epc_cat_name2);
			strcpy(nameconvert->oe_code, oe->oe_code);
			strcpy(nameconvert->oe_name, oe->oe_name);
			strcpy(nameconvert->info, oe->info);
			const_char_map<ExchangeData*> &vendors = brand_vendors_map[brand];
			auto it3 = vendors.find(oe->oe_code);
			if (it3 != vendors.end())
				strcpy(nameconvert->vendor_name, it3->second->name);

			//strncpy_ex(nameconvert->standard_name, Vocabulary_map2[oe->oe_name].c_str(), 120);
			if (vocabulary)
				strncpy_ex(nameconvert->standard_name, vocabulary->name, 48);
		}
	}

	for (auto it = catalog_oes.begin(); it != catalog_oes.end(); ++it)
	{
		Catalog *data = *it;
		if (catalog_maps.find(data) == catalog_maps.end())
			printf("\t目录“%s”未映射\n", data->cat_name);
	}

	for (auto it = name_catalog_map.begin(); it != name_catalog_map.end(); ++it)
	{
		Catalog *data = it->second;
		if (oe_catalog_maps.find(data) == oe_catalog_maps.end())
			printf("\tEPC目录“%s”未映射\n", it->first.c_str());
	}
}

bool JMParse::run(DDEDatabase &db_data, DDEDatabase &db_read, DDEDatabase &db_result, const char *data_dir, bool _check)
{
	check = _check;

	if (!initialize(db_result))
		return false;

	parse_dir(db_read, data_dir);

	generate_jmdata(db_result);
	generate_unidentifyoe(db_data, db_result);
	generate_nameconvert(db_data, db_result);

	return true;
}

bool JMParse::parse_dir(DDEDatabase &db_read, const char *data_dir)
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

bool JMParse::parse_file(DDEDatabase &db_read, const char *data_file)
{
	catalog = 0;
	catalog2 = 0;
	catalog3 = 0;
	e_document_ex doc;
	//DDEScheme *scheme = db_read.scheme();
	//DDETableScheme *table_scheme = scheme->find("JMData");
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

	e_node *node = db_read.scheme()->create_e_node(doc, "JMData");
	if (!node)
		return false;
	if (!doc.parse_node_file(node, data_file, 0, 1))
		return false;

	db_read.clear();
	E2DDE efile(db_read.scheme());
	efile.parse(db_read, doc, e_dde_parse_combine);

	JMData_tb = db_read.get("JMData");
	if (!JMData_tb)
		return false;

	size_t size = JMData_tb->size();
	if (!size)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		jmdata = reinterpret_cast<JMData*>(JMData_tb->at(i));
		
		parse_cat1();
		if (!catalog)
		{
			printf("\t目录解析失败！\n");
			continue;
		}

		parse_cat2();
		parse_cat3();
		parse_vocabulary();
	}

	return true;
}

