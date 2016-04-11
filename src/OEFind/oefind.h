#ifndef __OEFIND_H__
#define __OEFIND_H__

class OEFind
{
public:
	OEFind();
	~OEFind();

	bool run(const char *find_oe_string,const int brand_choice,const bool all_brand_flag,const bool match_flag);
	bool get_conf_dir();
	bool read_configure();
	bool check_config_Vehicle();
	bool check_config_OEData();
	bool check_config_BaseData();
	bool file_encryption();
	bool parse_database();
	bool read_basedata();
	bool read_oedata();

	vector <OEData*> OEData_find_result;
	DDEDatabase db_read;
protected:
	char conf_dir[1024];
	const_char_map<Brand*> Brand_name_map;
	map<Brand*,vector<OEData*>> brand_oedata_map;

};

#endif	// __OEFIND_H__