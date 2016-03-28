#include "stdafx.h"
#include "ModelDigital.h"

ModelDigital::ModelDigital() : db_result(0)
{
}

ModelDigital::~ModelDigital()
{
}

const int32_t vehicle_type = 2;
const int32_t vehicle_level = 3;
const int32_t product_stat = 4;
const int32_t sale_stat = 5;
const int32_t import_type = 6;
const int32_t intake_charge_type = 7;
const int32_t fuel_type = 8;
const int32_t fuel_system_type = 9;
const int32_t tran_type = 10;
const int32_t tran_desc = 11;
const int32_t brake_type = 12;
const int32_t front_suspension_type = 13;
const int32_t rear_suspension_type = 14;
const int32_t steering_type = 15;
const int32_t steering_aid_type = 16;
const int32_t engine_position = 17;
const int32_t drive_mode = 18;
const int32_t body = 19;
const int32_t door_num = 20;
const int32_t rim_mat = 21;
const int32_t option = 22;
const int32_t emiss_stand = 23;
const int32_t cylinder_align = 24;
const int32_t tire_spec = 25;
const int32_t rim_spec = 26;
const int32_t drive_type = 27;
const int32_t spare_tire_spec = 28;

bool ModelDigital::get_constant_info()
{
	DDETable *ConstantValue_tb = db_result->get("ConstantValue");
	if (!ConstantValue_tb)
		return false;

	size_t size = ConstantValue_tb->size();
	if (!size)
		return false;

	for (size_t i = 0; i < size; ++i)
	{
		ConstantValue *record = reinterpret_cast<ConstantValue*>(ConstantValue_tb->at(i));
		switch (record->class_id)
		{
		case vehicle_type:
			vehicle_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case vehicle_level:
			vehicle_level_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case product_stat:
			product_stat_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case sale_stat:
			sale_stat_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case import_type:
			import_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case intake_charge_type:
			intake_charge_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case fuel_type:
			fuel_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case fuel_system_type:
			fuel_system_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case tran_type:
			tran_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case tran_desc:
			tran_desc_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case brake_type:
			brake_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case front_suspension_type:
			front_suspension_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case rear_suspension_type:
			rear_suspension_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case steering_type:
			steering_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case steering_aid_type:
			steering_aid_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case engine_position:
			engine_position_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case drive_mode:
			drive_mode_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case body:
			body_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case door_num:
			door_num_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case rim_mat:
			rim_mat_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case option:
			option_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case emiss_stand:
			emiss_stand_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case cylinder_align:
			cylinder_align_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case tire_spec:
			tire_spec_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case rim_spec:
			rim_spec_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case drive_type:
			drive_type_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		case spare_tire_spec:
			spare_tire_spec_map[record->descr] = static_cast<uint8_t>(record->value);
			break;

		default:
			break;
		}
	}

	return true;
}

uint8_t ModelDigital::get_value(const_char_map<uint8_t> &value_map, const int32_t class_id, const char *descr)
{
	if (*descr == '\0')
		return 0;

	auto it = value_map.find(descr);
	if (it != value_map.end())
		return it->second;

	DDETable *ConstantValue_tb = db_result->get("ConstantValue");
	if (ConstantValue_tb)
	{
		ConstantValue *record = reinterpret_cast<ConstantValue*>(ConstantValue_tb->allocate());
		ConstantValue_tb->push_back(reinterpret_cast<char*>(record));
		record->class_id = class_id;
		strncpy_ex(record->descr, descr, 40);
		uint8_t value = static_cast<uint8_t>(value_map.size()) + 1;
		record->value = value;
		value_map[record->descr] = value;
		return value;
	}

	return static_cast<uint8_t>(-1);
}

bool ModelDigital::run(DDEDatabase &_db_result)
{
	db_result = &_db_result;

	DDETable *LevelModelData_tb = db_result->get("LevelModelData");
	if (!LevelModelData_tb)
		return false;

	size_t size = LevelModelData_tb->size();
	if (!size)
		return true;

	DDETable *LevelModel_tb = db_result->get("LevelModel");
	if (!LevelModel_tb)
		return false;

	if (!get_constant_info())
		return false;

	LevelModel *model = reinterpret_cast<LevelModel*>(LevelModel_tb->allocate(size));
	LevelModel_tb->push_back(reinterpret_cast<char*>(model), size);
	for (size_t i = 0; i < size; ++i)
	{
		LevelModelData *record = reinterpret_cast<LevelModelData*>(LevelModelData_tb->at(i));
		memcpy(model->code, record->code, 316);
		//strncpy_ex(model->code, record->code, 20);
		//strncpy_ex(model->manu, record->manu, 20);
		//strncpy_ex(model->brand, record->brand, 20);
		//strncpy_ex(model->series, record->series, 20);
		//strncpy_ex(model->model, record->model, 56);
		//strncpy_ex(model->chassis, record->chassis, 20);
		//strncpy_ex(model->sale_name, record->sale_name, 40);
		//strncpy_ex(model->variant, record->variant, 20);
		//strncpy_ex(model->sale_name_ah, record->sale_name_ah, 40);
		//strncpy_ex(model->text1, record->text1, 20);
		//strncpy_ex(model->text2, record->text2, 40);

		model->model_year = record->model_year;
		model->emiss_stand = get_value(emiss_stand_map, emiss_stand, record->emiss_stand);
		model->vehicle_type = get_value(vehicle_type_map, vehicle_type, record->vehicle_type);
		model->vehicle_level = get_value(vehicle_level_map, vehicle_level, record->vehicle_level);
		model->msrp = record->msrp;
		model->year_start = record->year_start;
		model->month_start = record->month_start;
		model->year_make = record->year_make;
		model->year_stop = record->year_stop;
		model->product_stat = get_value(product_stat_map, product_stat, record->product_stat);
		model->sale_stat = get_value(sale_stat_map, sale_stat, record->sale_stat);
		strncpy_ex(model->country, record->country, 16);
		model->import = get_value(import_map, import_type, record->import);
		strncpy_ex(model->engine, record->engine, 16);
		model->cylinder_volume = record->cylinder_volume;
		model->displacement = record->displacement;
		model->intake_charge_type = get_value(intake_charge_type_map, intake_charge_type, record->intake_charge_type);
		model->fuel_type = get_value(fuel_type_map, fuel_type, record->fuel_type);
		model->fuel_grade = record->fuel_grade;
		model->max_horsepower = record->max_horsepower;
		model->max_power = record->max_power;
		model->max_rpm = static_cast<int16_t>(atoi(record->max_rpm));
		char *pos = strchr(record->max_rpm, '-');
		if (pos)
			model->max_rpm2 = static_cast<int16_t>(atoi(pos+1));
		else
			model->max_rpm2 = 0;

		model->max_torque = record->max_torque;
		model->max_torque_rpm = static_cast<int16_t>(atoi(record->max_torque_rpm));
		pos = strchr(record->max_torque_rpm, '-');
		if (pos)
			model->max_torque_rpm2 = static_cast<int16_t>(atoi(pos+1));
		else
			model->max_torque_rpm2 = 0;

		model->cylinder_align = get_value(cylinder_align_map, cylinder_align, record->cylinder_align);
		model->cylinder_num = record->cylinder_num;
		model->valve_num = record->valve_num;
		model->fuel_system_type = get_value(fuel_system_type_map, fuel_system_type, record->fuel_system_type);
		model->compression_ratio = record->compression_ratio;
		model->fuel_economy_mix = record->fuel_economy_mix;
		model->fuel_economy_urb = record->fuel_economy_urb;
		model->fuel_economy_suburb = record->fuel_economy_suburb;
		model->acc_time = record->acc_time;
		model->max_speed = record->max_speed;
		model->tran_type = get_value(tran_type_map, tran_type, record->tran_type);
		model->tran_desc = get_value(tran_desc_map, tran_desc, record->tran_desc);
		model->gears_num = record->gears_num;
		model->front_brake_type = get_value(brake_type_map, brake_type, record->front_brake_type);
		model->rear_brake_type = get_value(brake_type_map, brake_type, record->rear_brake_type);
		model->front_suspension_type = get_value(front_suspension_type_map, front_suspension_type, record->front_suspension_type);
		model->rear_suspension_type = get_value(rear_suspension_type_map, rear_suspension_type, record->rear_suspension_type);
		model->steering_type = get_value(steering_type_map, steering_type, record->steering_type);
		model->steering_aid_type = get_value(steering_aid_type_map, steering_aid_type, record->steering_aid_type);
		model->min_grd_clearance = record->min_grd_clearance;
		model->min_turn_radius = record->min_turn_radius;
		model->engine_position = get_value(engine_position_map, engine_position, record->engine_position);
		model->drive_mode = get_value(drive_mode_map, drive_mode, record->drive_mode);
		model->drive_type = get_value(drive_type_map, drive_type, record->drive_type);
		model->body = get_value(body_map, body, record->body);
		model->length = record->length;
		model->width = record->width;
		model->height = record->height;
		model->wheelbase = record->wheelbase;
		model->front_track = record->front_track;
		model->rear_track = record->rear_track;
		model->curb_weight = record->curb_weight;
		model->gross_weight = record->gross_weight;
		model->fuel_tank_cap = record->fuel_tank_cap;
		model->cargo_cap = static_cast<int16_t>(atoi(record->cargo_cap));
		pos = strchr(record->cargo_cap, '-');
		if (pos)
			model->cargo_cap2 = static_cast<int16_t>(atoi(pos+1));
		else
			model->cargo_cap2 = 0;

		model->door_num = get_value(door_num_map, door_num, record->door_num);
		model->seat_num = record->seat_num;
		model->num_speaker = record->num_speaker;
		model->front_tire_spec = get_value(tire_spec_map, tire_spec, record->front_tire_spec);
		model->rear_tire_spec = get_value(tire_spec_map, tire_spec, record->rear_tire_spec);
		model->front_rim_spec = get_value(rim_spec_map, rim_spec, record->front_rim_spec);
		model->rear_rim_spec = get_value(rim_spec_map, rim_spec, record->rear_rim_spec);
		model->rim_mat = get_value(rim_mat_map, rim_mat, record->rim_mat);
		model->spare_tire_spec = get_value(spare_tire_spec_map, spare_tire_spec, record->spare_tire_spec);

		model->drivers_airbag = get_value(option_map, option, record->drivers_airbag);
		model->passenger_airbag = get_value(option_map, option, record->passenger_airbag);
		model->front_side_airbag = get_value(option_map, option, record->front_side_airbag);
		model->rear_side_airbag = get_value(option_map, option, record->rear_side_airbag);
		model->front_head_airbag = get_value(option_map, option, record->front_head_airbag);
		model->rear_head_airbag = get_value(option_map, option, record->rear_head_airbag);
		model->knee_airbag = get_value(option_map, option, record->knee_airbag);
		model->tpms = get_value(option_map, option, record->tpms);
		model->rsc = get_value(option_map, option, record->rsc);
		model->sbr = get_value(option_map, option, record->sbr);
		model->isofix = get_value(option_map, option, record->isofix);
		model->latch = get_value(option_map, option, record->latch);
		model->immobilizer = get_value(option_map, option, record->immobilizer);
		model->central_lock = get_value(option_map, option, record->central_lock);
		model->rke = get_value(option_map, option, record->rke);
		model->peps = get_value(option_map, option, record->peps);
		model->abs = get_value(option_map, option, record->abs);
		model->ebd = get_value(option_map, option, record->ebd);
		model->eba = get_value(option_map, option, record->eba);
		model->tcs = get_value(option_map, option, record->tcs);
		model->esp = get_value(option_map, option, record->esp);
		model->epb = get_value(option_map, option, record->epb);
		model->hdc = get_value(option_map, option, record->hdc);
		model->avs = get_value(option_map, option, record->avs);
		model->air_suspension = get_value(option_map, option, record->air_suspension);
		model->variable_ratio_steering = get_value(option_map, option, record->variable_ratio_steering);
		model->blis = get_value(option_map, option, record->blis);
		model->autobrake = get_value(option_map, option, record->autobrake);
		model->afs = get_value(option_map, option, record->afs);
		model->leather_steering_wheel = get_value(option_map, option, record->leather_steering_wheel);
		model->steering_wheel_adjust = get_value(option_map, option, record->steering_wheel_adjust);
		model->steering_wheel_adjust2 = get_value(option_map, option, record->steering_wheel_adjust2);
		model->steering_wheel_electric_regulation = get_value(option_map, option, record->steering_wheel_electric_regulation);
		model->mfw = get_value(option_map, option, record->mfw);
		model->steering_wheel_gearshift = get_value(option_map, option, record->steering_wheel_gearshift);
		model->leather_seats = get_value(option_map, option, record->leather_seats);
		model->sports_seats = get_value(option_map, option, record->sports_seats);
		model->seat_height_adjust = get_value(option_map, option, record->seat_height_adjust);
		model->lumbar_support_adjust = get_value(option_map, option, record->lumbar_support_adjust);
		model->shoulder_support_adjust = get_value(option_map, option, record->shoulder_support_adjust);
		model->drivers_esa = get_value(option_map, option, record->drivers_esa);
		model->passenger_esa = get_value(option_map, option, record->passenger_esa);
		model->second_row_backrest_adjust = get_value(option_map, option, record->second_row_backrest_adjust);
		model->second_row_move = get_value(option_map, option, record->second_row_move);
		model->rear_esa = get_value(option_map, option, record->rear_esa);
		model->seat_position_memory = get_value(option_map, option, record->seat_position_memory);
		model->front_heated_seat = get_value(option_map, option, record->front_heated_seat);
		model->rear_heated_seat = get_value(option_map, option, record->rear_heated_seat);
		model->seat_ventilate = get_value(option_map, option, record->seat_ventilate);
		model->seat_massage = get_value(option_map, option, record->seat_massage);
		model->rear_seat_recline = get_value(option_map, option, record->rear_seat_recline);
		model->rear_seat_recline_part = get_value(option_map, option, record->rear_seat_recline_part);
		model->third_seat_row = get_value(option_map, option, record->third_seat_row);
		model->front_central_rest = get_value(option_map, option, record->front_central_rest);
		model->rear_central_rest = get_value(option_map, option, record->rear_central_rest);
		model->rear_cup_shelf = get_value(option_map, option, record->rear_cup_shelf);
		model->ambient_lighting = get_value(option_map, option, record->ambient_lighting);
		model->rear_window_sunshade = get_value(option_map, option, record->rear_window_sunshade);
		model->rear_side_sunshade = get_value(option_map, option, record->rear_side_sunshade);
		model->sun_visor = get_value(option_map, option, record->sun_visor);
		model->electric_trunk = get_value(option_map, option, record->electric_trunk);
		model->sport_appearance_package = get_value(option_map, option, record->sport_appearance_package);
		model->electric_suction_door = get_value(option_map, option, record->electric_suction_door);
		model->electric_sunroof = get_value(option_map, option, record->electric_sunroof);
		model->panoramic_sunroof = get_value(option_map, option, record->panoramic_sunroof);
		model->hid = get_value(option_map, option, record->hid);
		model->led = get_value(option_map, option, record->led);
		model->drl = get_value(option_map, option, record->drl);
		model->automatic_headlights = get_value(option_map, option, record->automatic_headlights);
		model->afs = get_value(option_map, option, record->afs);
		model->front_fog_lamp = get_value(option_map, option, record->front_fog_lamp);
		model->headlight_height_adjust = get_value(option_map, option, record->headlight_height_adjust);
		model->sra = get_value(option_map, option, record->sra);
		model->front_power_window = get_value(option_map, option, record->front_power_window);
		model->rear_power_window = get_value(option_map, option, record->rear_power_window);
		model->anti_pinch_window = get_value(option_map, option, record->anti_pinch_window);
		model->insulating_glass = get_value(option_map, option, record->insulating_glass);
		model->electric_rearview_mirror = get_value(option_map, option, record->electric_rearview_mirror);
		model->heated_rearview_mirror = get_value(option_map, option, record->heated_rearview_mirror);
		model->auto_dimming_rearview_mirror = get_value(option_map, option, record->auto_dimming_rearview_mirror);
		model->electric_fold_rearview_mirror = get_value(option_map, option, record->electric_fold_rearview_mirror);
		model->position_memory_rearview_mirror = get_value(option_map, option, record->position_memory_rearview_mirror);
		model->rear_wiper = get_value(option_map, option, record->rear_wiper);
		model->sensing_wiper = get_value(option_map, option, record->sensing_wiper);
		model->ccs = get_value(option_map, option, record->ccs);
		model->park_assist = get_value(option_map, option, record->park_assist);
		model->reverse_video_image = get_value(option_map, option, record->reverse_video_image);
		model->ecu_screen = get_value(option_map, option, record->ecu_screen);
		model->hud = get_value(option_map, option, record->hud);
		model->gps = get_value(option_map, option, record->gps);
		model->gps_is = get_value(option_map, option, record->gps_is);
		model->central_control_screen = get_value(option_map, option, record->central_control_screen);
		model->hmi = get_value(option_map, option, record->hmi);
		model->built_in_disk = get_value(option_map, option, record->built_in_disk);
		model->bluetooth = get_value(option_map, option, record->bluetooth);
		model->tv = get_value(option_map, option, record->tv);
		model->rear_lcd_screen = get_value(option_map, option, record->rear_lcd_screen);
		model->aux = get_value(option_map, option, record->aux);
		model->mp3 = get_value(option_map, option, record->mp3);
		model->single_disc_cd = get_value(option_map, option, record->single_disc_cd);
		model->multi_disc_cd = get_value(option_map, option, record->multi_disc_cd);
		model->virtual_multi_disc_cd = get_value(option_map, option, record->virtual_multi_disc_cd);
		model->single_disc_dvd = get_value(option_map, option, record->single_disc_dvd);
		model->multi_disc_dvd = get_value(option_map, option, record->multi_disc_dvd);
		model->ac = get_value(option_map, option, record->ac);
		model->thermatic = get_value(option_map, option, record->thermatic);
		model->rear_ac = get_value(option_map, option, record->rear_ac);
		model->rear_air_outlet = get_value(option_map, option, record->rear_air_outlet);
		model->temp_zc = get_value(option_map, option, record->temp_zc);
		model->pollen_filter = get_value(option_map, option, record->pollen_filter);
		model->fridge = get_value(option_map, option, record->fridge);
		model->park_assist_vision = get_value(option_map, option, record->park_assist_vision);
		model->nvs = get_value(option_map, option, record->nvs);
		model->splitview = get_value(option_map, option, record->splitview);
		model->acc = get_value(option_map, option, record->acc);
		model->pancam = get_value(option_map, option, record->pancam);
		model->parking_sensor = get_value(option_map, option, record->parking_sensor);
		model->telematics = get_value(option_map, option, record->telematics);
		strncpy_ex(model->color, record->color, 160);
		model++;
	}

	return true;
}

