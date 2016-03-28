#ifndef __JM_PARSE_H__
#define __JM_PARSE_H__

class JMParse
{
public:
	JMParse();
	~JMParse();

	bool run(DDEDatabase &db_data, DDEDatabase &db_read, DDEDatabase &db_result, const char *data_dir, bool _check);

protected:
	bool initialize(DDEDatabase &db_result);
	bool parse_dir(DDEDatabase &db_read, const char *data_dir);
	bool parse_file(DDEDatabase &db_read, const char *data_file);
	void parse_cat1();
	void parse_cat2();
	void parse_cat3();
	void parse_vocabulary();
	void generate_jmdata(DDEDatabase &db_result);
	void generate_unidentifyoe(DDEDatabase &db_data, DDEDatabase &db_result);
	void generate_nameconvert(DDEDatabase &db_data, DDEDatabase &db_result);

	bool check;
	JMData *jmdata;
	DDETable *JMData_tb;

	Vocabulary *vocabulary;
	DDETable *Vocabulary_tb;
	const_char_map<const_char_map<Vocabulary *>> Vocabulary_map;

	Catalog *catalog;
	Catalog *catalog2;
	Catalog *catalog3;
	DDETable *Catalog_tb;
	const_char_map<Catalog *> Catalog_map;
	const_char_map<Catalog *> Catalog2_map;
	const_char_map<Catalog *> Catalog3_map;
	map<int32_t, Catalog*> Catalog_id_map;
	set<Catalog*> catalog_oes;
};

class e_document_ex: public e_document
{
public:
	e_document_ex();
	virtual ~e_document_ex();

	virtual void parse_error(const char *what, void *where) override;
};

#endif	// __JM_PARSE_H__

