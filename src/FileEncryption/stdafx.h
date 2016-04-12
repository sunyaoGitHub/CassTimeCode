
#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifndef __linux
#pragma once
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)   
#endif
#include <QtCore>

#include "DDE/DDELib.h"
using namespace dsa::dde;

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif	// __STDAFX_H__