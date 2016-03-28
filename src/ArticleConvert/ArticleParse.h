#ifndef __ARTICLE_PARSE_H__
#define __ARTICLE_PARSE_H__

class ArticleParse
{
public:
	ArticleParse();
	~ArticleParse();

	bool run(DDEDatabase &db_data, DDEDatabase &db_read, DDEDatabase &db_result, const char *data_dir, bool _check);

protected:
	bool initialize(DDEDatabase &db_result);
	bool parse_dir(DDEDatabase &db_read, const char *data_dir);
	bool parse_file(DDEDatabase &db_read, const char *data_file);
	bool parse_cat1();
	bool parse_cat2();
	bool parse_article();
	bool parse_oe2model();
	bool parse_oe2catalog();
	void read_catalog_map(DDEDatabase &db_data);
	void read_oename(DDEDatabase &db_data);

	bool check;
	YunArticle *yunarticle;
	DDETable *YunArticle_tb;

	EPCModel *model;
	const_char_map<EPCModel *> EPCModel_map;

	const_char_map<Brand *> Brand_map;
	map<int32_t, Brand*> Brand_id_map;

	OE *oe;
	DDETable *OE_tb;
	map<int32_t, const_char_map<OE*>> Brand_OE_map;
	map<int32_t, OE*> OE_id_map;

	OE2Model *oe2model;
	DDETable *OE2Model_tb;
	map<int32_t, set<OE2Model*>> OE2Model_id_map;

	Catalog *catalog;
	Catalog *catalog2;
	DDETable *Catalog_tb;
	const_char_map<Catalog *> Catalog_map;
	const_char_map<set<Catalog *>> Catalog2_map;
	map<int32_t, Catalog*> Catalog_id_map;

	OE2Catalog *oe2catalog;
	DDETable *OE2Catalog_tb;
	map<int32_t, set<OE2Catalog*>> OE2Catalog_id_map;

	string option_code_prefix;
	string option_code;
	bool first_oe;
	size_t size_cat2;
};

class e_document_ex: public e_document
{
public:
	e_document_ex();
	virtual ~e_document_ex();

	virtual void parse_error(const char *what, void *where) override;
};

#endif	// __ARTICLE_PARSE_H__

