#ifndef __MODEL_PARSE_H__
#define __MODEL_PARSE_H__

class ModelParse
{
public:
	ModelParse();
	~ModelParse();

	bool run(DDEDatabase &db, int _mode, bool _check);

protected:
	bool initialize(DDEDatabase &db);
	bool parse_brand();
	bool parse_series();
	bool parse_manufacturer();
	bool parse_engine();
	void get_model_code(const char *name, char *code);
	bool parse_model();
	bool parse_epc_model();

	bool check;
	int mode;
	LevelModel *level;
	DDETable *LevelModel_tb;

	Brand *brand;
	DDETable *Brand_tb;
	const_char_map<Brand *> Brand_map;
	const_char_map<Brand *> Brand_code_map;
	map<int32_t, Brand*> Brand_id_map;

	Series *series;
	DDETable *Series_tb;
	const_char_map<Series *> Series_map;
	map<int32_t, Series*> Series_id_map;

	Manufacturer *manu;
	DDETable *Manufacturer_tb;
	const_char_map<Manufacturer *> Manufacturer_map;
	map<int32_t, Manufacturer*> Manufacturer_id_map;

	Engine *engine;
	DDETable *Engine_tb;
	const_char_map<Engine *> Engine_map;
	map<int32_t, Engine*> Engine_id_map;

	Model *model;
	DDETable *Model_tb;
	const_char_map<Model *> Model_map;
	map<int32_t, Model*> Model_id_map;

	EPCModel *epcmodel;
	DDETable *EPCModel_tb;
	const_char_map<EPCModel *> EPCModel_map;

	ModelTransfer *modeltransfer;
	DDETable *ModelTransfer_tb;
	const_char_map<ModelTransfer *> ModelTransfer_map;

	DDETable *Level2YunData_tb;
};

#endif	// __MODEL_PARSE_H__

