// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>

#define _ATL_NO_AUTOMATIC_NAMESPACE

// Needed by ThreadSafeQueue.h
#include <atlbase.h>
