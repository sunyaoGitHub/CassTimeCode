#ifndef __ARTICLE_CONVERT_H__
#define __ARTICLE_CONVERT_H__

class ArticleConvert
{
public:
	ArticleConvert();
	~ArticleConvert();

	bool run(const char *data_dir, const char *_conf_file);

protected:
	bool read_configure();
	bool read_base();
	bool read_model_map();
	bool read_dir(const char *data_dir);
	bool write_result();
	bool read_catalog_map();
	bool read_oename();

	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;
	bool import_check;

	DDEDatabase db_data;
	DDEDatabase db_read;
	DDEDatabase db_result;
};

#endif	// __ARTICLE_CONVERT_H__

