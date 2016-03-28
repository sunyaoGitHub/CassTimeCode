#ifndef __JM_CONVERT_H__
#define __JM_CONVERT_H__

class JMConvert
{
public:
	JMConvert();
	~JMConvert();

	bool run(const char *data_file, const char *data_dir, const char *map_file, const char *vendor_file, const char *_conf_file);

protected:
	bool read_configure();
	bool read_dir(const char *data_dir);
	bool read_data(const char *data_file);
	bool read_map(const char *map_file);
	bool read_vendor(const char *vendor_file);
	bool write_result();

	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;
	bool import_check;

	DDEDatabase db_data;
	DDEDatabase db_read;
	DDEDatabase db_result;
};

#endif	// __JM_CONVERT_H__

