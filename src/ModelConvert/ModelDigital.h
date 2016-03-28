#ifndef __MODEL_DIGITAL_H__
#define __MODEL_DIGITAL_H__

class ModelDigital
{
public:
	ModelDigital();
	~ModelDigital();

	bool run(DDEDatabase &_db_result);

protected:
	bool get_constant_info();
	uint8_t get_value(const_char_map<uint8_t> &value_map, const int32_t class_id, const char *descr);

	DDEDatabase *db_result;
	const_char_map<uint8_t> vehicle_type_map;
	const_char_map<uint8_t> vehicle_level_map;
	const_char_map<uint8_t> product_stat_map;
	const_char_map<uint8_t> sale_stat_map;
	const_char_map<uint8_t> import_map;
	const_char_map<uint8_t> intake_charge_type_map;
	const_char_map<uint8_t> fuel_type_map;
	const_char_map<uint8_t> fuel_system_type_map;
	const_char_map<uint8_t> tran_type_map;
	const_char_map<uint8_t> tran_desc_map;
	const_char_map<uint8_t> brake_type_map;
	const_char_map<uint8_t> front_suspension_type_map;
	const_char_map<uint8_t> rear_suspension_type_map;
	const_char_map<uint8_t> steering_type_map;
	const_char_map<uint8_t> steering_aid_type_map;
	const_char_map<uint8_t> engine_position_map;
	const_char_map<uint8_t> drive_mode_map;
	const_char_map<uint8_t> body_map;
	const_char_map<uint8_t> door_num_map;
	const_char_map<uint8_t> rim_mat_map;
	const_char_map<uint8_t> option_map;
	const_char_map<uint8_t> emiss_stand_map;
	const_char_map<uint8_t> cylinder_align_map;
	const_char_map<uint8_t> tire_spec_map;
	const_char_map<uint8_t> rim_spec_map;
	const_char_map<uint8_t> drive_type_map;
	const_char_map<uint8_t> spare_tire_spec_map;
};

#endif	// __MODEL_DIGITAL_H__

