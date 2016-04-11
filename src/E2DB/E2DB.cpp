#include "stdafx.h"
#include "E2DB.h"

class e_document_ex: public e_document
{
public:
	e_document_ex();
	virtual ~e_document_ex();

	virtual void parse_error(const char *what, void *where) override;
};

e_document_ex::e_document_ex() : e_document()
{
}

e_document_ex::~e_document_ex()
{
}

void e_document_ex::parse_error(const char *what, void *where)
{
	e_document::parse_error(what, where);
	if (where)
	{
		char buffer[100];
		strncpy_ex(buffer, (char *)where, 100);
		printf("parse error: %s\n\tnear: %s\n", what, buffer);
	} 
	else
	{
		printf("parse error: %s\n", what);
	}
}

#define TRACE(SQL)           \
	if(m_trace){             \
	cout << SQL << endl; \
	}

E2DB::E2DB() : db(0)
{
}

E2DB::~E2DB()
{
	if (m_otl_connect.connected)
	{
		try
		{
			m_otl_connect.commit();
			m_otl_connect.logoff();
		}
		catch(otl_exception &p)
		{
			cerr << "[error] commit to dbserver error: " << endl; // print out error message
			cerr<<p.msg<<endl; // print out error message
			cerr<<p.stm_text<<endl; // print out SQL that caused the error
			cerr<<p.sqlstate<<endl; // print out SQLSTATE message
			cerr<<p.var_info<<endl; // print out the variable that caused the error
		}
	}
}

bool E2DB::run(const char *_data_file, const char *_conf_file)
{
	conf_file = _conf_file;
	if (!read_configure())
		return false;

	char scheme_file[1024];
	xml_attribute<> *attribute = root_conf->first_attribute("scheme", 0, false);
	if (attribute)
	{
		if (!get_absolute_path(scheme_file, attribute->value(), conf_dir))
		{
			printf("配置文件“%s”E2DB.scheme内容“%s”错误，异常退出！\n", conf_file, attribute->value());
			return false;
		}
	}
	else
	{
		char exe_dir[1024];
		get_exe_path(scheme_file, 1024);
		get_file_dir(exe_dir, scheme_file, 1024);
		get_absolute_path(scheme_file, "../../common/conf/DDE/Vehicle.e", exe_dir);
	}

	DDEScheme scheme;
	if (!scheme.parse_file(scheme_file))
	{
		printf("配置文件“%s”不存在或错误，异常退出！\n", scheme_file);
		return false;
	}

	db.scheme(&scheme);

	attribute = root_conf->first_attribute("connect_str", 0, false);
	if (!attribute)
		return false;

	otl_connect::otl_initialize(); //初始化OTL 数据库连接环境
	try
	{
		m_otl_connect.rlogon(attribute->value());//建立数据库连接
	}
	catch(otl_exception& p)
	{
		cerr << "[error] connect to dbserver error with: " << attribute->value() << endl;
		cerr<<p.msg<<endl; // print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return false;
	}

	if (!m_otl_connect.connected)//连接成功
		return false;

	//char level_code[20];
	//int yun_id;
	//try
	//{
	//	otl_stream i(50, "select * from ModelTransfer", m_otl_connect);
	//	while(!i.eof())
	//	{
	//		i >> level_code >> yun_id;
	//		printf("%s %d\n", level_code, yun_id);
	//	}

	//	otl_stream s(100, "$SQLTables $1:'%' $2:'' $3:'ModelTransfer'", m_otl_connect); 
	//	while(!s.eof())
	//	{
	//		s >> level_code;
	//		printf("table: %s\n", level_code);
	//	}
	//}
	//catch(otl_exception& p)
	//{
	//	cerr << "msg: " <<p.msg<<endl; // print out error message
	//	cerr << "stm_text: " << p.stm_text<<endl; // print out SQL that caused the error
	//	cerr << "sqlstate: " <<p.sqlstate<<endl; // print out SQLSTATE message
	//	cerr << "var_info: " <<p.var_info<<endl; // print out the variable that caused the error
	//	return false;
	//}

	data_file = _data_file;
	if (!read_data()) //读取数据文件
	{
		printf("数据读取失败！\n");
		return false;
	}

	if (!write_db())
	{
		printf("数据入库失败！\n");
		return false;
	}

	return true;
}

// 读取配置文件
bool E2DB::read_configure()
{
	try
	{
		xml_conf.parse_file<0>(conf_file);
	}
	catch (parse_error &e)
	{
		printf("配置文件“%s”解析错误: %s，位置：%s，异常退出！\n", conf_file, e.what(), e.where<char>());
		return false;
	}

	root_conf = xml_conf.first_node("E2DB", 0, false);
	if (!root_conf)
	{
		printf("配置文件“%s”内容无效，异常退出！\n", conf_file);
		return false;
	}

	xml_attribute<> *attribute = root_conf->first_attribute("echo_time", 0, false);
	if (attribute && ((*attribute->value()) == '1'))
		TimeConsume::s_mode = 1;
	else
		TimeConsume::s_mode = 0;

	get_file_dir(conf_dir, conf_file, 1024);
	return true;
}

bool E2DB::read_data()
{
	TimeConsume tc("读取数据");
	e_document_ex doc;
	if (!doc.parse_file(data_file))
		return false;

	E2DDE efile(db.scheme());
	efile.parse(db, doc, e_dde_parse_combine);

	return true;
}

bool E2DB::write_db()
{
	for (size_t i = 0; i < db.m_tables.size(); ++i)  //每一个表分别入库
	{
		DDETable *table = db.m_tables[i];
		if (table_exist(table))//判断表是否存在
			insert_table(table);
		else
			printf("%s no existed!\n", table->m_name.c_str());
	}

	return true;
}

bool E2DB::table_exist(const DDETable *table)
{
	char sqlCmd[128] = {0};
	sprintf(sqlCmd,"$SQLTables $1:'%%' $2:'%%' $3:'%s'", table->m_name.c_str());
	otl_stream os(100, sqlCmd, m_otl_connect);
	return !os.eof();
}

string field_type_desc(const DDEColumnScheme *column)
{
	switch(column->type())
	{
	case DDE_DATA_CHAR:
		if (column->size() > 1)
		{
			char type_desc[32];
			sprintf(type_desc, "<char[%u]>", column->size());
			return type_desc;
		}
		else
			return "<char[2]>";
		break;

	case DDE_DATA_INT8:
	case DDE_DATA_UINT8:
		//return "<char[2]>";
		return "<short>";

	case DDE_DATA_INT16:
	case DDE_DATA_UINT16:
		return "<short>";

	case DDE_DATA_INT32:
		return "<int>";

	case DDE_DATA_UINT32:
		return "<unsigned>";

	case DDE_DATA_INT64:
		return "<bigint>";

	case DDE_DATA_UINT64:
		return "<ubigint>";

	case DDE_DATA_FLOAT:
		return "<float>";

	case DDE_DATA_DOUBLE:
		return "<double>";

	case DDE_DATA_DATE:
	case DDE_DATA_TIME:
	case DDE_DATA_DATETIME:
		return "<timestamp>";

	default:
		assert(false);
		break;
	}
	return "";
}

void set_value(otl_stream &os, const DDEColumnScheme *column, char *record)
{
	uint16_t offset = column->offset();
	switch(column->type())
	{
	case DDE_DATA_CHAR:
		if (column->size() == 1)
			os << *(record + offset);
		else
			os << (record + offset);
		break;

	case DDE_DATA_INT8:
		os << static_cast<short>(*(record +offset));
		break;

	case DDE_DATA_UINT8:
		os << static_cast<short>(*reinterpret_cast<unsigned char*>(record +offset));
		break;

	case DDE_DATA_INT16:
	case DDE_DATA_UINT16:
		os << *reinterpret_cast<short*>(record + offset);
		break;

	case DDE_DATA_INT32:
		os << *reinterpret_cast<int*>(record + offset);
		break;

	case DDE_DATA_UINT32:
		os << *reinterpret_cast<unsigned int*>(record + offset);
		break; 

	case DDE_DATA_INT64:
		os << *reinterpret_cast<int64_t*>(record + offset);
		break;

	case DDE_DATA_UINT64:
		os << *reinterpret_cast<uint64_t*>(record + offset);
		break;

	case DDE_DATA_FLOAT:
		os << *reinterpret_cast<float*>(record + offset);
		break;

	case DDE_DATA_DOUBLE:
		os << *reinterpret_cast<double*>(record + offset);
		break;

	default:
		break;
	}
}

bool E2DB::insert_table(const DDETable *table)
{
	DDETableScheme *scheme = table->scheme();

	string sql = "insert into ";
	sql += table->m_name;
	sql += " values (";
	for(size_t i = 0; i < scheme->columns().size(); ++i)
	{
		DDEColumnScheme *column = (scheme->columns())[i];
		sql += ":";
		sql += column->name();
		sql += field_type_desc(column);
		sql += ",";
	}

	sql.erase(sql.size()-1);
	sql += ")";

	printf(sql.c_str());
	printf("\n");

	try
	{
		//otl_cursor::direct_exec(m_otl_connect, "set unique_checks=0");
		//otl_cursor::direct_exec(m_otl_connect, "set foreign_key_checks=0");
		//otl_cursor::direct_exec(m_otl_connect, "set autocommit=0");
		//otl_cursor::direct_exec(m_otl_connect, "start transaction");
		otl_nocommit_stream os(3000, sql.c_str(), m_otl_connect);
		//os.set_flush(false);
		//os.set_commit(0);
		for(size_t i = 0; i < table->size(); ++i)  //每一行，数据有多列，每一行对应列分别写入
		{
			char* record = const_cast<char*>(table->at(i));  
			for(size_t j = 0; j < scheme->columns().size(); ++j)
			{
				DDEColumnScheme *column = (scheme->columns())[j];
				set_value(os,column,record);
			}
		}
		os.flush();
		//m_otl_connect.commit();
		//otl_cursor::direct_exec(m_otl_connect, "commit");
		//otl_cursor::direct_exec(m_otl_connect, "set unique_checks=1");
		//otl_cursor::direct_exec(m_otl_connect, "set foreign_key_checks=1");
		//otl_cursor::direct_exec(m_otl_connect, "set autocommit=1");

		return true;
	}
	catch(otl_exception &p)
	{
		cerr<<p.msg<<endl; // print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return false;
	}

//	size_t size = table->m_name.size() + 24;
//	size += scheme->buffer_size() * 6000;
//	printf("%s %u\n", table->m_name.c_str(), size);
//	char *buffer = new char[size];
//	char *dst = buffer;
//	int count = 0;
//	for (size_t i=0; i < table->size(); i++)
//	{
//		if (!count)
//		{
//			dst = buffer;
//			dst += sprintf(dst, "insert into %s values (", table->m_name.c_str());
//		}
//		else
//		{
//			*dst++ = ',';
//			*dst++ = '(';
//		}
//
//		char *data = const_cast<char*>(table->at(i));
//		for(size_t j = 0; j < scheme->columns().size(); ++j)
//		{
//			DDEColumnScheme *column = (scheme->columns())[j];
//			char *src = data + column->offset();
//			switch (column->type())
//			{
//			case DDE_DATA_CHAR:
//				{
//					*dst++ = '\'';
//					if (column->size() == 1)
//						*dst++ = *src;
//					else
//					{
//						while (*src)
//							*dst++ = *src++;
//					}
//
//					*dst++ = '\'';
//					*dst++ = ',';
//				}
//				break;
//
//			case DDE_DATA_BOOL:
//				{
//					bool *col_data = reinterpret_cast<bool *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						*dst++ = *col_data ? '1' : '0';
//						*dst++ = ',';
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_INT8:
//				{
//					int8_t *col_data = reinterpret_cast<int8_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%d,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_UINT8:
//				{
//					uint8_t *col_data = reinterpret_cast<uint8_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%u,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_INT16:
//				{
//					int16_t *col_data = reinterpret_cast<int16_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%d,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_UINT16:
//				{
//					uint16_t *col_data = reinterpret_cast<uint16_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%u,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_INT32:
//				{
//					int32_t *col_data = reinterpret_cast<int32_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%d,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_UINT32:
//				{
//					uint32_t *col_data = reinterpret_cast<uint32_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%u,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_INT64:
//				{
//					int64_t *col_data = reinterpret_cast<int64_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//#if defined(__LP64__) || defined(_LP64)
//						dst += sprintf(dst, "%ld,", *col_data);
//#else
//						dst += sprintf(dst, "%lld,", *col_data);
//#endif
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_UINT64:
//				{
//					uint64_t *col_data = reinterpret_cast<uint64_t *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//#if defined(__LP64__) || defined(_LP64)
//						dst += sprintf(dst, "%lu,", *col_data);
//#else
//						dst += sprintf(dst, "%llu,", *col_data);
//#endif
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_FLOAT:
//				{
//					float *col_data = reinterpret_cast<float *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%f,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			case DDE_DATA_DOUBLE:
//				{
//					double *col_data = reinterpret_cast<double *>(src);
//					for (size_t k = 0; k < column->size(); ++k)
//					{
//						dst += sprintf(dst, "%lf,", *col_data);
//						col_data++;
//					}
//				}
//				break;
//
//			//case DDE_DATA_DATE:
//			//	{
//			//		time_t *col_data = reinterpret_cast<time_t *>(src);
//			//		for (size_t k = 0; k < column->size(); ++k)
//			//		{
//			//			dt = *col_data;
//			//			*dst++ = ' ';
//			//			dst += dt.to_date_string(dst);
//			//			col_data++;
//			//		}
//			//	}
//			//	break;
//
//			//case DDE_DATA_TIME:
//			//	{
//			//		time_t *col_data = reinterpret_cast<time_t *>(src);
//			//		for (size_t k = 0; k < column->size(); ++k)
//			//		{
//			//			dt = *col_data;
//			//			*dst++ = ' ';
//			//			dst += dt.to_time_string(dst);
//			//			col_data++;
//			//		}
//			//	}
//			//	break;
//
//			//case DDE_DATA_DATETIME:
//			//	{
//			//		time_t *col_data = reinterpret_cast<time_t *>(src);
//			//		for (size_t k = 0; k < column->size(); ++k)
//			//		{
//			//			dt = *col_data;
//			//			*dst++ = ' ';
//			//			*dst++ = '\'';
//			//			dst += dt.to_string(dst);
//			//			*dst++ = '\'';
//			//			col_data++;
//			//		}
//			//	}
//			//	break;
//
//			default:
//				assert(false);
//				continue;
//			}
//		}
//
//		--dst;
//		*dst++ = ')';
//		count++;
//
//		if (count == 6000)
//		{
//			try
//			{
//				//printf("sql: %s\n", buffer);
//				otl_cursor::direct_exec(m_otl_connect, buffer);
//			}
//			catch(otl_exception& p)
//			{
//				cerr << "msg: "<< p.msg<<endl;
//				cerr << "stm_text: "<<p.stm_text<<endl;
//				cerr << "sqlstate: "<<p.sqlstate<<endl;
//				cerr << "var_info: "<<p.var_info<<endl;
//				delete[] buffer;
//				return false;
//			}
//
//			count = 0;
//		}
//	}
//
//	if (count)
//	{
//		try
//		{
//			//printf("sql: %s\n", buffer);
//			otl_cursor::direct_exec(m_otl_connect, buffer);
//		}
//		catch(otl_exception& p)
//		{
//			cerr << "msg: "<< p.msg<<endl;
//			cerr << "stm_text: "<<p.stm_text<<endl;
//			cerr << "sqlstate: "<<p.sqlstate<<endl;
//			cerr << "var_info: "<<p.var_info<<endl;
//			delete[] buffer;
//			return false;
//		}
//	}
//
//	delete[] buffer;
//	return true;
}

