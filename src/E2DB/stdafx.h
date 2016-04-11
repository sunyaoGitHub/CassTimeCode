// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifndef __linux
#pragma once
#endif

#include "TextSnippet/TextSnippetLib.h"
using namespace dsa::snippets::textsnippet;

#include "DDE/DDELib.h"
using namespace dsa::dde;

#include "rapidxml/dsaxml.hpp"
using namespace rapidxml;

#include "STLHelper/const_char_map.hpp"
using namespace dsa::stlhelper;

#define OTL_ODBC                        //使用ODBC方式
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_BIGINT int64_t
#define OTL_UBIGINT	uint64_t
#include "otl/otlv4.h"

#include <fstream>
using namespace std;

#ifdef __linux
#include <cstring>
#endif

#endif	// __STDAFX_H__

