#ifndef __MODEL_CONVERT_H__
#define __MODEL_CONVERT_H__

class ModelConvert
{
public:
	ModelConvert();
	~ModelConvert();

	bool run(const char *_data_file, const char *_conf_file);

protected:
	bool read_configure();
	bool read_base();
	bool read_data();
	bool write_result();

	const char *data_file;
	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;
	bool import_check;
	int mode;
	DDEDatabase db_result;
};

#endif	// __MODEL_CONVERT_H__

