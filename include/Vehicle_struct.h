#ifndef __VEHICLE_STRUCT_H__
#define __VEHICLE_STRUCT_H__

#pragma once

#include <stdint.h>
#include <time.h>

const int32_t ID_TABLE_CONSTANTCLASS = 1;	// 常量类型
const int32_t ID_TABLE_CONSTANTVALUE = 2;	// 常量值
const int32_t ID_TABLE_BRAND = 3;	// 品牌
const int32_t ID_TABLE_SERIES = 4;	// 车系
const int32_t ID_TABLE_MANUFACTURER = 5;	// 厂家
const int32_t ID_TABLE_ENGINE = 6;	// 发动机
const int32_t ID_TABLE_MODEL = 7;	// 车型
const int32_t ID_TABLE_EPCMODEL = 8;	// EPC车型
const int32_t ID_TABLE_MODELTRANSFER = 9;	// 车型转换
const int32_t ID_TABLE_OE = 10;	// 原厂件
const int32_t ID_TABLE_AM = 11;	// 品牌件
const int32_t ID_TABLE_PARTLINK = 12;	// 配件关系
const int32_t ID_TABLE_OE2MODEL = 13;	// 原厂件2车型
const int32_t ID_TABLE_CATALOG = 14;	// 目录
const int32_t ID_TABLE_OE2CATALOG = 15;	// 配件目录
const int32_t ID_TABLE_VIN2MODEL = 16;	// VIN查询
const int32_t ID_TABLE_LEVELMODELDATA = 17;	// 力洋车型原数据
const int32_t ID_TABLE_LEVELMODEL = 18;	// 力洋车型
const int32_t ID_TABLE_LEVEL2YUNDATA = 19;	// 力洋2云车配原数据
const int32_t ID_TABLE_YUNARTICLE = 20;	// 云车配配件
const int32_t ID_TABLE_JMDATA = 21;	// 精米原数据
const int32_t ID_TABLE_VOCABULARY = 22;	// 分类信息
const int32_t ID_TABLE_UNIDENTIFYOE = 23;	// 未分类OE
const int32_t ID_TABLE_OEDATA = 24;	// 原厂件数据
const int32_t ID_TABLE_VENDORDATA = 25;	// 经销商数据
const int32_t ID_TABLE_EXCHANGEDATA = 26;	// 交换数据
const int32_t ID_TABLE_CATALOGMAP = 27;	// 目录映射
const int32_t ID_TABLE_NAMECONVERT = 28;	// 名称标准化
const int32_t ID_TABLE_OEDATAEX = 29;	// 原厂件数据扩展
const int32_t ID_TABLE_OPTIONCODE = 30;	// 配置项

// 常量类型
typedef struct _ConstantClass
{
	int32_t class_id;	// 类型ID
	char class_name[24];	// 类型名称
	char class_descr[28];	// 类型描述
} ConstantClass;

// 常量值
typedef struct _ConstantValue
{
	int32_t class_id;	// 类型ID
	int32_t value;	// 值
	char descr[40];	// 描述
} ConstantValue;

// 品牌
typedef struct _Brand
{
	int32_t brand_id;	// 品牌ID
	char brand_code[16];	// 品牌编码
	char brand_name_cn[16];	// 品牌中文名称
	char brand_name_en[20];	// 品牌英文名称
} Brand;

// 车系
typedef struct _Series
{
	int32_t series_id;	// 车系ID
	int32_t brand_id;	// 品牌ID
	char series_name_cn[24];	// 车系中文名称
	char series_name_en[24];	// 车系英文名称
} Series;

// 厂家
typedef struct _Manufacturer
{
	int32_t manu_id;	// 厂家ID
	char manu_name[20];	// 厂家名称
	uint16_t import;	// 国产合资进口
	char country[14];	// 国别
} Manufacturer;

// 发动机
typedef struct _Engine
{
	int32_t engine_id;	// 发动机ID
	char engine_code[14];	// 发动机型号
	uint16_t cylinder_volume;	// 气缸容积
	float displacement;	// 排量(升)
	uint8_t intake_charge_type;	// 进气形式
	uint8_t fuel_type;	// 燃油类型
	uint8_t fuel_grade;	// 燃油标号
	uint8_t cylinder_align;	// 气缸排列形式
	uint8_t cylinder_num;	// 气缸数(个)
	uint8_t valve_num;	// 每缸气门数(个)
	uint8_t fuel_system_type;	// 供油方式
} Engine;

// 车型
typedef struct _Model
{
	int32_t model_id;	// 车型ID
	int32_t series_id;	// 车系ID
	int32_t manu_id;	// 厂家ID
	int32_t engine_id;	// 发动机ID
	char model_code[20];	// 车型编码
	char model_name[32];	// 车型名称
	char chassis[16];	// 代号/底盘号
	float msrp;	// 指导价格(万元)
	float compression_ratio;	// 压缩比
	float fuel_economy_mix;	// 工信部综合油耗(l)
	float fuel_economy_urb;	// 市区工况油耗
	float fuel_economy_suburb;	// 市郊工况油耗
	float acc_time;	// 加速时间(0-100km/h)
	float min_turn_radius;	// 最小转弯半径
	uint16_t model_year;	// 年款
	uint16_t max_horsepower;	// 最大马力(ps)
	uint16_t max_power;	// 最大功率(kw)
	uint16_t max_rpm;	// 最大功率转速(rpm)
	uint16_t max_rpm2;	// 最大功率转速上限(rpm)
	uint16_t max_torque;	// 最大扭矩(n·m)
	uint16_t max_torque_rpm;	// 最大扭矩转速(rpm)
	uint16_t max_torque_rpm2;	// 最大扭矩转速上限(rpm)
	uint16_t max_speed;	// 最高车速(km/h)
	uint16_t min_grd_clearance;	// 最小离地间隙(mm)
	uint16_t length;	// 长(mm)
	uint16_t width;	// 宽(mm)
	uint16_t height;	// 高(mm)
	uint16_t wheelbase;	// 轴距(mm)
	uint16_t front_track;	// 前轮距(mm)
	uint16_t rear_track;	// 后轮距(mm)
	uint16_t curb_weight;	// 整备质量(kg)
	uint16_t gross_weight;	// 最大载重质量(kg)
	uint16_t fuel_tank_cap;	// 油箱容积(l)
	uint16_t cargo_cap;	// 行李厢容积(l)
	uint16_t cargo_cap2;	// 行李厢容积上限(l)
	uint16_t year_make;	// 生产年份
	uint16_t year_stop;	// 停产年份
	uint16_t year_start;	// 上市年份
	uint8_t month_start;	// 上市月份
	uint8_t emiss_stand;	// 排放标准
	uint8_t vehicle_type;	// 车辆类型
	uint8_t vehicle_level;	// 车辆级别
	uint8_t product_stat;	// 生产状态
	uint8_t sale_stat;	// 销售状态
	uint8_t tran_desc;	// 变速器描述
	uint8_t gears_num;	// 档位数
	uint8_t front_brake_type;	// 前制动器类型
	uint8_t rear_brake_type;	// 后制动器类型
	uint8_t front_suspension_type;	// 前悬挂类型
	uint8_t rear_suspension_type;	// 后悬挂类型
	uint8_t steering_type;	// 转向机形式
	uint8_t steering_aid_type;	// 助力类型
	uint8_t engine_position;	// 发动机位置
	uint8_t drive_mode;	// 驱动方式
	uint8_t body;	// 车身型式
	uint8_t door_num;	// 车门数
	uint8_t seat_num;	// 座位数(个)
	uint8_t num_speaker;	// 扬声器数量
	uint8_t front_tire_spec;	// 前轮胎规格
	uint8_t rear_tire_spec;	// 后轮胎规格
	uint8_t front_rim_spec;	// 前轮毂规格
	uint8_t rear_rim_spec;	// 后轮毂规格
	uint8_t rim_mat;	// 轮毂材料
	uint8_t spare_tire_spec;	// 备胎规格
} Model;

// EPC车型
typedef struct _EPCModel
{
	int32_t epc_model_id;	// 车型ID
	int32_t brand_id;	// 品牌ID
	char epc_model_code[16];	// 车型编码
	char epc_model_name[32];	// 车型名称
} EPCModel;

// 车型转换
typedef struct _ModelTransfer
{
	int32_t model_id;	// 车型ID
	char model_code[20];	// 车型编码
	int32_t epc_model_id;	// 车型ID
	char epc_model_code[16];	// EPC车型编码
} ModelTransfer;

// 原厂件
typedef struct _OE
{
	int32_t oe_id;	// 原厂件ID
	int32_t brand_id;	// 品牌ID
	char oe_code[24];	// 原厂件编码
	char oe_name[64];	// 原厂件名称
	char info[48];	// 附加信息
	double price_4s;	// 4S店价格
	char key_word[64];	// 搜索词
} OE;

// 品牌件
typedef struct _AM
{
	int32_t am_id;	// 品牌件ID
	int32_t brand_id;	// 品牌ID
	char am_code[24];	// 品牌件编码
	char am_name[64];	// 品牌件名称
} AM;

// 配件关系
typedef struct _PartLink
{
	int32_t left_id;	// 配件ID
	int32_t right_id;	// 配件ID
	int32_t relation;	// 关系
} PartLink;

// 原厂件2车型
typedef struct _OE2Model
{
	int32_t oe_id;	// 原厂件ID
	int32_t epc_model_id;	// 车型ID
	char option_code[160];	// 选项码
} OE2Model;

// 目录
typedef struct _Catalog
{
	int32_t cat_id;	// 目录ID
	int32_t parent_cat_id;	// 父目录ID
	char cat_name[48];	// 目录名称
	int32_t sort_order;	// 次序
	char key_word[124];	// 搜索词
} Catalog;

// 配件目录
typedef struct _OE2Catalog
{
	int32_t oe_id;	// 原厂件ID
	int32_t cat_id;	// 目录编号
} OE2Catalog;

// VIN查询
typedef struct _VIN2Model
{
	char vin[20];	// VIN号
	int32_t model_id;	// 车型ID
	char option_code[200];	// 选项码
} VIN2Model;

// 力洋车型原数据
typedef struct _LevelModelData
{
	char code[20];	// 车型编码
	char manu[20];	// 厂家
	char brand[20];	// 品牌
	char series[20];	// 车系
	char model[56];	// 车型
	char chassis[16];	// 代号/底盘号
	char sale_name[40];	// 销售名称
	char variant[20];	// 销售版本
	char sale_name_ah[40];	// 销售名称(汽车之家)
	char text1[20];	// 车型文本1
	char text2[40];	// 车型文本2
	uint16_t model_year;	// 年款
	char emiss_stand[10];	// 排放标准
	char vehicle_type[12];	// 车辆类型
	char vehicle_level[12];	// 车辆级别
	float msrp;	// 指导价格(万元)
	uint16_t year_start;	// 上市年份
	uint8_t month_start;	// 上市月份
	uint16_t year_make;	// 生产年份
	uint16_t year_stop;	// 停产年份
	char product_stat[8];	// 生产状态
	char sale_stat[8];	// 销售状态
	char country[16];	// 国别
	char import[8];	// 国产合资进口
	char engine[18];	// 发动机型号
	uint16_t cylinder_volume;	// 气缸容积
	float displacement;	// 排量(升)
	char intake_charge_type[20];	// 进气形式
	char fuel_type[12];	// 燃油类型
	uint8_t fuel_grade;	// 燃油标号
	uint16_t max_horsepower;	// 最大马力(ps)
	uint16_t max_power;	// 最大功率(kw)
	char max_rpm[12];	// 最大功率转速(rpm)
	uint16_t max_torque;	// 最大扭矩(n·m)
	char max_torque_rpm[12];	// 最大扭矩转速(rpm)
	char cylinder_align[4];	// 气缸排列形式
	uint8_t cylinder_num;	// 气缸数(个)
	uint8_t valve_num;	// 每缸气门数(个)
	float compression_ratio;	// 压缩比
	char fuel_system_type[12];	// 供油方式
	float fuel_economy_mix;	// 工信部综合油耗(l)
	float fuel_economy_urb;	// 市区工况油耗
	float fuel_economy_suburb;	// 市郊工况油耗
	float acc_time;	// 加速时间(0-100km/h)
	uint16_t max_speed;	// 最高车速(km/h)
	char tran_type[8];	// 变速箱类型
	char tran_desc[20];	// 变速器描述
	uint8_t gears_num;	// 档位数
	char front_brake_type[20];	// 前制动器类型
	char rear_brake_type[20];	// 后制动器类型
	char front_suspension_type[40];	// 前悬挂类型
	char rear_suspension_type[40];	// 后悬挂类型
	char steering_type[12];	// 转向机形式
	char steering_aid_type[16];	// 助力类型
	uint16_t min_grd_clearance;	// 最小离地间隙(mm)
	float min_turn_radius;	// 最小转弯半径
	char engine_position[8];	// 发动机位置
	char drive_mode[12];	// 驱动方式
	char drive_type[12];	// 驱动形式
	char body[8];	// 车身型式
	uint16_t length;	// 长(mm)
	uint16_t width;	// 宽(mm)
	uint16_t height;	// 高(mm)
	uint16_t wheelbase;	// 轴距(mm)
	uint16_t front_track;	// 前轮距(mm)
	uint16_t rear_track;	// 后轮距(mm)
	uint16_t curb_weight;	// 整备质量(kg)
	uint16_t gross_weight;	// 最大载重质量(kg)
	uint16_t fuel_tank_cap;	// 油箱容积(l)
	char cargo_cap[20];	// 行李厢容积(l)
	char door_num[8];	// 车门数
	uint8_t seat_num;	// 座位数(个)
	uint8_t num_speaker;	// 扬声器数量
	char front_tire_spec[20];	// 前轮胎规格
	char rear_tire_spec[20];	// 后轮胎规格
	char front_rim_spec[20];	// 前轮毂规格
	char rear_rim_spec[20];	// 后轮毂规格
	char rim_mat[12];	// 轮毂材料
	char spare_tire_spec[12];	// 备胎规格
	char drivers_airbag[8];	// 驾驶座安全气囊
	char passenger_airbag[8];	// 副驾驶安全气囊
	char front_side_airbag[8];	// 前排侧气囊
	char rear_side_airbag[8];	// 后排侧气囊
	char front_head_airbag[8];	// 前排头部气囊(气帘)
	char rear_head_airbag[8];	// 后排头部气囊(气帘)
	char knee_airbag[8];	// 膝部气囊
	char tpms[8];	// 胎压监测装置
	char rsc[8];	// 零胎压继续行驶
	char sbr[8];	// 安全带未系提示
	char isofix[8];	// isofix儿童座椅接口
	char latch[8];	// latch座椅接口
	char immobilizer[8];	// 发动机电子防盗
	char central_lock[8];	// 中控锁
	char rke[8];	// 遥控钥匙
	char peps[8];	// 无钥匙启动系统
	char abs[8];	// abs防抱死
	char ebd[8];	// 制动力分配(ebd/cbc等)
	char eba[8];	// 刹车辅助(eba/bas/ba等)
	char tcs[8];	// 牵引力控制(asr/tcs/trc等)
	char esp[8];	// 车身稳定控制(esp/dsc/vsc等)
	char epb[8];	// 自动驻车/上坡辅助
	char hdc[8];	// 陡坡缓降
	char avs[8];	// 可变悬挂
	char air_suspension[8];	// 空气悬挂
	char variable_ratio_steering[8];	// 可变转向比
	char blis[8];	// 并线辅助
	char autobrake[8];	// 主动刹车
	char afs[8];	// 主动转向系统
	char leather_steering_wheel[8];	// 真皮方向盘
	char steering_wheel_adjust[8];	// 方向盘上下调节
	char steering_wheel_adjust2[8];	// 方向盘前后调节
	char steering_wheel_electric_regulation[8];	// 方向盘电动调节
	char mfw[8];	// 多功能方向盘
	char steering_wheel_gearshift[8];	// 方向盘换挡
	char leather_seats[8];	// 真皮座椅
	char sports_seats[8];	// 运动座椅
	char seat_height_adjust[8];	// 座椅高低调节
	char lumbar_support_adjust[8];	// 腰部支撑调节
	char shoulder_support_adjust[8];	// 肩部支撑调节
	char drivers_esa[8];	// 驾驶座座椅电动调节
	char passenger_esa[8];	// 副驾驶座座椅电动调节
	char second_row_backrest_adjust[8];	// 第二排靠背角度调节
	char second_row_move[8];	// 第二排座椅移动
	char rear_esa[8];	// 后排座椅电动调节
	char seat_position_memory[8];	// 电动座椅记忆
	char front_heated_seat[8];	// 前排座椅加热
	char rear_heated_seat[8];	// 后排座椅加热
	char seat_ventilate[8];	// 座椅通风
	char seat_massage[8];	// 座椅按摩
	char rear_seat_recline[8];	// 后排座椅整体放倒
	char rear_seat_recline_part[8];	// 后排座椅比例放倒
	char third_seat_row[8];	// 第三排座椅
	char front_central_rest[8];	// 前座中央扶手
	char rear_central_rest[8];	// 后座中央扶手
	char rear_cup_shelf[8];	// 后排杯架
	char ambient_lighting[8];	// 车内氛围灯
	char rear_window_sunshade[8];	// 后风挡遮阳帘
	char rear_side_sunshade[8];	// 后排侧遮阳帘
	char sun_visor[8];	// 遮阳板化妆镜
	char electric_trunk[8];	// 电动后备箱
	char sport_appearance_package[8];	// 运动外观套件
	char electric_suction_door[8];	// 电动吸合门
	char electric_sunroof[8];	// 电动天窗
	char panoramic_sunroof[8];	// 全景天窗
	char hid[8];	// 氙气大灯
	char led[8];	// led大灯
	char drl[8];	// 日间行车灯
	char automatic_headlights[8];	// 自动头灯
	char acl[8];	// 转向头灯
	char front_fog_lamp[8];	// 前雾灯
	char headlight_height_adjust[8];	// 大灯高度可调
	char sra[8];	// 大灯清洗装置
	char front_power_window[8];	// 前电动车窗
	char rear_power_window[8];	// 后电动车窗
	char anti_pinch_window[8];	// 车窗防夹手功能
	char insulating_glass[8];	// 隔热玻璃
	char electric_rearview_mirror[8];	// 后视镜电动调节
	char heated_rearview_mirror[8];	// 后视镜加热
	char auto_dimming_rearview_mirror[8];	// 后视镜自动防眩目
	char electric_fold_rearview_mirror[8];	// 后视镜电动折叠
	char position_memory_rearview_mirror[8];	// 后视镜记忆
	char rear_wiper[8];	// 后雨刷
	char sensing_wiper[8];	// 感应雨刷
	char ccs[8];	// 定速巡航
	char park_assist[8];	// 泊车辅助
	char reverse_video_image[8];	// 倒车视频影像
	char ecu_screen[8];	// 行车电脑显示屏
	char hud[8];	// hud抬头数字显示
	char gps[8];	// gps导航
	char gps_is[8];	// 定位互动服务
	char central_control_screen[8];	// 中控台彩色大屏
	char hmi[8];	// 人机交互系统
	char built_in_disk[8];	// 内置硬盘
	char bluetooth[8];	// 蓝牙/车载电话
	char tv[8];	// 车载电视
	char rear_lcd_screen[8];	// 后排液晶屏
	char aux[8];	// 外接音源接口(aux/usb/ipod等)
	char mp3[8];	// 音频支持mp3
	char single_disc_cd[8];	// 单碟cd
	char multi_disc_cd[8];	// 多碟cd
	char virtual_multi_disc_cd[8];	// 虚拟多碟cd
	char single_disc_dvd[8];	// 单碟dvd
	char multi_disc_dvd[8];	// 多碟dvd
	char ac[8];	// 空调
	char thermatic[8];	// 自动空调
	char rear_ac[8];	// 后排独立空调
	char rear_air_outlet[8];	// 后座出风口
	char temp_zc[8];	// 温度分区控制
	char pollen_filter[8];	// 空气调节/花粉过滤
	char fridge[8];	// 车载冰箱
	char park_assist_vision[8];	// 自动泊车入位
	char nvs[8];	// 夜视系统
	char splitview[8];	// 中控液晶屏分屏显示
	char acc[8];	// 自适应巡航
	char pancam[8];	// 全景摄像头
	char parking_sensor[8];	// 倒车雷达
	char telematics[8];	// 车载信息服务
	char color[160];	// 车身颜色
} LevelModelData;

// 力洋车型
typedef struct _LevelModel
{
	char code[20];	// 车型编码
	char manu[20];	// 厂家
	char brand[20];	// 品牌
	char series[20];	// 车系
	char model[56];	// 车型
	char chassis[16];	// 代号/底盘号
	char sale_name[40];	// 销售名称
	char variant[20];	// 销售版本
	char sale_name_ah[40];	// 销售名称(汽车之家)
	char text1[20];	// 车型文本1
	char text2[40];	// 车型文本2
	char country[16];	// 国别
	char engine[16];	// 发动机型号
	float msrp;	// 指导价格(万元)
	float displacement;	// 排量(升)
	float compression_ratio;	// 压缩比
	float fuel_economy_mix;	// 工信部综合油耗(l)
	float fuel_economy_urb;	// 市区工况油耗
	float fuel_economy_suburb;	// 市郊工况油耗
	float acc_time;	// 加速时间(0-100km/h)
	float min_turn_radius;	// 最小转弯半径
	uint16_t model_year;	// 年款
	uint16_t cylinder_volume;	// 气缸容积
	uint16_t max_horsepower;	// 最大马力(ps)
	uint16_t max_power;	// 最大功率(kw)
	uint16_t max_rpm;	// 最大功率转速(rpm)
	uint16_t max_rpm2;	// 最大功率转速上限(rpm)
	uint16_t max_torque;	// 最大扭矩(n·m)
	uint16_t max_torque_rpm;	// 最大扭矩转速(rpm)
	uint16_t max_torque_rpm2;	// 最大扭矩转速上限(rpm)
	uint16_t max_speed;	// 最高车速(km/h)
	uint16_t min_grd_clearance;	// 最小离地间隙(mm)
	uint16_t length;	// 长(mm)
	uint16_t width;	// 宽(mm)
	uint16_t height;	// 高(mm)
	uint16_t wheelbase;	// 轴距(mm)
	uint16_t front_track;	// 前轮距(mm)
	uint16_t rear_track;	// 后轮距(mm)
	uint16_t curb_weight;	// 整备质量(kg)
	uint16_t gross_weight;	// 最大载重质量(kg)
	uint16_t fuel_tank_cap;	// 油箱容积(l)
	uint16_t cargo_cap;	// 行李厢容积(l)
	uint16_t cargo_cap2;	// 行李厢容积上限(l)
	uint16_t year_make;	// 生产年份
	uint16_t year_stop;	// 停产年份
	uint16_t year_start;	// 上市年份
	uint8_t month_start;	// 上市月份
	uint8_t emiss_stand;	// 排放标准
	uint8_t vehicle_type;	// 车辆类型
	uint8_t vehicle_level;	// 车辆级别
	uint8_t product_stat;	// 生产状态
	uint8_t sale_stat;	// 销售状态
	uint8_t import;	// 国产合资进口
	uint8_t intake_charge_type;	// 进气形式
	uint8_t fuel_type;	// 燃油类型
	uint8_t fuel_grade;	// 燃油标号
	uint8_t cylinder_align;	// 气缸排列形式
	uint8_t cylinder_num;	// 气缸数(个)
	uint8_t valve_num;	// 每缸气门数(个)
	uint8_t fuel_system_type;	// 供油方式
	uint8_t tran_type;	// 变速箱类型
	uint8_t tran_desc;	// 变速器描述
	uint8_t gears_num;	// 档位数
	uint8_t front_brake_type;	// 前制动器类型
	uint8_t rear_brake_type;	// 后制动器类型
	uint8_t front_suspension_type;	// 前悬挂类型
	uint8_t rear_suspension_type;	// 后悬挂类型
	uint8_t steering_type;	// 转向机形式
	uint8_t steering_aid_type;	// 助力类型
	uint8_t engine_position;	// 发动机位置
	uint8_t drive_mode;	// 驱动方式
	uint8_t drive_type;	// 驱动形式
	uint8_t body;	// 车身型式
	uint8_t door_num;	// 车门数
	uint8_t seat_num;	// 座位数(个)
	uint8_t num_speaker;	// 扬声器数量
	uint8_t front_tire_spec;	// 前轮胎规格
	uint8_t rear_tire_spec;	// 后轮胎规格
	uint8_t front_rim_spec;	// 前轮毂规格
	uint8_t rear_rim_spec;	// 后轮毂规格
	uint8_t rim_mat;	// 轮毂材料
	uint8_t spare_tire_spec;	// 备胎规格
	uint8_t drivers_airbag;	// 驾驶座安全气囊
	uint8_t passenger_airbag;	// 副驾驶安全气囊
	uint8_t front_side_airbag;	// 前排侧气囊
	uint8_t rear_side_airbag;	// 后排侧气囊
	uint8_t front_head_airbag;	// 前排头部气囊(气帘)
	uint8_t rear_head_airbag;	// 后排头部气囊(气帘)
	uint8_t knee_airbag;	// 膝部气囊
	uint8_t tpms;	// 胎压监测装置
	uint8_t rsc;	// 零胎压继续行驶
	uint8_t sbr;	// 安全带未系提示
	uint8_t isofix;	// isofix儿童座椅接口
	uint8_t latch;	// latch座椅接口
	uint8_t immobilizer;	// 发动机电子防盗
	uint8_t central_lock;	// 中控锁
	uint8_t rke;	// 遥控钥匙
	uint8_t peps;	// 无钥匙启动系统
	uint8_t abs;	// abs防抱死
	uint8_t ebd;	// 制动力分配(ebd/cbc等)
	uint8_t eba;	// 刹车辅助(eba/bas/ba等)
	uint8_t tcs;	// 牵引力控制(asr/tcs/trc等)
	uint8_t esp;	// 车身稳定控制(esp/dsc/vsc等)
	uint8_t epb;	// 自动驻车/上坡辅助
	uint8_t hdc;	// 陡坡缓降
	uint8_t avs;	// 可变悬挂
	uint8_t air_suspension;	// 空气悬挂
	uint8_t variable_ratio_steering;	// 可变转向比
	uint8_t blis;	// 并线辅助
	uint8_t autobrake;	// 主动刹车
	uint8_t afs;	// 主动转向系统
	uint8_t leather_steering_wheel;	// 真皮方向盘
	uint8_t steering_wheel_adjust;	// 方向盘上下调节
	uint8_t steering_wheel_adjust2;	// 方向盘前后调节
	uint8_t steering_wheel_electric_regulation;	// 方向盘电动调节
	uint8_t mfw;	// 多功能方向盘
	uint8_t steering_wheel_gearshift;	// 方向盘换挡
	uint8_t leather_seats;	// 真皮座椅
	uint8_t sports_seats;	// 运动座椅
	uint8_t seat_height_adjust;	// 座椅高低调节
	uint8_t lumbar_support_adjust;	// 腰部支撑调节
	uint8_t shoulder_support_adjust;	// 肩部支撑调节
	uint8_t drivers_esa;	// 驾驶座座椅电动调节
	uint8_t passenger_esa;	// 副驾驶座座椅电动调节
	uint8_t second_row_backrest_adjust;	// 第二排靠背角度调节
	uint8_t second_row_move;	// 第二排座椅移动
	uint8_t rear_esa;	// 后排座椅电动调节
	uint8_t seat_position_memory;	// 电动座椅记忆
	uint8_t front_heated_seat;	// 前排座椅加热
	uint8_t rear_heated_seat;	// 后排座椅加热
	uint8_t seat_ventilate;	// 座椅通风
	uint8_t seat_massage;	// 座椅按摩
	uint8_t rear_seat_recline;	// 后排座椅整体放倒
	uint8_t rear_seat_recline_part;	// 后排座椅比例放倒
	uint8_t third_seat_row;	// 第三排座椅
	uint8_t front_central_rest;	// 前座中央扶手
	uint8_t rear_central_rest;	// 后座中央扶手
	uint8_t rear_cup_shelf;	// 后排杯架
	uint8_t ambient_lighting;	// 车内氛围灯
	uint8_t rear_window_sunshade;	// 后风挡遮阳帘
	uint8_t rear_side_sunshade;	// 后排侧遮阳帘
	uint8_t sun_visor;	// 遮阳板化妆镜
	uint8_t electric_trunk;	// 电动后备箱
	uint8_t sport_appearance_package;	// 运动外观套件
	uint8_t electric_suction_door;	// 电动吸合门
	uint8_t electric_sunroof;	// 电动天窗
	uint8_t panoramic_sunroof;	// 全景天窗
	uint8_t hid;	// 氙气大灯
	uint8_t led;	// led大灯
	uint8_t drl;	// 日间行车灯
	uint8_t automatic_headlights;	// 自动头灯
	uint8_t acl;	// 转向头灯
	uint8_t front_fog_lamp;	// 前雾灯
	uint8_t headlight_height_adjust;	// 大灯高度可调
	uint8_t sra;	// 大灯清洗装置
	uint8_t front_power_window;	// 前电动车窗
	uint8_t rear_power_window;	// 后电动车窗
	uint8_t anti_pinch_window;	// 车窗防夹手功能
	uint8_t insulating_glass;	// 隔热玻璃
	uint8_t electric_rearview_mirror;	// 后视镜电动调节
	uint8_t heated_rearview_mirror;	// 后视镜加热
	uint8_t auto_dimming_rearview_mirror;	// 后视镜自动防眩目
	uint8_t electric_fold_rearview_mirror;	// 后视镜电动折叠
	uint8_t position_memory_rearview_mirror;	// 后视镜记忆
	uint8_t rear_wiper;	// 后雨刷
	uint8_t sensing_wiper;	// 感应雨刷
	uint8_t ccs;	// 定速巡航
	uint8_t park_assist;	// 泊车辅助
	uint8_t reverse_video_image;	// 倒车视频影像
	uint8_t ecu_screen;	// 行车电脑显示屏
	uint8_t hud;	// hud抬头数字显示
	uint8_t gps;	// gps导航
	uint8_t gps_is;	// 定位互动服务
	uint8_t central_control_screen;	// 中控台彩色大屏
	uint8_t hmi;	// 人机交互系统
	uint8_t built_in_disk;	// 内置硬盘
	uint8_t bluetooth;	// 蓝牙/车载电话
	uint8_t tv;	// 车载电视
	uint8_t rear_lcd_screen;	// 后排液晶屏
	uint8_t aux;	// 外接音源接口(aux/usb/ipod等)
	uint8_t mp3;	// 音频支持mp3
	uint8_t single_disc_cd;	// 单碟cd
	uint8_t multi_disc_cd;	// 多碟cd
	uint8_t virtual_multi_disc_cd;	// 虚拟多碟cd
	uint8_t single_disc_dvd;	// 单碟dvd
	uint8_t multi_disc_dvd;	// 多碟dvd
	uint8_t ac;	// 空调
	uint8_t thermatic;	// 自动空调
	uint8_t rear_ac;	// 后排独立空调
	uint8_t rear_air_outlet;	// 后座出风口
	uint8_t temp_zc;	// 温度分区控制
	uint8_t pollen_filter;	// 空气调节/花粉过滤
	uint8_t fridge;	// 车载冰箱
	uint8_t park_assist_vision;	// 自动泊车入位
	uint8_t nvs;	// 夜视系统
	uint8_t splitview;	// 中控液晶屏分屏显示
	uint8_t acc;	// 自适应巡航
	uint8_t pancam;	// 全景摄像头
	uint8_t parking_sensor;	// 倒车雷达
	uint8_t telematics;	// 车载信息服务
	char color[160];	// 车身颜色
} LevelModel;

// 力洋2云车配原数据
typedef struct _Level2YunData
{
	char level_code[20];	// 力洋车型编码
	char yun_code[20];	// 云车配车型编码
	char brand_code[16];	// 品牌编码
	char name[40];	// 名称
} Level2YunData;

// 云车配配件
typedef struct _YunArticle
{
	char cat_id[8];	// 一级目录编号
	char cat_name[48];	// 一级目录名称
	char cat_id2[8];	// 二级目录编号
	char cat_name2[120];	// 二级目录名称
	char graph_num[10];	// 图编号
	char code[20];	// 零件号
	char code2[24];	// 零件号-长格式
	char descr[144];	// 零件描述
	char remark1[160];	// 备注1
	uint16_t number;	// 件数
	char type[128];	// 型号
	char remark2[128];	// 起始年
	char remark3[80];	// 终止年
	char remark4[64];	// 替换取消代码
	char remark5[32];	// 备注5
	char remark6[8];	// 左/右
	char remark7[8];	// 备注7
	char remark8[8];	// 备注8
	char remark9[12];	// 备注9
} YunArticle;

// 精米原数据
typedef struct _JMData
{
	char cat_name[40];	// 一级目录名称
	char cat_name2[40];	// 二级目录名称
	char cat_name3[40];	// 三级目录名称
	char name[48];	// 零件名称
	char alias[64];	// 零件别名
	int32_t number;	// 零件数量
	char oe_name[64];	// 原厂件名称
	char info[96];	// 备注
} JMData;

// 分类信息
typedef struct _Vocabulary
{
	int32_t cat_id;	// 目录编号
	char name[48];	// 零件名称
	char alias[64];	// 零件别名
	int32_t number;	// 零件数量
	char oe_name[64];	// 原厂件名称
	char info[96];	// 备注
} Vocabulary;

// 未分类OE
typedef struct _UnIdentifyOE
{
	char brand_code[16];	// 品牌编码
	char oe_code[24];	// 原厂件编码
	char cat_id[44];	// 一级目录
	char cat2_id[44];	// 二级目录
	char oe_name[64];	// 原厂件名称
	char info[64];	// 备注
} UnIdentifyOE;

// 原厂件数据
typedef struct _OEData
{
	char oe_code[24];	// 原厂件编码
	char brand[24];	// 品牌
	char oe_name[64];	// 原厂件名称
	double price;	// 销售价格
} OEData;

// 经销商数据
typedef struct _VendorData
{
	char code[24];	// 商品编码
	char type[16];	// 类别
	char brand_name_cn[16];	// 品牌中文名称
	char brand_code[16];	// 品牌编码
	char name[64];	// 商品名称
	char common[48];	// 通用名称
	char alias[48];	// 别名
	char oe_code[24];	// 对应原厂件编码
	char am_code[24];	// 品牌件编码
	char vehicle[16];	// 车型
	double price_4s;	// 4s店价格
	double price_cost;	// 保值单价
	double price;	// 销售价格
	int32_t stock;	// 可售库存
	int32_t sales_min;	// 最低售量
	char warranty[16];	// 保质期
	char post_sale[16];	// 售后周期
	char quality[16];	// 品质等级
	char report[16];	// 质检报告
	char unit[16];	// 计量单位
	char pack_size[16];	// 包装规格
	char pack_min[16];	// 最小包装
	char amount[16];	// 每车用量
	char weight[16];	// 重量
	char volume[16];	// 体积
	char dimension[16];	// 长宽高
	char color[16];	// 颜色
	char attention[16];	// 特别提醒
} VendorData;

// 交换数据
typedef struct _ExchangeData
{
	char code[24];	// 商品编码
	char brand_name_cn[16];	// 品牌中文名称
	char brand_code[16];	// 品牌编码
	char am_code[24];	// 品牌件编码
	char name[64];	// 商品名称
	char oe_code[24];	// 原厂件编码
	char oe_brand_name_cn[16];	// 品牌中文名称
	char oe_brand_code[16];	// 原厂品牌编码
	char vendor[48];	// 供应商
	double price_4s;	// 4S店价格
	double price;	// 销售价格
	int32_t stock;	// 可售库存
#ifdef __linux
#ifndef __LP64__
	int32_t dummy_1;
#endif
#endif
} ExchangeData;

// 目录映射
typedef struct _CatalogMap
{
	char cat_name[40];	// 一级目录名称
	char cat_name2[40];	// 二级目录名称
	char cat_name3[40];	// 三级目录名称
	char epc_cat_name[48];	// EPC一级目录名称
	char epc_cat_name2[48];	// EPC二级目录名称
} CatalogMap;

// 名称标准化
typedef struct _NameConvert
{
	char cat_name[40];	// 一级目录名称
	char cat_name2[40];	// 二级目录名称
	char cat_name3[40];	// 三级目录名称
	char epc_cat_name[48];	// EPC一级目录名称
	char epc_cat_name2[48];	// EPC二级目录名称
	char oe_code[24];	// 原厂件编码
	char oe_name[64];	// 原厂件名称
	char info[32];	// 附加信息
	char vendor_name[64];	// 经销商原厂件名称
	char standard_name[48];	// 标准名称
} NameConvert;

// 原厂件数据扩展
typedef struct _OEDataEx
{
	char oe_code[24];	// 编码
	char brand[24];	// 品牌
	char erp_name[48];	// ERP名称
	char tb_name[48];	// TB名称
	char join_name[48];	// JOIN名称
	double tb_price;	// TB销售价格
	double join_price;	// JOIN销售价格
} OEDataEx;

// 配置项
typedef struct _OptionCode
{
	char option_code[8];	// 配置码
	int32_t brand_id;	// 品牌ID
	char descr[48];	// 配置项含义
} OptionCode;

#endif // __VEHICLE_STRUCT_H__

