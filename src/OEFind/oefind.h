#ifndef __OEFIND_H__
#define __OEFIND_H__

class OEFind
{
public:
	OEFind();
	~OEFind();

	bool run(const char *find_oe_string,const char *brand_choice,const bool all_brand_flag,const bool match_flag);
	bool get_conf_dir();
	bool read_configure();
	bool parse_database();
	bool read_oedata();

	const_char_map<const_char_map<OEData *>> brand_oedata_map;
	vector <OEData*> OEData_find_result;
protected:
	char conf_dir[1024];
	DDEDatabase db_read;
};

#endif	// __OEFIND_H__