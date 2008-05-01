//////////////////////////////////////////////////////////////////////
//
// SimpleLib Com Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0501
#endif

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>
#include <atlsnap.h>

#include "AtlControls.h"

#include "..\SimpleLib.h"
#include "..\SimpleLibUtils.h"
#include "..\SimpleLibWin.h"
using namespace Simple;
