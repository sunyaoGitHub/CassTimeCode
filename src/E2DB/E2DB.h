#ifndef __E2DB_H__
#define __E2DB_H__

class E2DB
{
public:
	E2DB();
	~E2DB();

	bool run(const char *_data_file, const char *_conf_file);

protected:
	bool read_configure();
	bool read_data();
	bool write_db();

	//判断表是否存在
	bool table_exist(const DDETable *table);

	//全列插入
	bool insert_table(const DDETable *table);

	const char *data_file;
	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;

	DDEDatabase db;
	otl_connect m_otl_connect;
};

#endif	// __E2DB_H__

