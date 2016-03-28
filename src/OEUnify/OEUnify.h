#ifndef __OEUNIFY_H__
#define __OEUNIFY_H__

class OEUnify
{
public:
	OEUnify();
	~OEUnify();
	bool run(const char *data_dir, const char *_conf_file);

protected:
	bool read_configure();
	bool read_base();
	bool read_dir(const char *data_dir);
	bool read_file(const char *data_file);
	void unify();

	bool check_OE_format(const char *str_);//SY  ADD
	bool write_result();


	void unify_audi(const_char_map<OEData*> &oedatas);
	void unify_benz(const_char_map<OEData*> &oedatas);
	void unify_bmw(const_char_map<OEData*> &oedatas);
	void unify_jaguar(const_char_map<OEData*> &oedatas);
	void unify_landrover(const_char_map<OEData*> &oedatas);
	void unify_porsche(const_char_map<OEData*> &oedatas);
	void unify_volvo(const_char_map<OEData*> &oedatas);
	void unify_vw(const_char_map<OEData*> &oedatas);

	const char *conf_file;
	char conf_dir[1024];
	dsa_xml_document<> xml_conf;
	xml_node<> *root_conf;
	bool import_check;

	const_char_map<Brand*> Brand_code_map;
	const_char_map<Brand*> Brand_name_map;
	//map<Brand*, const_char_map<OEDataEx*>> brand_oedata_map;
	map<Brand*, const_char_map<OEData*>> brand_oedata_map;

	DDEDatabase db_read;
	DDEDatabase db_result;
};

#endif	// __OEUNIFY_H__

