#ifndef __VENDOR_CONVERT_H__
#define __VENDOR_CONVERT_H__

struct Configure
{
	int run;
	int type;
	int mode;
};

class VendorConvert
{
public:
	VendorConvert();
	~VendorConvert();

	bool run(const char *oe_file, const char *data_dir, const char *_conf_file);

protected:
	bool read_configure();
	bool read_base();
	bool read_oe(const char *oe_file);
	bool read_vendor_data(const char *data_dir);
	bool write_result();
	bool write_delta();

	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;
	bool import_check;
	bool trim_space;
	bool oe_warning;
	bool price_warning;
	bool repeat_warning;

	DDEDatabase db_read;
	DDEDatabase db_result;
	const_char_map<Configure> Configure_map;
	friend class VendorParse;
};

#endif	// __VENDOR_CONVERT_H__

