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

#include "Vehicle_struct.h"

#include <fstream>
using namespace std;

#ifdef __linux
#include <cstring>
#endif

#endif	// __STDAFX_H__

