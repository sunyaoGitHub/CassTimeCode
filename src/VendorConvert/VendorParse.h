#ifndef __VENDOR_PARSE_H__
#define __VENDOR_PARSE_H__

class VendorConvert;
class VendorParse
{
public:
	VendorParse();
	~VendorParse();

	bool run(VendorConvert *_vc, const char *data_dir);

protected:
	bool initialize();
	bool parse_dir(const char *data_dir);
	bool parse_file(const char *data_file);
	bool parse_huali(Configure &conf, const char *vendor_name);
	bool parse(Configure &conf, const char *brand_name, const char *vendor_name);

	VendorConvert *vc;
	ExchangeData *exchangedata;
	DDETable *ExchangeData_tb;

	const_char_map<Brand *> Brand_map;
	const_char_map<Brand *> Brand_cn_map;
	const_char_map<const_char_map<OEData *>> OEData_map;
	map<string, string> Brand_not_find_map;
};

class e_document_ex: public e_document
{
public:
	e_document_ex();
	virtual ~e_document_ex();

	virtual void parse_error(const char *what, void *where) override;
};

#endif	// __VENDOR_PARSE_H__

