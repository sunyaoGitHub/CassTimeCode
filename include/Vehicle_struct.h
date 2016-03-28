#ifndef __VEHICLE_STRUCT_H__
#define __VEHICLE_STRUCT_H__

#pragma once

#include <stdint.h>
#include <time.h>

const int32_t ID_TABLE_CONSTANTCLASS = 1;	// ��������
const int32_t ID_TABLE_CONSTANTVALUE = 2;	// ����ֵ
const int32_t ID_TABLE_BRAND = 3;	// Ʒ��
const int32_t ID_TABLE_SERIES = 4;	// ��ϵ
const int32_t ID_TABLE_MANUFACTURER = 5;	// ����
const int32_t ID_TABLE_ENGINE = 6;	// ������
const int32_t ID_TABLE_MODEL = 7;	// ����
const int32_t ID_TABLE_EPCMODEL = 8;	// EPC����
const int32_t ID_TABLE_MODELTRANSFER = 9;	// ����ת��
const int32_t ID_TABLE_OE = 10;	// ԭ����
const int32_t ID_TABLE_AM = 11;	// Ʒ�Ƽ�
const int32_t ID_TABLE_PARTLINK = 12;	// �����ϵ
const int32_t ID_TABLE_OE2MODEL = 13;	// ԭ����2����
const int32_t ID_TABLE_CATALOG = 14;	// Ŀ¼
const int32_t ID_TABLE_OE2CATALOG = 15;	// ���Ŀ¼
const int32_t ID_TABLE_VIN2MODEL = 16;	// VIN��ѯ
const int32_t ID_TABLE_LEVELMODELDATA = 17;	// ������ԭ����
const int32_t ID_TABLE_LEVELMODEL = 18;	// ������
const int32_t ID_TABLE_LEVEL2YUNDATA = 19;	// ����2�Ƴ���ԭ����
const int32_t ID_TABLE_YUNARTICLE = 20;	// �Ƴ������
const int32_t ID_TABLE_JMDATA = 21;	// ����ԭ����
const int32_t ID_TABLE_VOCABULARY = 22;	// ������Ϣ
const int32_t ID_TABLE_UNIDENTIFYOE = 23;	// δ����OE
const int32_t ID_TABLE_OEDATA = 24;	// ԭ��������
const int32_t ID_TABLE_VENDORDATA = 25;	// ����������
const int32_t ID_TABLE_EXCHANGEDATA = 26;	// ��������
const int32_t ID_TABLE_CATALOGMAP = 27;	// Ŀ¼ӳ��
const int32_t ID_TABLE_NAMECONVERT = 28;	// ���Ʊ�׼��
const int32_t ID_TABLE_OEDATAEX = 29;	// ԭ����������չ
const int32_t ID_TABLE_OPTIONCODE = 30;	// ������

// ��������
typedef struct _ConstantClass
{
	int32_t class_id;	// ����ID
	char class_name[24];	// ��������
	char class_descr[28];	// ��������
} ConstantClass;

// ����ֵ
typedef struct _ConstantValue
{
	int32_t class_id;	// ����ID
	int32_t value;	// ֵ
	char descr[40];	// ����
} ConstantValue;

// Ʒ��
typedef struct _Brand
{
	int32_t brand_id;	// Ʒ��ID
	char brand_code[16];	// Ʒ�Ʊ���
	char brand_name_cn[16];	// Ʒ����������
	char brand_name_en[20];	// Ʒ��Ӣ������
} Brand;

// ��ϵ
typedef struct _Series
{
	int32_t series_id;	// ��ϵID
	int32_t brand_id;	// Ʒ��ID
	char series_name_cn[24];	// ��ϵ��������
	char series_name_en[24];	// ��ϵӢ������
} Series;

// ����
typedef struct _Manufacturer
{
	int32_t manu_id;	// ����ID
	char manu_name[20];	// ��������
	uint16_t import;	// �������ʽ���
	char country[14];	// ����
} Manufacturer;

// ������
typedef struct _Engine
{
	int32_t engine_id;	// ������ID
	char engine_code[14];	// �������ͺ�
	uint16_t cylinder_volume;	// �����ݻ�
	float displacement;	// ����(��)
	uint8_t intake_charge_type;	// ������ʽ
	uint8_t fuel_type;	// ȼ������
	uint8_t fuel_grade;	// ȼ�ͱ��
	uint8_t cylinder_align;	// ����������ʽ
	uint8_t cylinder_num;	// ������(��)
	uint8_t valve_num;	// ÿ��������(��)
	uint8_t fuel_system_type;	// ���ͷ�ʽ
} Engine;

// ����
typedef struct _Model
{
	int32_t model_id;	// ����ID
	int32_t series_id;	// ��ϵID
	int32_t manu_id;	// ����ID
	int32_t engine_id;	// ������ID
	char model_code[20];	// ���ͱ���
	char model_name[32];	// ��������
	char chassis[16];	// ����/���̺�
	float msrp;	// ָ���۸�(��Ԫ)
	float compression_ratio;	// ѹ����
	float fuel_economy_mix;	// ���Ų��ۺ��ͺ�(l)
	float fuel_economy_urb;	// ���������ͺ�
	float fuel_economy_suburb;	// �н������ͺ�
	float acc_time;	// ����ʱ��(0-100km/h)
	float min_turn_radius;	// ��Сת��뾶
	uint16_t model_year;	// ���
	uint16_t max_horsepower;	// �������(ps)
	uint16_t max_power;	// �����(kw)
	uint16_t max_rpm;	// �����ת��(rpm)
	uint16_t max_rpm2;	// �����ת������(rpm)
	uint16_t max_torque;	// ���Ť��(n��m)
	uint16_t max_torque_rpm;	// ���Ť��ת��(rpm)
	uint16_t max_torque_rpm2;	// ���Ť��ת������(rpm)
	uint16_t max_speed;	// ��߳���(km/h)
	uint16_t min_grd_clearance;	// ��С��ؼ�϶(mm)
	uint16_t length;	// ��(mm)
	uint16_t width;	// ��(mm)
	uint16_t height;	// ��(mm)
	uint16_t wheelbase;	// ���(mm)
	uint16_t front_track;	// ǰ�־�(mm)
	uint16_t rear_track;	// ���־�(mm)
	uint16_t curb_weight;	// ��������(kg)
	uint16_t gross_weight;	// �����������(kg)
	uint16_t fuel_tank_cap;	// �����ݻ�(l)
	uint16_t cargo_cap;	// �������ݻ�(l)
	uint16_t cargo_cap2;	// �������ݻ�����(l)
	uint16_t year_make;	// �������
	uint16_t year_stop;	// ͣ�����
	uint16_t year_start;	// �������
	uint8_t month_start;	// �����·�
	uint8_t emiss_stand;	// �ŷű�׼
	uint8_t vehicle_type;	// ��������
	uint8_t vehicle_level;	// ��������
	uint8_t product_stat;	// ����״̬
	uint8_t sale_stat;	// ����״̬
	uint8_t tran_desc;	// ����������
	uint8_t gears_num;	// ��λ��
	uint8_t front_brake_type;	// ǰ�ƶ�������
	uint8_t rear_brake_type;	// ���ƶ�������
	uint8_t front_suspension_type;	// ǰ��������
	uint8_t rear_suspension_type;	// ����������
	uint8_t steering_type;	// ת�����ʽ
	uint8_t steering_aid_type;	// ��������
	uint8_t engine_position;	// ������λ��
	uint8_t drive_mode;	// ������ʽ
	uint8_t body;	// ������ʽ
	uint8_t door_num;	// ������
	uint8_t seat_num;	// ��λ��(��)
	uint8_t num_speaker;	// ����������
	uint8_t front_tire_spec;	// ǰ��̥���
	uint8_t rear_tire_spec;	// ����̥���
	uint8_t front_rim_spec;	// ǰ��챹��
	uint8_t rear_rim_spec;	// ����챹��
	uint8_t rim_mat;	// ��챲���
	uint8_t spare_tire_spec;	// ��̥���
} Model;

// EPC����
typedef struct _EPCModel
{
	int32_t epc_model_id;	// ����ID
	int32_t brand_id;	// Ʒ��ID
	char epc_model_code[16];	// ���ͱ���
	char epc_model_name[32];	// ��������
} EPCModel;

// ����ת��
typedef struct _ModelTransfer
{
	int32_t model_id;	// ����ID
	char model_code[20];	// ���ͱ���
	int32_t epc_model_id;	// ����ID
	char epc_model_code[16];	// EPC���ͱ���
} ModelTransfer;

// ԭ����
typedef struct _OE
{
	int32_t oe_id;	// ԭ����ID
	int32_t brand_id;	// Ʒ��ID
	char oe_code[24];	// ԭ��������
	char oe_name[64];	// ԭ��������
	char info[48];	// ������Ϣ
	double price_4s;	// 4S��۸�
	char key_word[64];	// ������
} OE;

// Ʒ�Ƽ�
typedef struct _AM
{
	int32_t am_id;	// Ʒ�Ƽ�ID
	int32_t brand_id;	// Ʒ��ID
	char am_code[24];	// Ʒ�Ƽ�����
	char am_name[64];	// Ʒ�Ƽ�����
} AM;

// �����ϵ
typedef struct _PartLink
{
	int32_t left_id;	// ���ID
	int32_t right_id;	// ���ID
	int32_t relation;	// ��ϵ
} PartLink;

// ԭ����2����
typedef struct _OE2Model
{
	int32_t oe_id;	// ԭ����ID
	int32_t epc_model_id;	// ����ID
	char option_code[160];	// ѡ����
} OE2Model;

// Ŀ¼
typedef struct _Catalog
{
	int32_t cat_id;	// Ŀ¼ID
	int32_t parent_cat_id;	// ��Ŀ¼ID
	char cat_name[48];	// Ŀ¼����
	int32_t sort_order;	// ����
	char key_word[124];	// ������
} Catalog;

// ���Ŀ¼
typedef struct _OE2Catalog
{
	int32_t oe_id;	// ԭ����ID
	int32_t cat_id;	// Ŀ¼���
} OE2Catalog;

// VIN��ѯ
typedef struct _VIN2Model
{
	char vin[20];	// VIN��
	int32_t model_id;	// ����ID
	char option_code[200];	// ѡ����
} VIN2Model;

// ������ԭ����
typedef struct _LevelModelData
{
	char code[20];	// ���ͱ���
	char manu[20];	// ����
	char brand[20];	// Ʒ��
	char series[20];	// ��ϵ
	char model[56];	// ����
	char chassis[16];	// ����/���̺�
	char sale_name[40];	// ��������
	char variant[20];	// ���۰汾
	char sale_name_ah[40];	// ��������(����֮��)
	char text1[20];	// �����ı�1
	char text2[40];	// �����ı�2
	uint16_t model_year;	// ���
	char emiss_stand[10];	// �ŷű�׼
	char vehicle_type[12];	// ��������
	char vehicle_level[12];	// ��������
	float msrp;	// ָ���۸�(��Ԫ)
	uint16_t year_start;	// �������
	uint8_t month_start;	// �����·�
	uint16_t year_make;	// �������
	uint16_t year_stop;	// ͣ�����
	char product_stat[8];	// ����״̬
	char sale_stat[8];	// ����״̬
	char country[16];	// ����
	char import[8];	// �������ʽ���
	char engine[18];	// �������ͺ�
	uint16_t cylinder_volume;	// �����ݻ�
	float displacement;	// ����(��)
	char intake_charge_type[20];	// ������ʽ
	char fuel_type[12];	// ȼ������
	uint8_t fuel_grade;	// ȼ�ͱ��
	uint16_t max_horsepower;	// �������(ps)
	uint16_t max_power;	// �����(kw)
	char max_rpm[12];	// �����ת��(rpm)
	uint16_t max_torque;	// ���Ť��(n��m)
	char max_torque_rpm[12];	// ���Ť��ת��(rpm)
	char cylinder_align[4];	// ����������ʽ
	uint8_t cylinder_num;	// ������(��)
	uint8_t valve_num;	// ÿ��������(��)
	float compression_ratio;	// ѹ����
	char fuel_system_type[12];	// ���ͷ�ʽ
	float fuel_economy_mix;	// ���Ų��ۺ��ͺ�(l)
	float fuel_economy_urb;	// ���������ͺ�
	float fuel_economy_suburb;	// �н������ͺ�
	float acc_time;	// ����ʱ��(0-100km/h)
	uint16_t max_speed;	// ��߳���(km/h)
	char tran_type[8];	// ����������
	char tran_desc[20];	// ����������
	uint8_t gears_num;	// ��λ��
	char front_brake_type[20];	// ǰ�ƶ�������
	char rear_brake_type[20];	// ���ƶ�������
	char front_suspension_type[40];	// ǰ��������
	char rear_suspension_type[40];	// ����������
	char steering_type[12];	// ת�����ʽ
	char steering_aid_type[16];	// ��������
	uint16_t min_grd_clearance;	// ��С��ؼ�϶(mm)
	float min_turn_radius;	// ��Сת��뾶
	char engine_position[8];	// ������λ��
	char drive_mode[12];	// ������ʽ
	char drive_type[12];	// ������ʽ
	char body[8];	// ������ʽ
	uint16_t length;	// ��(mm)
	uint16_t width;	// ��(mm)
	uint16_t height;	// ��(mm)
	uint16_t wheelbase;	// ���(mm)
	uint16_t front_track;	// ǰ�־�(mm)
	uint16_t rear_track;	// ���־�(mm)
	uint16_t curb_weight;	// ��������(kg)
	uint16_t gross_weight;	// �����������(kg)
	uint16_t fuel_tank_cap;	// �����ݻ�(l)
	char cargo_cap[20];	// �������ݻ�(l)
	char door_num[8];	// ������
	uint8_t seat_num;	// ��λ��(��)
	uint8_t num_speaker;	// ����������
	char front_tire_spec[20];	// ǰ��̥���
	char rear_tire_spec[20];	// ����̥���
	char front_rim_spec[20];	// ǰ��챹��
	char rear_rim_spec[20];	// ����챹��
	char rim_mat[12];	// ��챲���
	char spare_tire_spec[12];	// ��̥���
	char drivers_airbag[8];	// ��ʻ����ȫ����
	char passenger_airbag[8];	// ����ʻ��ȫ����
	char front_side_airbag[8];	// ǰ�Ų�����
	char rear_side_airbag[8];	// ���Ų�����
	char front_head_airbag[8];	// ǰ��ͷ������(����)
	char rear_head_airbag[8];	// ����ͷ������(����)
	char knee_airbag[8];	// ϥ������
	char tpms[8];	// ̥ѹ���װ��
	char rsc[8];	// ��̥ѹ������ʻ
	char sbr[8];	// ��ȫ��δϵ��ʾ
	char isofix[8];	// isofix��ͯ���νӿ�
	char latch[8];	// latch���νӿ�
	char immobilizer[8];	// ���������ӷ���
	char central_lock[8];	// �п���
	char rke[8];	// ң��Կ��
	char peps[8];	// ��Կ������ϵͳ
	char abs[8];	// abs������
	char ebd[8];	// �ƶ�������(ebd/cbc��)
	char eba[8];	// ɲ������(eba/bas/ba��)
	char tcs[8];	// ǣ��������(asr/tcs/trc��)
	char esp[8];	// �����ȶ�����(esp/dsc/vsc��)
	char epb[8];	// �Զ�פ��/���¸���
	char hdc[8];	// ���»���
	char avs[8];	// �ɱ�����
	char air_suspension[8];	// ��������
	char variable_ratio_steering[8];	// �ɱ�ת���
	char blis[8];	// ���߸���
	char autobrake[8];	// ����ɲ��
	char afs[8];	// ����ת��ϵͳ
	char leather_steering_wheel[8];	// ��Ƥ������
	char steering_wheel_adjust[8];	// ���������µ���
	char steering_wheel_adjust2[8];	// ������ǰ�����
	char steering_wheel_electric_regulation[8];	// �����̵綯����
	char mfw[8];	// �๦�ܷ�����
	char steering_wheel_gearshift[8];	// �����̻���
	char leather_seats[8];	// ��Ƥ����
	char sports_seats[8];	// �˶�����
	char seat_height_adjust[8];	// ���θߵ͵���
	char lumbar_support_adjust[8];	// ����֧�ŵ���
	char shoulder_support_adjust[8];	// �粿֧�ŵ���
	char drivers_esa[8];	// ��ʻ�����ε綯����
	char passenger_esa[8];	// ����ʻ�����ε綯����
	char second_row_backrest_adjust[8];	// �ڶ��ſ����Ƕȵ���
	char second_row_move[8];	// �ڶ��������ƶ�
	char rear_esa[8];	// �������ε綯����
	char seat_position_memory[8];	// �綯���μ���
	char front_heated_seat[8];	// ǰ�����μ���
	char rear_heated_seat[8];	// �������μ���
	char seat_ventilate[8];	// ����ͨ��
	char seat_massage[8];	// ���ΰ�Ħ
	char rear_seat_recline[8];	// ������������ŵ�
	char rear_seat_recline_part[8];	// �������α����ŵ�
	char third_seat_row[8];	// ����������
	char front_central_rest[8];	// ǰ���������
	char rear_central_rest[8];	// �����������
	char rear_cup_shelf[8];	// ���ű���
	char ambient_lighting[8];	// ���ڷ�Χ��
	char rear_window_sunshade[8];	// ��絲������
	char rear_side_sunshade[8];	// ���Ų�������
	char sun_visor[8];	// �����廯ױ��
	char electric_trunk[8];	// �綯����
	char sport_appearance_package[8];	// �˶�����׼�
	char electric_suction_door[8];	// �綯������
	char electric_sunroof[8];	// �綯�촰
	char panoramic_sunroof[8];	// ȫ���촰
	char hid[8];	// ������
	char led[8];	// led���
	char drl[8];	// �ռ��г���
	char automatic_headlights[8];	// �Զ�ͷ��
	char acl[8];	// ת��ͷ��
	char front_fog_lamp[8];	// ǰ���
	char headlight_height_adjust[8];	// ��Ƹ߶ȿɵ�
	char sra[8];	// �����ϴװ��
	char front_power_window[8];	// ǰ�綯����
	char rear_power_window[8];	// ��綯����
	char anti_pinch_window[8];	// ���������ֹ���
	char insulating_glass[8];	// ���Ȳ���
	char electric_rearview_mirror[8];	// ���Ӿ��綯����
	char heated_rearview_mirror[8];	// ���Ӿ�����
	char auto_dimming_rearview_mirror[8];	// ���Ӿ��Զ���ѣĿ
	char electric_fold_rearview_mirror[8];	// ���Ӿ��綯�۵�
	char position_memory_rearview_mirror[8];	// ���Ӿ�����
	char rear_wiper[8];	// ����ˢ
	char sensing_wiper[8];	// ��Ӧ��ˢ
	char ccs[8];	// ����Ѳ��
	char park_assist[8];	// ��������
	char reverse_video_image[8];	// ������ƵӰ��
	char ecu_screen[8];	// �г�������ʾ��
	char hud[8];	// huḑͷ������ʾ
	char gps[8];	// gps����
	char gps_is[8];	// ��λ��������
	char central_control_screen[8];	// �п�̨��ɫ����
	char hmi[8];	// �˻�����ϵͳ
	char built_in_disk[8];	// ����Ӳ��
	char bluetooth[8];	// ����/���ص绰
	char tv[8];	// ���ص���
	char rear_lcd_screen[8];	// ����Һ����
	char aux[8];	// �����Դ�ӿ�(aux/usb/ipod��)
	char mp3[8];	// ��Ƶ֧��mp3
	char single_disc_cd[8];	// ����cd
	char multi_disc_cd[8];	// ���cd
	char virtual_multi_disc_cd[8];	// ������cd
	char single_disc_dvd[8];	// ����dvd
	char multi_disc_dvd[8];	// ���dvd
	char ac[8];	// �յ�
	char thermatic[8];	// �Զ��յ�
	char rear_ac[8];	// ���Ŷ����յ�
	char rear_air_outlet[8];	// ���������
	char temp_zc[8];	// �¶ȷ�������
	char pollen_filter[8];	// ��������/���۹���
	char fridge[8];	// ���ر���
	char park_assist_vision[8];	// �Զ�������λ
	char nvs[8];	// ҹ��ϵͳ
	char splitview[8];	// �п�Һ����������ʾ
	char acc[8];	// ����ӦѲ��
	char pancam[8];	// ȫ������ͷ
	char parking_sensor[8];	// �����״�
	char telematics[8];	// ������Ϣ����
	char color[160];	// ������ɫ
} LevelModelData;

// ������
typedef struct _LevelModel
{
	char code[20];	// ���ͱ���
	char manu[20];	// ����
	char brand[20];	// Ʒ��
	char series[20];	// ��ϵ
	char model[56];	// ����
	char chassis[16];	// ����/���̺�
	char sale_name[40];	// ��������
	char variant[20];	// ���۰汾
	char sale_name_ah[40];	// ��������(����֮��)
	char text1[20];	// �����ı�1
	char text2[40];	// �����ı�2
	char country[16];	// ����
	char engine[16];	// �������ͺ�
	float msrp;	// ָ���۸�(��Ԫ)
	float displacement;	// ����(��)
	float compression_ratio;	// ѹ����
	float fuel_economy_mix;	// ���Ų��ۺ��ͺ�(l)
	float fuel_economy_urb;	// ���������ͺ�
	float fuel_economy_suburb;	// �н������ͺ�
	float acc_time;	// ����ʱ��(0-100km/h)
	float min_turn_radius;	// ��Сת��뾶
	uint16_t model_year;	// ���
	uint16_t cylinder_volume;	// �����ݻ�
	uint16_t max_horsepower;	// �������(ps)
	uint16_t max_power;	// �����(kw)
	uint16_t max_rpm;	// �����ת��(rpm)
	uint16_t max_rpm2;	// �����ת������(rpm)
	uint16_t max_torque;	// ���Ť��(n��m)
	uint16_t max_torque_rpm;	// ���Ť��ת��(rpm)
	uint16_t max_torque_rpm2;	// ���Ť��ת������(rpm)
	uint16_t max_speed;	// ��߳���(km/h)
	uint16_t min_grd_clearance;	// ��С��ؼ�϶(mm)
	uint16_t length;	// ��(mm)
	uint16_t width;	// ��(mm)
	uint16_t height;	// ��(mm)
	uint16_t wheelbase;	// ���(mm)
	uint16_t front_track;	// ǰ�־�(mm)
	uint16_t rear_track;	// ���־�(mm)
	uint16_t curb_weight;	// ��������(kg)
	uint16_t gross_weight;	// �����������(kg)
	uint16_t fuel_tank_cap;	// �����ݻ�(l)
	uint16_t cargo_cap;	// �������ݻ�(l)
	uint16_t cargo_cap2;	// �������ݻ�����(l)
	uint16_t year_make;	// �������
	uint16_t year_stop;	// ͣ�����
	uint16_t year_start;	// �������
	uint8_t month_start;	// �����·�
	uint8_t emiss_stand;	// �ŷű�׼
	uint8_t vehicle_type;	// ��������
	uint8_t vehicle_level;	// ��������
	uint8_t product_stat;	// ����״̬
	uint8_t sale_stat;	// ����״̬
	uint8_t import;	// �������ʽ���
	uint8_t intake_charge_type;	// ������ʽ
	uint8_t fuel_type;	// ȼ������
	uint8_t fuel_grade;	// ȼ�ͱ��
	uint8_t cylinder_align;	// ����������ʽ
	uint8_t cylinder_num;	// ������(��)
	uint8_t valve_num;	// ÿ��������(��)
	uint8_t fuel_system_type;	// ���ͷ�ʽ
	uint8_t tran_type;	// ����������
	uint8_t tran_desc;	// ����������
	uint8_t gears_num;	// ��λ��
	uint8_t front_brake_type;	// ǰ�ƶ�������
	uint8_t rear_brake_type;	// ���ƶ�������
	uint8_t front_suspension_type;	// ǰ��������
	uint8_t rear_suspension_type;	// ����������
	uint8_t steering_type;	// ת�����ʽ
	uint8_t steering_aid_type;	// ��������
	uint8_t engine_position;	// ������λ��
	uint8_t drive_mode;	// ������ʽ
	uint8_t drive_type;	// ������ʽ
	uint8_t body;	// ������ʽ
	uint8_t door_num;	// ������
	uint8_t seat_num;	// ��λ��(��)
	uint8_t num_speaker;	// ����������
	uint8_t front_tire_spec;	// ǰ��̥���
	uint8_t rear_tire_spec;	// ����̥���
	uint8_t front_rim_spec;	// ǰ��챹��
	uint8_t rear_rim_spec;	// ����챹��
	uint8_t rim_mat;	// ��챲���
	uint8_t spare_tire_spec;	// ��̥���
	uint8_t drivers_airbag;	// ��ʻ����ȫ����
	uint8_t passenger_airbag;	// ����ʻ��ȫ����
	uint8_t front_side_airbag;	// ǰ�Ų�����
	uint8_t rear_side_airbag;	// ���Ų�����
	uint8_t front_head_airbag;	// ǰ��ͷ������(����)
	uint8_t rear_head_airbag;	// ����ͷ������(����)
	uint8_t knee_airbag;	// ϥ������
	uint8_t tpms;	// ̥ѹ���װ��
	uint8_t rsc;	// ��̥ѹ������ʻ
	uint8_t sbr;	// ��ȫ��δϵ��ʾ
	uint8_t isofix;	// isofix��ͯ���νӿ�
	uint8_t latch;	// latch���νӿ�
	uint8_t immobilizer;	// ���������ӷ���
	uint8_t central_lock;	// �п���
	uint8_t rke;	// ң��Կ��
	uint8_t peps;	// ��Կ������ϵͳ
	uint8_t abs;	// abs������
	uint8_t ebd;	// �ƶ�������(ebd/cbc��)
	uint8_t eba;	// ɲ������(eba/bas/ba��)
	uint8_t tcs;	// ǣ��������(asr/tcs/trc��)
	uint8_t esp;	// �����ȶ�����(esp/dsc/vsc��)
	uint8_t epb;	// �Զ�פ��/���¸���
	uint8_t hdc;	// ���»���
	uint8_t avs;	// �ɱ�����
	uint8_t air_suspension;	// ��������
	uint8_t variable_ratio_steering;	// �ɱ�ת���
	uint8_t blis;	// ���߸���
	uint8_t autobrake;	// ����ɲ��
	uint8_t afs;	// ����ת��ϵͳ
	uint8_t leather_steering_wheel;	// ��Ƥ������
	uint8_t steering_wheel_adjust;	// ���������µ���
	uint8_t steering_wheel_adjust2;	// ������ǰ�����
	uint8_t steering_wheel_electric_regulation;	// �����̵綯����
	uint8_t mfw;	// �๦�ܷ�����
	uint8_t steering_wheel_gearshift;	// �����̻���
	uint8_t leather_seats;	// ��Ƥ����
	uint8_t sports_seats;	// �˶�����
	uint8_t seat_height_adjust;	// ���θߵ͵���
	uint8_t lumbar_support_adjust;	// ����֧�ŵ���
	uint8_t shoulder_support_adjust;	// �粿֧�ŵ���
	uint8_t drivers_esa;	// ��ʻ�����ε綯����
	uint8_t passenger_esa;	// ����ʻ�����ε綯����
	uint8_t second_row_backrest_adjust;	// �ڶ��ſ����Ƕȵ���
	uint8_t second_row_move;	// �ڶ��������ƶ�
	uint8_t rear_esa;	// �������ε綯����
	uint8_t seat_position_memory;	// �綯���μ���
	uint8_t front_heated_seat;	// ǰ�����μ���
	uint8_t rear_heated_seat;	// �������μ���
	uint8_t seat_ventilate;	// ����ͨ��
	uint8_t seat_massage;	// ���ΰ�Ħ
	uint8_t rear_seat_recline;	// ������������ŵ�
	uint8_t rear_seat_recline_part;	// �������α����ŵ�
	uint8_t third_seat_row;	// ����������
	uint8_t front_central_rest;	// ǰ���������
	uint8_t rear_central_rest;	// �����������
	uint8_t rear_cup_shelf;	// ���ű���
	uint8_t ambient_lighting;	// ���ڷ�Χ��
	uint8_t rear_window_sunshade;	// ��絲������
	uint8_t rear_side_sunshade;	// ���Ų�������
	uint8_t sun_visor;	// �����廯ױ��
	uint8_t electric_trunk;	// �綯����
	uint8_t sport_appearance_package;	// �˶�����׼�
	uint8_t electric_suction_door;	// �綯������
	uint8_t electric_sunroof;	// �綯�촰
	uint8_t panoramic_sunroof;	// ȫ���촰
	uint8_t hid;	// ������
	uint8_t led;	// led���
	uint8_t drl;	// �ռ��г���
	uint8_t automatic_headlights;	// �Զ�ͷ��
	uint8_t acl;	// ת��ͷ��
	uint8_t front_fog_lamp;	// ǰ���
	uint8_t headlight_height_adjust;	// ��Ƹ߶ȿɵ�
	uint8_t sra;	// �����ϴװ��
	uint8_t front_power_window;	// ǰ�綯����
	uint8_t rear_power_window;	// ��綯����
	uint8_t anti_pinch_window;	// ���������ֹ���
	uint8_t insulating_glass;	// ���Ȳ���
	uint8_t electric_rearview_mirror;	// ���Ӿ��綯����
	uint8_t heated_rearview_mirror;	// ���Ӿ�����
	uint8_t auto_dimming_rearview_mirror;	// ���Ӿ��Զ���ѣĿ
	uint8_t electric_fold_rearview_mirror;	// ���Ӿ��綯�۵�
	uint8_t position_memory_rearview_mirror;	// ���Ӿ�����
	uint8_t rear_wiper;	// ����ˢ
	uint8_t sensing_wiper;	// ��Ӧ��ˢ
	uint8_t ccs;	// ����Ѳ��
	uint8_t park_assist;	// ��������
	uint8_t reverse_video_image;	// ������ƵӰ��
	uint8_t ecu_screen;	// �г�������ʾ��
	uint8_t hud;	// huḑͷ������ʾ
	uint8_t gps;	// gps����
	uint8_t gps_is;	// ��λ��������
	uint8_t central_control_screen;	// �п�̨��ɫ����
	uint8_t hmi;	// �˻�����ϵͳ
	uint8_t built_in_disk;	// ����Ӳ��
	uint8_t bluetooth;	// ����/���ص绰
	uint8_t tv;	// ���ص���
	uint8_t rear_lcd_screen;	// ����Һ����
	uint8_t aux;	// �����Դ�ӿ�(aux/usb/ipod��)
	uint8_t mp3;	// ��Ƶ֧��mp3
	uint8_t single_disc_cd;	// ����cd
	uint8_t multi_disc_cd;	// ���cd
	uint8_t virtual_multi_disc_cd;	// ������cd
	uint8_t single_disc_dvd;	// ����dvd
	uint8_t multi_disc_dvd;	// ���dvd
	uint8_t ac;	// �յ�
	uint8_t thermatic;	// �Զ��յ�
	uint8_t rear_ac;	// ���Ŷ����յ�
	uint8_t rear_air_outlet;	// ���������
	uint8_t temp_zc;	// �¶ȷ�������
	uint8_t pollen_filter;	// ��������/���۹���
	uint8_t fridge;	// ���ر���
	uint8_t park_assist_vision;	// �Զ�������λ
	uint8_t nvs;	// ҹ��ϵͳ
	uint8_t splitview;	// �п�Һ����������ʾ
	uint8_t acc;	// ����ӦѲ��
	uint8_t pancam;	// ȫ������ͷ
	uint8_t parking_sensor;	// �����״�
	uint8_t telematics;	// ������Ϣ����
	char color[160];	// ������ɫ
} LevelModel;

// ����2�Ƴ���ԭ����
typedef struct _Level2YunData
{
	char level_code[20];	// �����ͱ���
	char yun_code[20];	// �Ƴ��䳵�ͱ���
	char brand_code[16];	// Ʒ�Ʊ���
	char name[40];	// ����
} Level2YunData;

// �Ƴ������
typedef struct _YunArticle
{
	char cat_id[8];	// һ��Ŀ¼���
	char cat_name[48];	// һ��Ŀ¼����
	char cat_id2[8];	// ����Ŀ¼���
	char cat_name2[120];	// ����Ŀ¼����
	char graph_num[10];	// ͼ���
	char code[20];	// �����
	char code2[24];	// �����-����ʽ
	char descr[144];	// �������
	char remark1[160];	// ��ע1
	uint16_t number;	// ����
	char type[128];	// �ͺ�
	char remark2[128];	// ��ʼ��
	char remark3[80];	// ��ֹ��
	char remark4[64];	// �滻ȡ������
	char remark5[32];	// ��ע5
	char remark6[8];	// ��/��
	char remark7[8];	// ��ע7
	char remark8[8];	// ��ע8
	char remark9[12];	// ��ע9
} YunArticle;

// ����ԭ����
typedef struct _JMData
{
	char cat_name[40];	// һ��Ŀ¼����
	char cat_name2[40];	// ����Ŀ¼����
	char cat_name3[40];	// ����Ŀ¼����
	char name[48];	// �������
	char alias[64];	// �������
	int32_t number;	// �������
	char oe_name[64];	// ԭ��������
	char info[96];	// ��ע
} JMData;

// ������Ϣ
typedef struct _Vocabulary
{
	int32_t cat_id;	// Ŀ¼���
	char name[48];	// �������
	char alias[64];	// �������
	int32_t number;	// �������
	char oe_name[64];	// ԭ��������
	char info[96];	// ��ע
} Vocabulary;

// δ����OE
typedef struct _UnIdentifyOE
{
	char brand_code[16];	// Ʒ�Ʊ���
	char oe_code[24];	// ԭ��������
	char cat_id[44];	// һ��Ŀ¼
	char cat2_id[44];	// ����Ŀ¼
	char oe_name[64];	// ԭ��������
	char info[64];	// ��ע
} UnIdentifyOE;

// ԭ��������
typedef struct _OEData
{
	char oe_code[24];	// ԭ��������
	char brand[24];	// Ʒ��
	char oe_name[64];	// ԭ��������
	double price;	// ���ۼ۸�
} OEData;

// ����������
typedef struct _VendorData
{
	char code[24];	// ��Ʒ����
	char type[16];	// ���
	char brand_name_cn[16];	// Ʒ����������
	char brand_code[16];	// Ʒ�Ʊ���
	char name[64];	// ��Ʒ����
	char common[48];	// ͨ������
	char alias[48];	// ����
	char oe_code[24];	// ��Ӧԭ��������
	char am_code[24];	// Ʒ�Ƽ�����
	char vehicle[16];	// ����
	double price_4s;	// 4s��۸�
	double price_cost;	// ��ֵ����
	double price;	// ���ۼ۸�
	int32_t stock;	// ���ۿ��
	int32_t sales_min;	// �������
	char warranty[16];	// ������
	char post_sale[16];	// �ۺ�����
	char quality[16];	// Ʒ�ʵȼ�
	char report[16];	// �ʼ챨��
	char unit[16];	// ������λ
	char pack_size[16];	// ��װ���
	char pack_min[16];	// ��С��װ
	char amount[16];	// ÿ������
	char weight[16];	// ����
	char volume[16];	// ���
	char dimension[16];	// �����
	char color[16];	// ��ɫ
	char attention[16];	// �ر�����
} VendorData;

// ��������
typedef struct _ExchangeData
{
	char code[24];	// ��Ʒ����
	char brand_name_cn[16];	// Ʒ����������
	char brand_code[16];	// Ʒ�Ʊ���
	char am_code[24];	// Ʒ�Ƽ�����
	char name[64];	// ��Ʒ����
	char oe_code[24];	// ԭ��������
	char oe_brand_name_cn[16];	// Ʒ����������
	char oe_brand_code[16];	// ԭ��Ʒ�Ʊ���
	char vendor[48];	// ��Ӧ��
	double price_4s;	// 4S��۸�
	double price;	// ���ۼ۸�
	int32_t stock;	// ���ۿ��
#ifdef __linux
#ifndef __LP64__
	int32_t dummy_1;
#endif
#endif
} ExchangeData;

// Ŀ¼ӳ��
typedef struct _CatalogMap
{
	char cat_name[40];	// һ��Ŀ¼����
	char cat_name2[40];	// ����Ŀ¼����
	char cat_name3[40];	// ����Ŀ¼����
	char epc_cat_name[48];	// EPCһ��Ŀ¼����
	char epc_cat_name2[48];	// EPC����Ŀ¼����
} CatalogMap;

// ���Ʊ�׼��
typedef struct _NameConvert
{
	char cat_name[40];	// һ��Ŀ¼����
	char cat_name2[40];	// ����Ŀ¼����
	char cat_name3[40];	// ����Ŀ¼����
	char epc_cat_name[48];	// EPCһ��Ŀ¼����
	char epc_cat_name2[48];	// EPC����Ŀ¼����
	char oe_code[24];	// ԭ��������
	char oe_name[64];	// ԭ��������
	char info[32];	// ������Ϣ
	char vendor_name[64];	// ������ԭ��������
	char standard_name[48];	// ��׼����
} NameConvert;

// ԭ����������չ
typedef struct _OEDataEx
{
	char oe_code[24];	// ����
	char brand[24];	// Ʒ��
	char erp_name[48];	// ERP����
	char tb_name[48];	// TB����
	char join_name[48];	// JOIN����
	double tb_price;	// TB���ۼ۸�
	double join_price;	// JOIN���ۼ۸�
} OEDataEx;

// ������
typedef struct _OptionCode
{
	char option_code[8];	// ������
	int32_t brand_id;	// Ʒ��ID
	char descr[48];	// �������
} OptionCode;

#endif // __VEHICLE_STRUCT_H__

