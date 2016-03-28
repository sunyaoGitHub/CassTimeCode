#include "stdafx.h"
#include "ModelParse.h"

ModelParse::ModelParse()
{
}

ModelParse::~ModelParse()
{
}

bool ModelParse::initialize(DDEDatabase &db)
{
	LevelModel_tb = db.get("LevelModel");
	if (!LevelModel_tb)
		return false;

	Brand_tb = db.get("Brand");
	if (!Brand_tb)
		return false;

	Series_tb = db.get("Series");
	if (!Series_tb)
		return false;

	Manufacturer_tb = db.get("Manufacturer");
	if (!Manufacturer_tb)
		return false;

	Engine_tb = db.get("Engine");
	if (!Engine_tb)
		return false;

	Model_tb = db.get("Model");
	if (!Model_tb)
		return false;

	EPCModel_tb = db.get("EPCModel");
	if (!EPCModel_tb)
		return false;

	ModelTransfer_tb = db.get("ModelTransfer");
	if (!ModelTransfer_tb)
		return false;

	Level2YunData_tb = db.find("Level2YunData");
	if (!Level2YunData_tb)
		return false;

	size_t size = Brand_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		Brand_map[data->brand_name_cn] = data;
		Brand_code_map[data->brand_code] = data;
		Brand_id_map[data->brand_id] = data;
	}

	size = Series_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Series *data = reinterpret_cast<Series*>(Series_tb->at(i));
		Series_map[data->series_name_cn] = data;
		Series_id_map[data->series_id] = data;
	}

	size = Manufacturer_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Manufacturer *data = reinterpret_cast<Manufacturer*>(Manufacturer_tb->at(i));
		Manufacturer_map[data->manu_name] = data;
		Manufacturer_id_map[data->manu_id] = data;
	}

	size = Engine_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Engine *data = reinterpret_cast<Engine*>(Engine_tb->at(i));
		Engine_map[data->engine_code] = data;
		Engine_id_map[data->engine_id] = data;
	}

	size = Model_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Model *data = reinterpret_cast<Model*>(Model_tb->at(i));
		Model_map[data->model_code] = data;
		Model_id_map[data->model_id] = data;
	}

	return true;
}

bool ModelParse::parse_brand()
{
	char *name = level->brand;
	if (*name == '\0')
		return false;

	auto it = Brand_map.find(name);
	if (it != Brand_map.end())
	{
		brand = it->second;
		return true;
	}

	brand = reinterpret_cast<Brand*>(Brand_tb->allocate());
	Brand_tb->push_back(reinterpret_cast<char*>(brand));
	strncpy_ex(brand->brand_code, name, 16);
	strncpy_ex(brand->brand_name_en, name, 20);
	strncpy_ex(brand->brand_name_cn, name, 16);
	Brand_map[brand->brand_name_cn] = brand;
	Brand_code_map[brand->brand_code] = brand;
	brand->brand_id = static_cast<int32_t>(Brand_tb->size());
	Brand_id_map[brand->brand_id] = brand;

	printf("\t发现新品牌“%s”。\n", brand->brand_name_cn);
	return true;
}

bool ModelParse::parse_series()
{
	char *name = level->series;
	if (*name == '\0')
		return false;

	bool add = true;
	auto it = Series_map.find(name);
	if (it != Series_map.end())
	{
		series = it->second;
		add = false;
	}
	else
	{
		char series_name[40];
		const char *pos = strchr(name, '[');
		if (pos)
		{
			const char *pos2 = strchr(pos+1, ']');
			if (pos2)
				strncpy_ex(series_name, pos+1, pos2 - pos);
			else
				strcpy(series_name, pos+1);

			it = Series_map.find(series_name);
			if (it != Series_map.end())
			{
				series = it->second;
				Series_map[name] = series;
				add = false;
			}
		}
		else
			strncpy_ex(series_name, name, 40);

		if (add)
		{
			series = reinterpret_cast<Series*>(Series_tb->allocate());
			Series_tb->push_back(reinterpret_cast<char*>(series));
			strncpy_ex(series->series_name_cn, series_name, 24);
			Series_map[series->series_name_cn] = series;
			if (pos)
				Series_map[name] = series;
			series->series_id = static_cast<int32_t>(Series_tb->size());
			Series_id_map[series->series_id] = series;
		}
	}

	if (add)
	{
		series->brand_id = brand->brand_id;
		printf("\t发现“%s”新车系“%s”。\n", brand->brand_name_cn, series->series_name_cn);
	}
	else if (check)
	{
		if (series->brand_id != brand->brand_id)
		{
			auto it2 = Brand_id_map.find(series->brand_id);
			if (it2 == Brand_id_map.end())
			{
				printf("\t车系“%s”所属品牌ID“%d”无效，修正为“%d”。\n", series->series_name_cn, series->brand_id, brand->brand_id);
				series->brand_id = brand->brand_id;
			}
			else
			{
				Brand *brand2 = it2->second;
				printf("\t发现不同品牌“%s”、“%s”具有重复车系名称“%s”。\n", brand->brand_name_cn, brand2->brand_name_cn, series->series_name_cn);
			}
		}
	}

	return true;
}

bool ModelParse::parse_manufacturer()
{
	char *name = level->manu;
	if (*name == '\0')
		return false;

	auto it = Manufacturer_map.find(name);
	if (it != Manufacturer_map.end())
	{
		manu = it->second;
		if (check)
		{
			if (manu->import != level->import)
				printf("\t厂商“%s”国产合资进口属性不同：%d %d。\n", manu->manu_name, manu->import, level->import);

			if (strcmp(manu->country, level->country) != 0)
				printf("\t厂商“%s”国别不同：%s %s。\n", manu->manu_name, manu->country, level->country);
		}

		return true;
	}

	manu = reinterpret_cast<Manufacturer*>(Manufacturer_tb->allocate());
	Manufacturer_tb->push_back(reinterpret_cast<char*>(manu));
	strncpy_ex(manu->manu_name, name, 20);
	Manufacturer_map[manu->manu_name] = manu;
	manu->manu_id = static_cast<int32_t>(Manufacturer_tb->size());
	Manufacturer_id_map[manu->manu_id] = manu;
	strncpy_ex(manu->country, level->country, 14);
	manu->import = level->import;
	printf("\t发现新厂商“%s”。\n", manu->manu_name);
	return true;
}

bool ModelParse::parse_engine()
{
	char *code = level->engine;
	if (*code == '\0')
		return false;

	bool add = true;
	char cc[18] = {0};
	auto it = Engine_map.find(code);
	if (it != Engine_map.end())
	{
		engine = it->second;
		add = false;
	}
	else
	{
		int pos = 0;
		const char *data = code;
		while (*data && pos < 18)
		{
			if (*data == '(' || *data == '+' || *data == ';')
				break;

			if (*data != ' ')
			{
				cc[pos] = *data;
				++pos;
			}

			++data;
		}

		if (!pos)
			return false;

		cc[pos] = '\0';

		//if (strcmp(brand->name_cn, "宝马") == 0 )
		//{
		//	pos = strlen(cc);
		//	if (pos == 8)
		//	{
		//		if (isalpha(cc[pos-1]) && isalpha(cc[pos-2]))
		//			cc[pos-2] = '\0';
		//	}
		//	else if (pos == 7)
		//	{
		//		if (isalpha(cc[pos-1]))
		//			cc[pos-1] = '\0';
		//	}
		//}

		it = Engine_map.find(cc);
		if (it != Engine_map.end())
		{
			engine = it->second;
			Engine_map[code] = engine;
			add = false;
		}
	}

	if (add)
	{
		engine = reinterpret_cast<Engine*>(Engine_tb->allocate());
		Engine_tb->push_back(reinterpret_cast<char*>(engine));
		strncpy_ex(engine->engine_code, cc, 14);
		Engine_map[engine->engine_code] = engine;
		Engine_map[code] = engine;
		engine->engine_id = static_cast<int32_t>(Engine_tb->size());
		Engine_id_map[engine->engine_id] = engine;

		engine->cylinder_volume = level->cylinder_volume;
		engine->displacement = level->displacement;
		engine->intake_charge_type = level->intake_charge_type;
		engine->fuel_type = level->fuel_type;
		if (engine->fuel_type > 3)	// 混动
			engine->fuel_type = 1;
		engine->fuel_grade = level->fuel_grade;
		engine->cylinder_align = level->cylinder_align;
		engine->cylinder_num = level->cylinder_num;
		engine->valve_num = level->valve_num;
		engine->fuel_system_type = level->fuel_system_type;
		printf("\t发现新发动机“%s”。\n", engine->engine_code);
	}
	else if (check)
	{
		if (engine->cylinder_volume != level->cylinder_volume)
			printf("\t发动机“%s”->“%s”气缸容积不同：%d %d。\n", level->engine, engine->engine_code, engine->cylinder_volume, level->cylinder_volume);

		if (engine->displacement != level->displacement)
			printf("\t发动机“%s”->“%s”排量不同：%f %f。\n", level->engine, engine->engine_code, engine->displacement, level->displacement);

		if (engine->intake_charge_type != level->intake_charge_type)
			printf("\t发动机“%s”->“%s”进气形式不同：%d %d。\n", level->engine, engine->engine_code, engine->intake_charge_type, level->intake_charge_type);

		if (engine->fuel_type != (level->fuel_type > 3 ? 1 : level->fuel_type))
			printf("\t发动机“%s”->“%s”燃油类型不同：%d %d。\n", level->engine, engine->engine_code, engine->fuel_type, level->fuel_type);

		if (engine->fuel_grade != level->fuel_grade)
			printf("\t发动机“%s”->“%s”燃油标号不同：%d %d。\n", level->engine, engine->engine_code, engine->fuel_grade, level->fuel_grade);

		if (engine->cylinder_align != level->cylinder_align)
			printf("\t发动机“%s”->“%s”气缸排列形式不同：%d %d。\n", level->engine, engine->engine_code, engine->cylinder_align, level->cylinder_align);

		if (engine->cylinder_num != level->cylinder_num)
			printf("\t发动机“%s”->“%s”气缸数不同：%d %d。\n", level->engine, engine->engine_code, engine->cylinder_num, level->cylinder_num);

		if (engine->valve_num != level->valve_num)
			printf("\t发动机“%s”->“%s”每缸气门数不同：%d %d。\n", level->engine, engine->engine_code, engine->valve_num, level->valve_num);

		if (engine->fuel_system_type != level->fuel_system_type)
			printf("\t发动机“%s”->“%s”供油方式不同：%d %d。\n", level->engine, engine->engine_code, engine->fuel_system_type, level->fuel_system_type);
	}

	return true;
}

void ModelParse::get_model_code(const char * /*name*/, char *code)
{
	//sprintf(code, "%d_%s_%s_%s", series->series_id, manu->country, name, level->chassis);
	sprintf(code, "%s", level->code);
}

bool ModelParse::parse_model()
{
	char code[40];
	char name[40];
	const char *pos = strchr(level->model, '[');
	if (pos)
	{
		const char *pos2 = strchr(pos+1, ']');
		if (pos2)
			strncpy_ex(name, pos+1, pos2 - pos > 32 ? 32 : pos2 - pos);
		else
			strncpy_ex(name, pos+1, 32);
	}
	else
		strncpy_ex(name, level->model, 32);

	get_model_code(name, code);

	if (*code == '\0')
		return false;

	auto it = Model_map.find(code);
	if (it != Model_map.end())
	{
		model = it->second;
		if (check)
		{
			if (model->series_id != series->series_id)
				printf("\t车型“%s”车系ID不同：%d %d。\n", model->model_code, model->series_id, series->series_id);

			if (model->manu_id != manu->manu_id)
				printf("\t车型“%s”厂家ID不同：%d %d。\n", model->model_code, model->manu_id, manu->manu_id);

			//if (strcmp(model->model_name, name) != 0)
			//	printf("\t车型“%s”名称不同：%s %s。\n", model->model_code, model->model_name, name);

			if (strcmp(model->chassis, level->chassis) != 0)
				printf("\t车型“%s”底盘不同：%s %s。\n", model->model_code, model->chassis, level->chassis);

			if (model->engine_id != engine->engine_id)
				printf("\t车型“%s”发动机ID不同：%d %d。\n", model->model_code, model->engine_id, engine->engine_id);

			if (model->model_year != level->model_year)
				printf("\t车型“%s”年款不同：%d %d。\n", model->model_code, model->model_year, level->model_year);

			if (model->emiss_stand != level->emiss_stand)
				printf("\t车型“%s”排放标准不同：%d %d。\n", model->model_code, model->emiss_stand, level->emiss_stand);

			if (model->msrp != level->msrp)
				printf("\t车型“%s”指导价格不同：%f %f。\n", model->model_code, model->msrp, level->msrp);

			if (model->year_start != level->year_start)
				printf("\t车型“%s”上市年份不同：%d %d。\n", model->model_code, model->year_start, level->year_start);

			if (model->month_start != level->month_start)
				printf("\t车型“%s”上市月份不同：%d %d。\n", model->model_code, model->month_start, level->month_start);

			if (model->year_make != level->year_make)
				printf("\t车型“%s”生产年份不同：%d %d。\n", model->model_code, model->year_make, level->year_make);

			if (model->year_stop != level->year_stop)
				printf("\t车型“%s”停产年份不同：%d %d。\n", model->model_code, model->year_stop, level->year_stop);

			if (model->product_stat != level->product_stat)
				printf("\t车型“%s”生产状态不同：%d %d。\n", model->model_code, model->product_stat, level->product_stat);

			if (model->sale_stat != level->sale_stat)
				printf("\t车型“%s”销售状态不同：%d %d。\n", model->model_code, model->sale_stat, level->sale_stat);

			if (model->max_horsepower != level->max_horsepower)
				printf("\t车型“%s”最大马力不同：%d %d。\n", model->model_code, model->max_horsepower, level->max_horsepower);

			if (model->max_power != level->max_power)
				printf("\t车型“%s”最大功率不同：%d %d。\n", model->model_code, model->max_power, level->max_power);

			if (model->max_rpm != level->max_rpm)
				printf("\t车型“%s”最大功率转速不同：%d %d。\n", model->model_code, model->max_rpm, level->max_rpm);

			if (model->max_rpm2 != level->max_rpm2)
				printf("\t车型“%s”最大功率转速上限不同：%d %d。\n", model->model_code, model->max_rpm2, level->max_rpm2);

			if (model->max_torque != level->max_torque)
				printf("\t车型“%s”最大扭矩不同：%d %d。\n", model->model_code, model->max_torque, level->max_torque);

			if (model->max_torque_rpm != level->max_torque_rpm)
				printf("\t车型“%s”最大扭矩转速不同：%d %d。\n", model->model_code, model->max_torque_rpm, level->max_torque_rpm);

			if (model->max_torque_rpm2 != level->max_torque_rpm2)
				printf("\t车型“%s”最大扭矩转速上限不同：%d %d。\n", model->model_code, model->max_torque_rpm2, level->max_torque_rpm2);

			if (model->compression_ratio != level->compression_ratio)
				printf("\t车型“%s”压缩比不同：%f %f。\n", model->model_code, model->compression_ratio, level->compression_ratio);

			if (model->fuel_economy_mix != level->fuel_economy_mix)
				printf("\t车型“%s”工信部综合油耗不同：%f %f。\n", model->model_code, model->fuel_economy_mix, level->fuel_economy_mix);

			if (model->fuel_economy_urb != level->fuel_economy_urb)
				printf("\t车型“%s”市区工况油耗不同：%f %f。\n", model->model_code, model->fuel_economy_urb, level->fuel_economy_urb);

			if (model->fuel_economy_suburb != level->fuel_economy_suburb)
				printf("\t车型“%s”市郊工况油耗不同：%f %f。\n", model->model_code, model->fuel_economy_suburb, level->fuel_economy_suburb);

			if (model->acc_time != level->acc_time)
				printf("\t车型“%s”加速时间不同：%f %f。\n", model->model_code, model->acc_time, level->acc_time);

			if (model->max_speed != level->max_speed)
				printf("\t车型“%s”最高车速不同：%d %d。\n", model->model_code, model->max_speed, level->max_speed);

			if (model->tran_desc != level->tran_desc)
				printf("\t车型“%s”变速器描述不同：%d %d。\n", model->model_code, model->tran_desc, level->tran_desc);

			if (model->gears_num != level->gears_num)
				printf("\t车型“%s”档位数不同：%d %d。\n", model->model_code, model->gears_num, level->gears_num);

			if (model->front_brake_type != level->front_brake_type)
				printf("\t车型“%s”前制动器类型不同：%d %d。\n", model->model_code, model->front_brake_type, level->front_brake_type);

			if (model->rear_brake_type != level->rear_brake_type)
				printf("\t车型“%s”后制动器类型不同：%d %d。\n", model->model_code, model->rear_brake_type, level->rear_brake_type);

			if (model->front_suspension_type != level->front_suspension_type)
				printf("\t车型“%s”前悬挂类型不同：%d %d。\n", model->model_code, model->front_suspension_type, level->front_suspension_type);

			if (model->rear_suspension_type != level->rear_suspension_type)
				printf("\t车型“%s”后悬挂类型不同：%d %d。\n", model->model_code, model->rear_suspension_type, level->rear_suspension_type);

			if (model->steering_type != level->steering_type)
				printf("\t车型“%s”转向机形式不同：%d %d。\n", model->model_code, model->steering_type, level->steering_type);

			if (model->steering_aid_type != level->steering_aid_type)
				printf("\t车型“%s”助力类型不同：%d %d。\n", model->model_code, model->steering_aid_type, level->steering_aid_type);

			if (model->min_grd_clearance != level->min_grd_clearance)
				printf("\t车型“%s”最小离地间隙不同：%d %d。\n", model->model_code, model->min_grd_clearance, level->min_grd_clearance);

			if (model->min_turn_radius != level->min_turn_radius)
				printf("\t车型“%s”最小转弯半径不同：%f %f。\n", model->model_code, model->min_turn_radius, level->min_turn_radius);

			if (model->engine_position != level->engine_position)
				printf("\t车型“%s”发动机位置不同：%d %d。\n", model->model_code, model->engine_position, level->engine_position);

			if (model->drive_mode != level->drive_mode)
				printf("\t车型“%s”驱动方式不同：%d %d。\n", model->model_code, model->drive_mode, level->drive_mode);

			if (model->body != level->body)
				printf("\t车型“%s”车身型式不同：%d %d。\n", model->model_code, model->body, level->body);

			if (model->length != level->length)
				printf("\t车型“%s”长不同：%d %d。\n", model->model_code, model->length, level->length);

			if (model->width != level->width)
				printf("\t车型“%s”宽不同：%d %d。\n", model->model_code, model->width, level->width);

			if (model->height != level->height)
				printf("\t车型“%s”高不同：%d %d。\n", model->model_code, model->height, level->height);

			if (model->wheelbase != level->wheelbase)
				printf("\t车型“%s”轴距不同：%d %d。\n", model->model_code, model->wheelbase, level->wheelbase);

			if (model->front_track != level->front_track)
				printf("\t车型“%s”前轮距不同：%d %d。\n", model->model_code, model->front_track, level->front_track);

			if (model->rear_track != level->rear_track)
				printf("\t车型“%s”后轮距不同：%d %d。\n", model->model_code, model->rear_track, level->rear_track);

			if (model->curb_weight != level->curb_weight)
				printf("\t车型“%s”整备质量不同：%d %d。\n", model->model_code, model->curb_weight, level->curb_weight);

			if (model->gross_weight != level->gross_weight)
				printf("\t车型“%s”最大载重质量不同：%d %d。\n", model->model_code, model->gross_weight, level->gross_weight);

			if (model->fuel_tank_cap != level->fuel_tank_cap)
				printf("\t车型“%s”油箱容积不同：%d %d。\n", model->model_code, model->fuel_tank_cap, level->fuel_tank_cap);

			if (model->cargo_cap != level->cargo_cap)
				printf("\t车型“%s”行李厢容积不同：%d %d。\n", model->model_code, model->cargo_cap, level->cargo_cap);

			if (model->cargo_cap2 != level->cargo_cap2)
				printf("\t车型“%s”行李厢容积上限不同：%d %d。\n", model->model_code, model->cargo_cap2, level->cargo_cap2);

			if (model->door_num != level->door_num)
				printf("\t车型“%s”车门数不同：%d %d。\n", model->model_code, model->door_num, level->door_num);

			if (model->seat_num != level->seat_num)
				printf("\t车型“%s”座位数不同：%d %d。\n", model->model_code, model->seat_num, level->seat_num);

			if (model->num_speaker != level->num_speaker)
				printf("\t车型“%s”扬声器数量不同：%d %d。\n", model->model_code, model->num_speaker, level->num_speaker);

			if (model->front_tire_spec != level->front_tire_spec)
				printf("\t车型“%s”前轮胎规格不同：%d %d。\n", model->model_code, model->front_tire_spec, level->front_tire_spec);

			if (model->rear_tire_spec != level->rear_tire_spec)
				printf("\t车型“%s”后轮胎规格不同：%d %d。\n", model->model_code, model->rear_tire_spec, level->rear_tire_spec);

			if (model->front_rim_spec != level->front_rim_spec)
				printf("\t车型“%s”前轮毂规格不同：%d %d。\n", model->model_code, model->front_rim_spec, level->front_rim_spec);

			if (model->rear_rim_spec != level->rear_rim_spec)
				printf("\t车型“%s”后轮毂规格不同：%d %d。\n", model->model_code, model->rear_rim_spec, level->rear_rim_spec);

			if (model->rim_mat != level->rim_mat)
				printf("\t车型“%s”轮毂材料不同：%d %d。\n", model->model_code, model->rim_mat, level->rim_mat);

			if (model->spare_tire_spec != level->spare_tire_spec)
				printf("\t车型“%s”备胎规格不同：%d %d。\n", model->model_code, model->spare_tire_spec, level->spare_tire_spec);

			if (model->vehicle_type != level->vehicle_type)
				printf("\t车型“%s”车辆类型不同：%d %d。\n", model->model_code, model->vehicle_type, level->vehicle_type);

			if (model->vehicle_level != level->vehicle_level)
				printf("\t车型“%s”车辆级别不同：%d %d。\n", model->model_code, model->vehicle_level, level->vehicle_level);
		}

		return true;
	}

	model = reinterpret_cast<Model*>(Model_tb->allocate());
	Model_tb->push_back(reinterpret_cast<char*>(model));
	strncpy_ex(model->model_code, code, 20);
	Model_map[model->model_code] = model;
	model->model_id = static_cast<int32_t>(Model_tb->size());
	Model_id_map[model->model_id] = model;
	model->series_id = series->series_id;
	model->manu_id = manu->manu_id;
	model->engine_id = engine->engine_id;
	if (*(level->variant) == '\0')
		strncpy_ex(model->model_name, name, 32);
	else
	{
		sprintf(code, "%s %s", name, level->variant);
		strncpy_ex(model->model_name, code, 32);
	}

	strncpy_ex(model->chassis, level->chassis, 16);

	model->msrp = level->msrp;
	model->compression_ratio = level->compression_ratio;
	model->fuel_economy_mix = level->fuel_economy_mix;
	model->fuel_economy_urb = level->fuel_economy_urb;
	model->fuel_economy_suburb = level->fuel_economy_suburb;
	model->acc_time = level->acc_time;
	model->min_turn_radius = level->min_turn_radius;
	model->model_year = level->model_year;
	model->max_horsepower = level->max_horsepower;
	model->max_power = level->max_power;
	model->max_rpm = level->max_rpm;
	model->max_rpm2 = level->max_rpm2;
	model->max_torque = level->max_torque;
	model->max_torque_rpm = level->max_torque_rpm;
	model->max_torque_rpm2 = level->max_torque_rpm2;
	model->max_speed = level->max_speed;
	model->min_grd_clearance = level->min_grd_clearance;
	model->length = level->length;
	model->width = level->width;
	model->height = level->height;
	model->wheelbase = level->wheelbase;
	model->front_track = level->front_track;
	model->rear_track = level->rear_track;
	model->curb_weight = level->curb_weight;
	model->gross_weight = level->gross_weight;
	model->fuel_tank_cap = level->fuel_tank_cap;
	model->cargo_cap = level->cargo_cap;
	model->cargo_cap2 = level->cargo_cap2;
	model->year_make = level->year_make;
	model->year_stop = level->year_stop;
	model->year_start = level->year_start;
	model->month_start = level->month_start;
	model->emiss_stand = level->emiss_stand;
	model->vehicle_type = level->vehicle_type;
	model->vehicle_level = level->vehicle_level;
	model->product_stat = level->product_stat;
	model->sale_stat = level->sale_stat;
	model->tran_desc = level->tran_desc;
	model->gears_num = level->gears_num;
	model->front_brake_type = level->front_brake_type;
	model->rear_brake_type = level->rear_brake_type;
	model->front_suspension_type = level->front_suspension_type;
	model->rear_suspension_type = level->rear_suspension_type;
	model->steering_type = level->steering_type;
	model->steering_aid_type = level->steering_aid_type;
	model->engine_position = level->engine_position;
	model->drive_mode = level->drive_mode;
	model->body = level->body;
	model->door_num = level->door_num;
	model->seat_num = level->seat_num;
	model->num_speaker = level->num_speaker;
	model->front_tire_spec = level->front_tire_spec;
	model->rear_tire_spec = level->rear_tire_spec;
	model->front_rim_spec = level->front_rim_spec;
	model->rear_rim_spec = level->rear_rim_spec;
	model->rim_mat = level->rim_mat;
	model->spare_tire_spec = level->spare_tire_spec;
	printf("\t发现新车型%d“%s”。\n", model->model_id, model->model_code);

	return true;
}

//bool ModelParse::parse_variant(const char *code)
//{
//	if (*code == '\0')
//		return false;
//
//	auto it = Variant_map.find(code);
//	if (it != Variant_map.end())
//	{
//		model = it->second;
//
//		if (check)
//		{
//			if (model->model_id !=  model->model_id)
//				printf("\t版本“%s”车型ID不同：%d %d。\n", model->code, model->model_id,  model->model_id);
//
//			if (model->engine_id != engine->engine_id)
//				printf("\t车型“%s”发动机ID不同：%d %d。\n", model->code, model->engine_id, engine->engine_id);
//
//			if (model->model_year != level->model_year)
//				printf("\t版本“%s”年款不同：%d %d。\n", model->code, model->model_year, level->model_year);
//
//			if (model->emiss_stand != level->emiss_stand)
//				printf("\t版本“%s”排放标准不同：%d %d。\n", model->code, model->emiss_stand, level->emiss_stand);
//
//			if (strcmp(model->name, level->model) != 0)
//				printf("\t版本“%s”名称不同：%s %s。\n", model->code, model->name, level->model);
//
//			if (model->msrp != level->msrp)
//				printf("\t版本“%s”指导价格不同：%f %f。\n", model->code, model->msrp, level->msrp);
//
//			if (model->year_start != level->year_start)
//				printf("\t版本“%s”上市年份不同：%d %d。\n", model->code, model->year_start, level->year_start);
//
//			if (model->month_start != level->month_start)
//				printf("\t版本“%s”上市月份不同：%d %d。\n", model->code, model->month_start, level->month_start);
//
//			if (model->year_make != level->year_make)
//				printf("\t版本“%s”生产年份不同：%d %d。\n", model->code, model->year_make, level->year_make);
//
//			if (model->year_stop != level->year_stop)
//				printf("\t版本“%s”停产年份不同：%d %d。\n", model->code, model->year_stop, level->year_stop);
//
//			if (model->product_stat != level->product_stat)
//				printf("\t版本“%s”生产状态不同：%d %d。\n", model->code, model->product_stat, level->product_stat);
//
//			if (model->sale_stat != level->sale_stat)
//				printf("\t版本“%s”销售状态不同：%d %d。\n", model->code, model->sale_stat, level->sale_stat);
//
//			if (model->intake_charge_type != level->intake_charge_type)
//				printf("\t版本“%s”进气形式不同：%d %d。\n", model->code, model->intake_charge_type, level->intake_charge_type);
//
//			if (model->max_horsepower != level->max_horsepower)
//				printf("\t版本“%s”最大马力不同：%d %d。\n", model->code, model->max_horsepower, level->max_horsepower);
//
//			if (model->max_power != level->max_power)
//				printf("\t版本“%s”最大功率不同：%d %d。\n", model->code, model->max_power, level->max_power);
//
//			if (model->max_rpm != level->max_rpm)
//				printf("\t版本“%s”最大功率转速不同：%d %d。\n", model->code, model->max_rpm, level->max_rpm);
//
//			if (model->max_rpm2 != level->max_rpm2)
//				printf("\t版本“%s”最大功率转速上限不同：%d %d。\n", model->code, model->max_rpm2, level->max_rpm2);
//
//			if (model->max_torque != level->max_torque)
//				printf("\t版本“%s”最大扭矩不同：%d %d。\n", model->code, model->max_torque, level->max_torque);
//
//			if (model->max_torque_rpm != level->max_torque_rpm)
//				printf("\t版本“%s”最大扭矩转速不同：%d %d。\n", model->code, model->max_torque_rpm, level->max_torque_rpm);
//
//			if (model->max_torque_rpm2 != level->max_torque_rpm2)
//				printf("\t版本“%s”最大扭矩转速上限不同：%d %d。\n", model->code, model->max_torque_rpm2, level->max_torque_rpm2);
//
//			if (model->compression_ratio != level->compression_ratio)
//				printf("\t版本“%s”压缩比不同：%f %f。\n", model->code, model->compression_ratio, level->compression_ratio);
//
//			if (model->fuel_economy_mix != level->fuel_economy_mix)
//				printf("\t版本“%s”工信部综合油耗不同：%f %f。\n", model->code, model->fuel_economy_mix, level->fuel_economy_mix);
//
//			if (model->fuel_economy_urb != level->fuel_economy_urb)
//				printf("\t版本“%s”市区工况油耗不同：%f %f。\n", model->code, model->fuel_economy_urb, level->fuel_economy_urb);
//
//			if (model->fuel_economy_suburb != level->fuel_economy_suburb)
//				printf("\t版本“%s”市郊工况油耗不同：%f %f。\n", model->code, model->fuel_economy_suburb, level->fuel_economy_suburb);
//
//			if (model->acc_time != level->acc_time)
//				printf("\t版本“%s”加速时间不同：%f %f。\n", model->code, model->acc_time, level->acc_time);
//
//			if (model->max_speed != level->max_speed)
//				printf("\t版本“%s”最高车速不同：%d %d。\n", model->code, model->max_speed, level->max_speed);
//
//			if (model->tran_desc != level->tran_desc)
//				printf("\t版本“%s”变速器描述不同：%d %d。\n", model->code, model->tran_desc, level->tran_desc);
//
//			if (model->gears_num != level->gears_num)
//				printf("\t版本“%s”档位数不同：%d %d。\n", model->code, model->gears_num, level->gears_num);
//
//			if (model->front_brake_type != level->front_brake_type)
//				printf("\t版本“%s”前制动器类型不同：%d %d。\n", model->code, model->front_brake_type, level->front_brake_type);
//
//			if (model->rear_brake_type != level->rear_brake_type)
//				printf("\t版本“%s”后制动器类型不同：%d %d。\n", model->code, model->rear_brake_type, level->rear_brake_type);
//
//			if (model->front_suspension_type != level->front_suspension_type)
//				printf("\t版本“%s”前悬挂类型不同：%d %d。\n", model->code, model->front_suspension_type, level->front_suspension_type);
//
//			if (model->rear_suspension_type != level->rear_suspension_type)
//				printf("\t版本“%s”后悬挂类型不同：%d %d。\n", model->code, model->rear_suspension_type, level->rear_suspension_type);
//
//			if (model->steering_type != level->steering_type)
//				printf("\t版本“%s”转向机形式不同：%d %d。\n", model->code, model->steering_type, level->steering_type);
//
//			if (model->steering_aid_type != level->steering_aid_type)
//				printf("\t版本“%s”助力类型不同：%d %d。\n", model->code, model->steering_aid_type, level->steering_aid_type);
//
//			if (model->min_grd_clearance != level->min_grd_clearance)
//				printf("\t版本“%s”最小离地间隙不同：%d %d。\n", model->code, model->min_grd_clearance, level->min_grd_clearance);
//
//			if (model->min_turn_radius != level->min_turn_radius)
//				printf("\t版本“%s”最小转弯半径不同：%f %f。\n", model->code, model->min_turn_radius, level->min_turn_radius);
//
//			if (model->engine_position != level->engine_position)
//				printf("\t版本“%s”发动机位置不同：%d %d。\n", model->code, model->engine_position, level->engine_position);
//
//			if (model->drive_mode != level->drive_mode)
//				printf("\t版本“%s”驱动方式不同：%d %d。\n", model->code, model->drive_mode, level->drive_mode);
//
//			if (model->body != level->body)
//				printf("\t版本“%s”车身型式不同：%d %d。\n", model->code, model->body, level->body);
//
//			if (model->length != level->length)
//				printf("\t版本“%s”长不同：%d %d。\n", model->code, model->length, level->length);
//
//			if (model->width != level->width)
//				printf("\t版本“%s”宽不同：%d %d。\n", model->code, model->width, level->width);
//
//			if (model->height != level->height)
//				printf("\t版本“%s”高不同：%d %d。\n", model->code, model->height, level->height);
//
//			if (model->wheelbase != level->wheelbase)
//				printf("\t版本“%s”轴距不同：%d %d。\n", model->code, model->wheelbase, level->wheelbase);
//
//			if (model->front_track != level->front_track)
//				printf("\t版本“%s”前轮距不同：%d %d。\n", model->code, model->front_track, level->front_track);
//
//			if (model->rear_track != level->rear_track)
//				printf("\t版本“%s”后轮距不同：%d %d。\n", model->code, model->rear_track, level->rear_track);
//
//			if (model->curb_weight != level->curb_weight)
//				printf("\t版本“%s”整备质量不同：%d %d。\n", model->code, model->curb_weight, level->curb_weight);
//
//			if (model->gross_weight != level->gross_weight)
//				printf("\t版本“%s”最大载重质量不同：%d %d。\n", model->code, model->gross_weight, level->gross_weight);
//
//			if (model->fuel_tank_cap != level->fuel_tank_cap)
//				printf("\t版本“%s”油箱容积不同：%d %d。\n", model->code, model->fuel_tank_cap, level->fuel_tank_cap);
//
//			if (model->cargo_cap != level->cargo_cap)
//				printf("\t版本“%s”行李厢容积不同：%d %d。\n", model->code, model->cargo_cap, level->cargo_cap);
//
//			if (model->cargo_cap2 != level->cargo_cap2)
//				printf("\t版本“%s”行李厢容积上限不同：%d %d。\n", model->code, model->cargo_cap2, level->cargo_cap2);
//
//			if (model->door_num != level->door_num)
//				printf("\t版本“%s”车门数不同：%d %d。\n", model->code, model->door_num, level->door_num);
//
//			if (model->seat_num != level->seat_num)
//				printf("\t版本“%s”座位数不同：%d %d。\n", model->code, model->seat_num, level->seat_num);
//
//			if (model->num_speaker != level->num_speaker)
//				printf("\t版本“%s”扬声器数量不同：%d %d。\n", model->code, model->num_speaker, level->num_speaker);
//
//			if (model->front_tire_spec != level->front_tire_spec)
//				printf("\t版本“%s”前轮胎规格不同：%d %d。\n", model->code, model->front_tire_spec, level->front_tire_spec);
//
//			if (model->rear_tire_spec != level->rear_tire_spec)
//				printf("\t版本“%s”后轮胎规格不同：%d %d。\n", model->code, model->rear_tire_spec, level->rear_tire_spec);
//
//			if (model->front_rim_spec != level->front_rim_spec)
//				printf("\t版本“%s”前轮毂规格不同：%d %d。\n", model->code, model->front_rim_spec, level->front_rim_spec);
//
//			if (model->rear_rim_spec != level->rear_rim_spec)
//				printf("\t版本“%s”后轮毂规格不同：%d %d。\n", model->code, model->rear_rim_spec, level->rear_rim_spec);
//
//			if (model->rim_mat != level->rim_mat)
//				printf("\t版本“%s”轮毂材料不同：%d %d。\n", model->code, model->rim_mat, level->rim_mat);
//
//			if (model->spare_tire_spec != level->spare_tire_spec)
//				printf("\t版本“%s”备胎规格不同：%d %d。\n", model->code, model->spare_tire_spec, level->spare_tire_spec);
//
//			if (model->drivers_airbag != level->drivers_airbag)
//				printf("\t版本“%s”驾驶座安全气囊不同：%d %d。\n", model->code, model->drivers_airbag, level->drivers_airbag);
//
//			if (model->passenger_airbag != level->passenger_airbag)
//				printf("\t版本“%s”副驾驶安全气囊不同：%d %d。\n", model->code, model->passenger_airbag, level->passenger_airbag);
//
//			if (model->front_side_airbag != level->front_side_airbag)
//				printf("\t版本“%s”前排侧气囊不同：%d %d。\n", model->code, model->front_side_airbag, level->front_side_airbag);
//
//			if (model->rear_side_airbag != level->rear_side_airbag)
//				printf("\t版本“%s”后排侧气囊不同：%d %d。\n", model->code, model->rear_side_airbag, level->rear_side_airbag);
//
//			if (model->front_head_airbag != level->front_head_airbag)
//				printf("\t版本“%s”前排头部气囊不同：%d %d。\n", model->code, model->front_head_airbag, level->front_head_airbag);
//
//			if (model->rear_head_airbag != level->rear_head_airbag)
//				printf("\t版本“%s”后排头部气囊不同：%d %d。\n", model->code, model->rear_head_airbag, level->rear_head_airbag);
//
//			if (model->knee_airbag != level->knee_airbag)
//				printf("\t版本“%s”膝部气囊不同：%d %d。\n", model->code, model->knee_airbag, level->knee_airbag);
//
//			if (model->tpms != level->tpms)
//				printf("\t版本“%s”胎压监测装置不同：%d %d。\n", model->code, model->tpms, level->tpms);
//
//			if (model->rsc != level->rsc)
//				printf("\t版本“%s”零胎压继续行驶不同：%d %d。\n", model->code, model->rsc, level->rsc);
//
//			if (model->sbr != level->sbr)
//				printf("\t版本“%s”安全带未系提示不同：%d %d。\n", model->code, model->sbr, level->sbr);
//
//			if (model->isofix != level->isofix)
//				printf("\t版本“%s”isofix儿童座椅接口不同：%d %d。\n", model->code, model->isofix, level->isofix);
//
//			if (model->latch != level->latch)
//				printf("\t版本“%s”latch座椅接口不同：%d %d。\n", model->code, model->latch, level->latch);
//
//			if (model->immobilizer != level->immobilizer)
//				printf("\t版本“%s”发动机电子防盗不同：%d %d。\n", model->code, model->immobilizer, level->immobilizer);
//
//			if (model->central_lock != level->central_lock)
//				printf("\t版本“%s”中控锁不同：%d %d。\n", model->code, model->central_lock, level->central_lock);
//
//			if (model->rke != level->rke)
//				printf("\t版本“%s”遥控钥匙不同：%d %d。\n", model->code, model->rke, level->rke);
//
//			if (model->peps != level->peps)
//				printf("\t版本“%s”无钥匙启动系统不同：%d %d。\n", model->code, model->peps, level->peps);
//
//			if (model->abs != level->abs)
//				printf("\t版本“%s”abs防抱死不同：%d %d。\n", model->code, model->abs, level->abs);
//
//			if (model->ebd != level->ebd)
//				printf("\t版本“%s”制动力分配不同：%d %d。\n", model->code, model->ebd, level->ebd);
//
//			if (model->eba != level->eba)
//				printf("\t版本“%s”刹车辅助不同：%d %d。\n", model->code, model->eba, level->eba);
//
//			if (model->tcs != level->tcs)
//				printf("\t版本“%s”牵引力控制不同：%d %d。\n", model->code, model->tcs, level->tcs);
//
//			if (model->esp != level->esp)
//				printf("\t版本“%s”车身稳定控制不同：%d %d。\n", model->code, model->esp, level->esp);
//
//			if (model->epb != level->epb)
//				printf("\t版本“%s”自动驻车不同：%d %d。\n", model->code, model->epb, level->epb);
//
//			if (model->hdc != level->hdc)
//				printf("\t版本“%s”陡坡缓降不同：%d %d。\n", model->code, model->hdc, level->hdc);
//
//			if (model->avs != level->avs)
//				printf("\t版本“%s”可变悬挂不同：%d %d。\n", model->code, model->avs, level->avs);
//
//			if (model->air_suspension != level->air_suspension)
//				printf("\t版本“%s”空气悬挂不同：%d %d。\n", model->code, model->air_suspension, level->air_suspension);
//
//			if (model->variable_ratio_steering != level->variable_ratio_steering)
//				printf("\t版本“%s”可变转向比不同：%d %d。\n", model->code, model->variable_ratio_steering, level->variable_ratio_steering);
//
//			if (model->blis != level->blis)
//				printf("\t版本“%s”并线辅助不同：%d %d。\n", model->code, model->blis, level->blis);
//
//			if (model->autobrake != level->autobrake)
//				printf("\t版本“%s”主动刹车不同：%d %d。\n", model->code, model->autobrake, level->autobrake);
//
//			if (model->afs != level->afs)
//				printf("\t版本“%s”主动转向系统不同：%d %d。\n", model->code, model->afs, level->afs);
//
//			if (model->leather_steering_wheel != level->leather_steering_wheel)
//				printf("\t版本“%s”真皮方向盘不同：%d %d。\n", model->code, model->leather_steering_wheel, level->leather_steering_wheel);
//
//			if (model->steering_wheel_adjust != level->steering_wheel_adjust)
//				printf("\t版本“%s”方向盘上下调节不同：%d %d。\n", model->code, model->steering_wheel_adjust, level->steering_wheel_adjust);
//
//			if (model->steering_wheel_adjust2 != level->steering_wheel_adjust2)
//				printf("\t版本“%s”方向盘前后调节不同：%d %d。\n", model->code, model->steering_wheel_adjust2, level->steering_wheel_adjust2);
//
//			if (model->steering_wheel_electric_regulation != level->steering_wheel_electric_regulation)
//				printf("\t版本“%s”方向盘电动调节不同：%d %d。\n", model->code, model->steering_wheel_electric_regulation, level->steering_wheel_electric_regulation);
//
//			if (model->mfw != level->mfw)
//				printf("\t版本“%s”多功能方向盘不同：%d %d。\n", model->code, model->mfw, level->mfw);
//
//			if (model->steering_wheel_gearshift != level->steering_wheel_gearshift)
//				printf("\t版本“%s”方向盘换挡不同：%d %d。\n", model->code, model->steering_wheel_gearshift, level->steering_wheel_gearshift);
//
//			if (model->leather_seats != level->leather_seats)
//				printf("\t版本“%s”真皮座椅不同：%d %d。\n", model->code, model->leather_seats, level->leather_seats);
//
//			if (model->sports_seats != level->sports_seats)
//				printf("\t版本“%s”运动座椅不同：%d %d。\n", model->code, model->sports_seats, level->sports_seats);
//
//			if (model->seat_height_adjust != level->seat_height_adjust)
//				printf("\t版本“%s”座椅高低调节不同：%d %d。\n", model->code, model->seat_height_adjust, level->seat_height_adjust);
//
//			if (model->lumbar_support_adjust != level->lumbar_support_adjust)
//				printf("\t版本“%s”腰部支撑调节不同：%d %d。\n", model->code, model->lumbar_support_adjust, level->lumbar_support_adjust);
//
//			if (model->shoulder_support_adjust != level->shoulder_support_adjust)
//				printf("\t版本“%s”肩部支撑调节不同：%d %d。\n", model->code, model->shoulder_support_adjust, level->shoulder_support_adjust);
//
//			if (model->drivers_esa != level->drivers_esa)
//				printf("\t版本“%s”驾驶座座椅电动调节不同：%d %d。\n", model->code, model->drivers_esa, level->drivers_esa);
//
//			if (model->passenger_esa != level->passenger_esa)
//				printf("\t版本“%s”副驾驶座座椅电动调节不同：%d %d。\n", model->code, model->passenger_esa, level->passenger_esa);
//
//			if (model->second_row_backrest_adjust != level->second_row_backrest_adjust)
//				printf("\t版本“%s”第二排靠背角度调节不同：%d %d。\n", model->code, model->second_row_backrest_adjust, level->second_row_backrest_adjust);
//
//			if (model->second_row_move != level->second_row_move)
//				printf("\t版本“%s”第二排座椅移动不同：%d %d。\n", model->code, model->second_row_move, level->second_row_move);
//
//			if (model->rear_esa != level->rear_esa)
//				printf("\t版本“%s”后排座椅电动调节不同：%d %d。\n", model->code, model->rear_esa, level->rear_esa);
//
//			if (model->seat_position_memory != level->seat_position_memory)
//				printf("\t版本“%s”电动座椅记忆不同：%d %d。\n", model->code, model->seat_position_memory, level->seat_position_memory);
//
//			if (model->front_heated_seat != level->front_heated_seat)
//				printf("\t版本“%s”前排座椅加热不同：%d %d。\n", model->code, model->front_heated_seat, level->front_heated_seat);
//
//			if (model->rear_heated_seat != level->rear_heated_seat)
//				printf("\t版本“%s”后排座椅加热不同：%d %d。\n", model->code, model->rear_heated_seat, level->rear_heated_seat);
//
//			if (model->seat_ventilate != level->seat_ventilate)
//				printf("\t版本“%s”座椅通风不同：%d %d。\n", model->code, model->seat_ventilate, level->seat_ventilate);
//
//			if (model->seat_massage != level->seat_massage)
//				printf("\t版本“%s”座椅按摩不同：%d %d。\n", model->code, model->seat_massage, level->seat_massage);
//
//			if (model->rear_seat_recline != level->rear_seat_recline)
//				printf("\t版本“%s”后排座椅整体放倒不同：%d %d。\n", model->code, model->rear_seat_recline, level->rear_seat_recline);
//
//			if (model->rear_seat_recline_part != level->rear_seat_recline_part)
//				printf("\t版本“%s”后排座椅比例放倒不同：%d %d。\n", model->code, model->rear_seat_recline_part, level->rear_seat_recline_part);
//
//			if (model->third_seat_row != level->third_seat_row)
//				printf("\t版本“%s”第三排座椅不同：%d %d。\n", model->code, model->third_seat_row, level->third_seat_row);
//
//			if (model->front_central_rest != level->front_central_rest)
//				printf("\t版本“%s”前座中央扶手不同：%d %d。\n", model->code, model->front_central_rest, level->front_central_rest);
//
//			if (model->rear_central_rest != level->rear_central_rest)
//				printf("\t版本“%s”后座中央扶手不同：%d %d。\n", model->code, model->rear_central_rest, level->rear_central_rest);
//
//			if (model->rear_cup_shelf != level->rear_cup_shelf)
//				printf("\t版本“%s”后排杯架不同：%d %d。\n", model->code, model->rear_cup_shelf, level->rear_cup_shelf);
//
//			if (model->ambient_lighting != level->ambient_lighting)
//				printf("\t版本“%s”车内氛围灯不同：%d %d。\n", model->code, model->ambient_lighting, level->ambient_lighting);
//
//			if (model->rear_window_sunshade != level->rear_window_sunshade)
//				printf("\t版本“%s”后风挡遮阳帘不同：%d %d。\n", model->code, model->rear_window_sunshade, level->rear_window_sunshade);
//
//			if (model->rear_side_sunshade != level->rear_side_sunshade)
//				printf("\t版本“%s”后排侧遮阳帘不同：%d %d。\n", model->code, model->rear_side_sunshade, level->rear_side_sunshade);
//
//			if (model->sun_visor != level->sun_visor)
//				printf("\t版本“%s”遮阳板化妆镜不同：%d %d。\n", model->code, model->sun_visor, level->sun_visor);
//
//			if (model->electric_trunk != level->electric_trunk)
//				printf("\t版本“%s”电动后备箱不同：%d %d。\n", model->code, model->electric_trunk, level->electric_trunk);
//
//			if (model->sport_appearance_package != level->sport_appearance_package)
//				printf("\t版本“%s”运动外观套件不同：%d %d。\n", model->code, model->sport_appearance_package, level->sport_appearance_package);
//
//			if (model->electric_suction_door != level->electric_suction_door)
//				printf("\t版本“%s”电动吸合门不同：%d %d。\n", model->code, model->electric_suction_door, level->electric_suction_door);
//
//			if (model->electric_sunroof != level->electric_sunroof)
//				printf("\t版本“%s”电动天窗不同：%d %d。\n", model->code, model->electric_sunroof, level->electric_sunroof);
//
//			if (model->panoramic_sunroof != level->panoramic_sunroof)
//				printf("\t版本“%s”全景天窗不同：%d %d。\n", model->code, model->panoramic_sunroof, level->panoramic_sunroof);
//
//			if (model->hid != level->hid)
//				printf("\t版本“%s”氙气大灯不同：%d %d。\n", model->code, model->hid, level->hid);
//
//			if (model->led != level->led)
//				printf("\t版本“%s”led大灯不同：%d %d。\n", model->code, model->led, level->led);
//
//			if (model->drl != level->drl)
//				printf("\t版本“%s”日间行车灯不同：%d %d。\n", model->code, model->drl, level->drl);
//
//			if (model->automatic_headlights != level->automatic_headlights)
//				printf("\t版本“%s”自动头灯不同：%d %d。\n", model->code, model->automatic_headlights, level->automatic_headlights);
//
//			if (model->acl != level->acl)
//				printf("\t版本“%s”转向头灯不同：%d %d。\n", model->code, model->acl, level->acl);
//
//			if (model->front_fog_lamp != level->front_fog_lamp)
//				printf("\t版本“%s”前雾灯不同：%d %d。\n", model->code, model->front_fog_lamp, level->front_fog_lamp);
//
//			if (model->headlight_height_adjust != level->headlight_height_adjust)
//				printf("\t版本“%s”大灯高度可调不同：%d %d。\n", model->code, model->headlight_height_adjust, level->headlight_height_adjust);
//
//			if (model->sra != level->sra)
//				printf("\t版本“%s”大灯清洗装置不同：%d %d。\n", model->code, model->sra, level->sra);
//
//			if (model->front_power_window != level->front_power_window)
//				printf("\t版本“%s”前电动车窗不同：%d %d。\n", model->code, model->front_power_window, level->front_power_window);
//
//			if (model->rear_power_window != level->rear_power_window)
//				printf("\t版本“%s”后电动车窗不同：%d %d。\n", model->code, model->rear_power_window, level->rear_power_window);
//
//			if (model->anti_pinch_window != level->anti_pinch_window)
//				printf("\t版本“%s”车窗防夹手功能不同：%d %d。\n", model->code, model->anti_pinch_window, level->anti_pinch_window);
//
//			if (model->insulating_glass != level->insulating_glass)
//				printf("\t版本“%s”隔热玻璃不同：%d %d。\n", model->code, model->insulating_glass, level->insulating_glass);
//
//			if (model->electric_rearview_mirror != level->electric_rearview_mirror)
//				printf("\t版本“%s”后视镜电动调节不同：%d %d。\n", model->code, model->electric_rearview_mirror, level->electric_rearview_mirror);
//
//			if (model->heated_rearview_mirror != level->heated_rearview_mirror)
//				printf("\t版本“%s”后视镜加热不同：%d %d。\n", model->code, model->heated_rearview_mirror, level->heated_rearview_mirror);
//
//			if (model->auto_dimming_rearview_mirror != level->auto_dimming_rearview_mirror)
//				printf("\t版本“%s”后视镜自动防眩目不同：%d %d。\n", model->code, model->auto_dimming_rearview_mirror, level->auto_dimming_rearview_mirror);
//
//			if (model->electric_fold_rearview_mirror != level->electric_fold_rearview_mirror)
//				printf("\t版本“%s”后视镜电动折叠不同：%d %d。\n", model->code, model->electric_fold_rearview_mirror, level->electric_fold_rearview_mirror);
//
//			if (model->position_memory_rearview_mirror != level->position_memory_rearview_mirror)
//				printf("\t版本“%s”后视镜记忆不同：%d %d。\n", model->code, model->position_memory_rearview_mirror, level->position_memory_rearview_mirror);
//
//			if (model->rear_wiper != level->rear_wiper)
//				printf("\t版本“%s”后雨刷不同：%d %d。\n", model->code, model->rear_wiper, level->rear_wiper);
//
//			if (model->sensing_wiper != level->sensing_wiper)
//				printf("\t版本“%s”感应雨刷不同：%d %d。\n", model->code, model->sensing_wiper, level->sensing_wiper);
//
//			if (model->ccs != level->ccs)
//				printf("\t版本“%s”定速巡航不同：%d %d。\n", model->code, model->ccs, level->ccs);
//
//			if (model->park_assist != level->park_assist)
//				printf("\t版本“%s”泊车辅助不同：%d %d。\n", model->code, model->park_assist, level->park_assist);
//
//			if (model->reverse_video_image != level->reverse_video_image)
//				printf("\t版本“%s”倒车视频影像不同：%d %d。\n", model->code, model->reverse_video_image, level->reverse_video_image);
//
//			if (model->ecu_screen != level->ecu_screen)
//				printf("\t版本“%s”行车电脑显示屏不同：%d %d。\n", model->code, model->ecu_screen, level->ecu_screen);
//
//			if (model->hud != level->hud)
//				printf("\t版本“%s”hud抬头数字显示不同：%d %d。\n", model->code, model->hud, level->hud);
//
//			if (model->gps != level->gps)
//				printf("\t版本“%s”gps导航不同：%d %d。\n", model->code, model->gps, level->gps);
//
//			if (model->gps_is != level->gps_is)
//				printf("\t版本“%s”定位互动服务不同：%d %d。\n", model->code, model->gps_is, level->gps_is);
//
//			if (model->central_control_screen != level->central_control_screen)
//				printf("\t版本“%s”中控台彩色大屏不同：%d %d。\n", model->code, model->central_control_screen, level->central_control_screen);
//
//			if (model->hmi != level->hmi)
//				printf("\t版本“%s”人机交互系统不同：%d %d。\n", model->code, model->hmi, level->hmi);
//
//			if (model->built_in_disk != level->built_in_disk)
//				printf("\t版本“%s”内置硬盘不同：%d %d。\n", model->code, model->built_in_disk, level->built_in_disk);
//
//			if (model->bluetooth != level->bluetooth)
//				printf("\t版本“%s”蓝牙不同：%d %d。\n", model->code, model->bluetooth, level->bluetooth);
//
//			if (model->tv != level->tv)
//				printf("\t版本“%s”车载电视不同：%d %d。\n", model->code, model->tv, level->tv);
//
//			if (model->rear_lcd_screen != level->rear_lcd_screen)
//				printf("\t版本“%s”后排液晶屏不同：%d %d。\n", model->code, model->rear_lcd_screen, level->rear_lcd_screen);
//
//			if (model->aux != level->aux)
//				printf("\t版本“%s”外接音源接口不同：%d %d。\n", model->code, model->aux, level->aux);
//
//			if (model->mp3 != level->mp3)
//				printf("\t版本“%s”音频支持mp3不同：%d %d。\n", model->code, model->mp3, level->mp3);
//
//			if (model->single_disc_cd != level->single_disc_cd)
//				printf("\t版本“%s”单碟cd不同：%d %d。\n", model->code, model->single_disc_cd, level->single_disc_cd);
//
//			if (model->multi_disc_cd != level->multi_disc_cd)
//				printf("\t版本“%s”多碟cd不同：%d %d。\n", model->code, model->multi_disc_cd, level->multi_disc_cd);
//
//			if (model->virtual_multi_disc_cd != level->virtual_multi_disc_cd)
//				printf("\t版本“%s”虚拟多碟cd不同：%d %d。\n", model->code, model->virtual_multi_disc_cd, level->virtual_multi_disc_cd);
//
//			if (model->single_disc_dvd != level->single_disc_dvd)
//				printf("\t版本“%s”单碟dvd不同：%d %d。\n", model->code, model->single_disc_dvd, level->single_disc_dvd);
//
//			if (model->multi_disc_dvd != level->multi_disc_dvd)
//				printf("\t版本“%s”多碟dvd不同：%d %d。\n", model->code, model->multi_disc_dvd, level->multi_disc_dvd);
//
//			if (model->ac != level->ac)
//				printf("\t版本“%s”空调不同：%d %d。\n", model->code, model->ac, level->ac);
//
//			if (model->thermatic != level->thermatic)
//				printf("\t版本“%s”自动空调不同：%d %d。\n", model->code, model->thermatic, level->thermatic);
//
//			if (model->rear_ac != level->rear_ac)
//				printf("\t版本“%s”后排独立空调不同：%d %d。\n", model->code, model->rear_ac, level->rear_ac);
//
//			if (model->rear_air_outlet != level->rear_air_outlet)
//				printf("\t版本“%s”后座出风口不同：%d %d。\n", model->code, model->rear_air_outlet, level->rear_air_outlet);
//
//			if (model->temp_zc != level->temp_zc)
//				printf("\t版本“%s”温度分区控制不同：%d %d。\n", model->code, model->temp_zc, level->temp_zc);
//
//			if (model->pollen_filter != level->pollen_filter)
//				printf("\t版本“%s”空气调节不同：%d %d。\n", model->code, model->pollen_filter, level->pollen_filter);
//
//			if (model->fridge != level->fridge)
//				printf("\t版本“%s”车载冰箱不同：%d %d。\n", model->code, model->fridge, level->fridge);
//
//			if (model->park_assist_vision != level->park_assist_vision)
//				printf("\t版本“%s”自动泊车入位不同：%d %d。\n", model->code, model->park_assist_vision, level->park_assist_vision);
//
//			if (model->nvs != level->nvs)
//				printf("\t版本“%s”夜视系统不同：%d %d。\n", model->code, model->nvs, level->nvs);
//
//			if (model->splitview != level->splitview)
//				printf("\t版本“%s”中控液晶屏分屏显示不同：%d %d。\n", model->code, model->splitview, level->splitview);
//
//			if (model->acc != level->acc)
//				printf("\t版本“%s”自适应巡航不同：%d %d。\n", model->code, model->acc, level->acc);
//
//			if (model->pancam != level->pancam)
//				printf("\t版本“%s”全景摄像头不同：%d %d。\n", model->code, model->pancam, level->pancam);
//
//			if (model->parking_sensor != level->parking_sensor)
//				printf("\t版本“%s”倒车雷达不同：%d %d。\n", model->code, model->parking_sensor, level->parking_sensor);
//
//			if (model->telematics != level->telematics)
//				printf("\t版本“%s”车载信息服务不同：%d %d。\n", model->code, model->telematics, level->telematics);
//
//			if (strcmp(model->color, level->color) != 0)
//				printf("\t版本“%s”车身颜色不同：%s %s。\n", model->code, model->color, level->color);
//		}
//
//		return true;
//	}
//
//	model = reinterpret_cast<Variant*>(Variant_tb->allocate());
//	Variant_tb->push_back(reinterpret_cast<char*>(model));
//	strncpy_ex(model->code, code, 40);
//	Variant_map[model->code] = model;
//	model->id = static_cast<int32_t>(Variant_tb->size());
//	Variant_id_map[model->id] = model;
//
//	model->model_id = model->model_id;
//	model->model_year = level->model_year;
//	model->emiss_stand = level->emiss_stand;
//	model->engine_id = engine->engine_id;
//	strncpy_ex(model->name, level->model, 20);
//	model->msrp = level->msrp;
//	model->year_start = level->year_start;
//	model->month_start = level->month_start;
//	model->year_make = level->year_make;
//	model->year_stop = level->year_stop;
//	model->product_stat = level->product_stat;
//	model->sale_stat = level->sale_stat;
//	model->intake_charge_type = level->intake_charge_type;
//	model->max_horsepower = level->max_horsepower;
//	model->max_power = level->max_power;
//	model->max_rpm = level->max_rpm;
//	model->max_rpm2 = level->max_rpm2;
//	model->max_torque = level->max_torque;
//	model->max_torque_rpm = level->max_torque_rpm;
//	model->max_torque_rpm2 = level->max_torque_rpm2;
//	model->compression_ratio = level->compression_ratio;
//	model->fuel_economy_mix = level->fuel_economy_mix;
//	model->fuel_economy_urb = level->fuel_economy_urb;
//	model->fuel_economy_suburb = level->fuel_economy_suburb;
//	model->acc_time = level->acc_time;
//	model->max_speed = level->max_speed;
//	model->tran_desc = level->tran_desc;
//	model->gears_num = level->gears_num;
//	model->front_brake_type = level->front_brake_type;
//	model->rear_brake_type = level->rear_brake_type;
//	model->front_suspension_type = level->front_suspension_type;
//	model->rear_suspension_type = level->rear_suspension_type;
//	model->steering_type = level->steering_type;
//	model->steering_aid_type = level->steering_aid_type;
//	model->min_grd_clearance = level->min_grd_clearance;
//	model->min_turn_radius = level->min_turn_radius;
//	model->engine_position = level->engine_position;
//	model->drive_mode = level->drive_mode;
//	model->body = level->body;
//	model->length = level->length;
//	model->width = level->width;
//	model->height = level->height;
//	model->wheelbase = level->wheelbase;
//	model->front_track = level->front_track;
//	model->rear_track = level->rear_track;
//	model->curb_weight = level->curb_weight;
//	model->gross_weight = level->gross_weight;
//	model->fuel_tank_cap = level->fuel_tank_cap;
//	model->cargo_cap = level->cargo_cap;
//	model->cargo_cap2 = level->cargo_cap2;
//	model->door_num = level->door_num;
//	model->seat_num = level->seat_num;
//	model->num_speaker = level->num_speaker;
//	model->front_tire_spec = level->front_tire_spec;
//	model->rear_tire_spec = level->rear_tire_spec;
//	model->front_rim_spec = level->front_rim_spec;
//	model->rear_rim_spec = level->rear_rim_spec;
//	model->rim_mat = level->rim_mat;
//	model->spare_tire_spec = level->spare_tire_spec;
//	model->drivers_airbag = level->drivers_airbag;
//	model->passenger_airbag = level->passenger_airbag;
//	model->front_side_airbag = level->front_side_airbag;
//	model->rear_side_airbag = level->rear_side_airbag;
//	model->front_head_airbag = level->front_head_airbag;
//	model->rear_head_airbag = level->rear_head_airbag;
//	model->knee_airbag = level->knee_airbag;
//	model->tpms = level->tpms;
//	model->rsc = level->rsc;
//	model->sbr = level->sbr;
//	model->isofix = level->isofix;
//	model->latch = level->latch;
//	model->immobilizer = level->immobilizer;
//	model->central_lock = level->central_lock;
//	model->rke = level->rke;
//	model->peps = level->peps;
//	model->abs = level->abs;
//	model->ebd = level->ebd;
//	model->eba = level->eba;
//	model->tcs = level->tcs;
//	model->esp = level->esp;
//	model->epb = level->epb;
//	model->hdc = level->hdc;
//	model->avs = level->avs;
//	model->air_suspension = level->air_suspension;
//	model->variable_ratio_steering = level->variable_ratio_steering;
//	model->blis = level->blis;
//	model->autobrake = level->autobrake;
//	model->afs = level->afs;
//	model->leather_steering_wheel = level->leather_steering_wheel;
//	model->steering_wheel_adjust = level->steering_wheel_adjust;
//	model->steering_wheel_adjust2 = level->steering_wheel_adjust2;
//	model->steering_wheel_electric_regulation = level->steering_wheel_electric_regulation;
//	model->mfw = level->mfw;
//	model->steering_wheel_gearshift = level->steering_wheel_gearshift;
//	model->leather_seats = level->leather_seats;
//	model->sports_seats = level->sports_seats;
//	model->seat_height_adjust = level->seat_height_adjust;
//	model->lumbar_support_adjust = level->lumbar_support_adjust;
//	model->shoulder_support_adjust = level->shoulder_support_adjust;
//	model->drivers_esa = level->drivers_esa;
//	model->passenger_esa = level->passenger_esa;
//	model->second_row_backrest_adjust = level->second_row_backrest_adjust;
//	model->second_row_move = level->second_row_move;
//	model->rear_esa = level->rear_esa;
//	model->seat_position_memory = level->seat_position_memory;
//	model->front_heated_seat = level->front_heated_seat;
//	model->rear_heated_seat = level->rear_heated_seat;
//	model->seat_ventilate = level->seat_ventilate;
//	model->seat_massage = level->seat_massage;
//	model->rear_seat_recline = level->rear_seat_recline;
//	model->rear_seat_recline_part = level->rear_seat_recline_part;
//	model->third_seat_row = level->third_seat_row;
//	model->front_central_rest = level->front_central_rest;
//	model->rear_central_rest = level->rear_central_rest;
//	model->rear_cup_shelf = level->rear_cup_shelf;
//	model->ambient_lighting = level->ambient_lighting;
//	model->rear_window_sunshade = level->rear_window_sunshade;
//	model->rear_side_sunshade = level->rear_side_sunshade;
//	model->sun_visor = level->sun_visor;
//	model->electric_trunk = level->electric_trunk;
//	model->sport_appearance_package = level->sport_appearance_package;
//	model->electric_suction_door = level->electric_suction_door;
//	model->electric_sunroof = level->electric_sunroof;
//	model->panoramic_sunroof = level->panoramic_sunroof;
//	model->hid = level->hid;
//	model->led = level->led;
//	model->drl = level->drl;
//	model->automatic_headlights = level->automatic_headlights;
//	model->acl = level->acl;
//	model->front_fog_lamp = level->front_fog_lamp;
//	model->headlight_height_adjust = level->headlight_height_adjust;
//	model->sra = level->sra;
//	model->front_power_window = level->front_power_window;
//	model->rear_power_window = level->rear_power_window;
//	model->anti_pinch_window = level->anti_pinch_window;
//	model->insulating_glass = level->insulating_glass;
//	model->electric_rearview_mirror = level->electric_rearview_mirror;
//	model->heated_rearview_mirror = level->heated_rearview_mirror;
//	model->auto_dimming_rearview_mirror = level->auto_dimming_rearview_mirror;
//	model->electric_fold_rearview_mirror = level->electric_fold_rearview_mirror;
//	model->position_memory_rearview_mirror = level->position_memory_rearview_mirror;
//	model->rear_wiper = level->rear_wiper;
//	model->sensing_wiper = level->sensing_wiper;
//	model->ccs = level->ccs;
//	model->park_assist = level->park_assist;
//	model->reverse_video_image = level->reverse_video_image;
//	model->ecu_screen = level->ecu_screen;
//	model->hud = level->hud;
//	model->gps = level->gps;
//	model->gps_is = level->gps_is;
//	model->central_control_screen = level->central_control_screen;
//	model->hmi = level->hmi;
//	model->built_in_disk = level->built_in_disk;
//	model->bluetooth = level->bluetooth;
//	model->tv = level->tv;
//	model->rear_lcd_screen = level->rear_lcd_screen;
//	model->aux = level->aux;
//	model->mp3 = level->mp3;
//	model->single_disc_cd = level->single_disc_cd;
//	model->multi_disc_cd = level->multi_disc_cd;
//	model->virtual_multi_disc_cd = level->virtual_multi_disc_cd;
//	model->single_disc_dvd = level->single_disc_dvd;
//	model->multi_disc_dvd = level->multi_disc_dvd;
//	model->ac = level->ac;
//	model->thermatic = level->thermatic;
//	model->rear_ac = level->rear_ac;
//	model->rear_air_outlet = level->rear_air_outlet;
//	model->temp_zc = level->temp_zc;
//	model->pollen_filter = level->pollen_filter;
//	model->fridge = level->fridge;
//	model->park_assist_vision = level->park_assist_vision;
//	model->nvs = level->nvs;
//	model->splitview = level->splitview;
//	model->acc = level->acc;
//	model->pancam = level->pancam;
//	model->parking_sensor = level->parking_sensor;
//	model->telematics = level->telematics;
//	strncpy_ex(model->color, level->color, 160);
//
//	return true;
//}

bool ModelParse::run(DDEDatabase &db, int _mode, bool _check)
{
	check = _check;
	mode = _mode;
	if (!initialize(db))
		return false;

	size_t size = LevelModel_tb->size();
	if (!size)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		level = reinterpret_cast<LevelModel*>(LevelModel_tb->at(i));
		printf("正在处理车型：%s\n", level->code);

		if (!parse_brand())
		{
			printf("\t品牌解析失败！\n");
			continue;
		}

		if (!parse_series())
		{
			printf("\t车系解析失败！\n");
			continue;
		}

		if (!parse_manufacturer())
		{
			printf("\t厂家解析失败！\n");
			continue;
		}

		if (!parse_engine())
		{
			printf("\t发动机解析失败！\n");
			continue;
		}

		if (!parse_model())
		{
			printf("\t车型解析失败！\n");
			continue;
		}
	}

	if (!parse_epc_model())
		printf("\tEPC车型解析失败！\n");

	return true;
}

bool ModelParse::parse_epc_model()
{
	const_char_map<set<Model*>> yun_models;
	const_char_map<Level2YunData*> level2yuns;
	size_t size = Level2YunData_tb->size();
	for (size_t i = 0; i < size; ++i)
	{
		Level2YunData *data = reinterpret_cast<Level2YunData*>(Level2YunData_tb->at(i));
		Model *model = Model_map[data->level_code];
		if (!model)
			continue;

		yun_models[data->yun_code].insert(model);
		level2yuns[data->level_code] = data;

		if (*(data->yun_code) == '\0')
		{
			printf("\tEPC车型为空：%s\n", data->level_code);
			continue;
		}

		auto it = Brand_code_map.find(data->brand_code);
		if (it == Brand_code_map.end())
			continue;

		Brand *brand = it->second;

		EPCModel *yunmodel = nullptr;
		auto it2 = EPCModel_map.find(data->yun_code);
		if (it2 == EPCModel_map.end())
		{
			yunmodel = reinterpret_cast<EPCModel*>(EPCModel_tb->allocate());
			EPCModel_tb->push_back(reinterpret_cast<char*>(yunmodel));
			strncpy_ex(yunmodel->epc_model_code, data->yun_code, 20);
			strncpy_ex(yunmodel->epc_model_name, data->name, 40);
			EPCModel_map[yunmodel->epc_model_code] = yunmodel;
			yunmodel->epc_model_id = static_cast<int32_t>(EPCModel_tb->size());
			yunmodel->brand_id = brand->brand_id;
		}
		else
		{
			yunmodel = it2->second;
			if (check)
			{
				if (yunmodel->brand_id != brand->brand_id)
					printf("\t云车配车型“%s”品牌不同：%d %d。\n", data->yun_code, yunmodel->brand_id, brand->brand_id);

				if (strcmp(yunmodel->epc_model_name, data->name) != 0)
					printf("\t云车配车型“%s”名称不同：%s %s。\n", data->yun_code, yunmodel->epc_model_name, data->name);
			}
		}

		auto it3 = ModelTransfer_map.find(data->level_code);
		if (it3 == ModelTransfer_map.end())
		{
			ModelTransfer *level2yun = reinterpret_cast<ModelTransfer*>(ModelTransfer_tb->allocate());
			ModelTransfer_tb->push_back(reinterpret_cast<char*>(level2yun));
			strncpy_ex(level2yun->model_code, data->level_code, 20);
			strncpy_ex(level2yun->epc_model_code, data->yun_code, 20);
			ModelTransfer_map[level2yun->model_code] = level2yun;
			level2yun->epc_model_id = yunmodel->epc_model_id;
			level2yun->model_id = model->model_id;
		}
		else
		{
			ModelTransfer *level2yun = it3->second;
			if (check)
			{
				if (level2yun->epc_model_id != yunmodel->epc_model_id)
				{
					level2yun->epc_model_id = yunmodel->epc_model_id;
					printf("\t力洋车型“%s”对应的云车配车型不同：%d %d。\n", data->level_code, level2yun->epc_model_id, yunmodel->epc_model_id);
				}
			}
		}
	}

	for (auto it = yun_models.begin(); it != yun_models.end(); ++it)
	{
		set<Model*> &models = it->second;
		for (auto it2 = models.begin(); it2 != models.end(); ++it2)
		{
			Model *model = *it2;
			if (model)
				printf("\t%s\t%s\t%s\t%s\t%s\t%s\n", model->model_code, model->model_name, Engine_id_map[model->engine_id]->engine_code, model->chassis, it->first, level2yuns[model->model_code]->name);
		}
	}

	return true;
}

