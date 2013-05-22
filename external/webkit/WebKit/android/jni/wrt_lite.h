/*
 * Copyright (c) S/W Platform, Samsung Electronics, Inc.
 * All rights reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */
/**
 * @file		wrt_lite.h
 * @brief		This class implements wrtlite interface for BONDI, JIL and other custom objects
 * @author
 * @version		1.0
 * @date		2009.11.02	v1.0		Created
 */
#ifndef wrt_lite_h
#define wrt_lite_h
//#if ENABLE(WRT)
#include "config.h"
#include "HashMap.h"
#include "wrt_lite_sys.h"

namespace v8 {
 template<class T> class Handle;
 class Context;
}

namespace WebCore {
class Frame;
}

using namespace WTF;
namespace WRTLite {

class CallbackStructure;

class WrtLite
{

public:
	WrtLite()
	  : callbackContainer(0)
	  , wrtObj(0)
	  , m_runtimeID(-1)
	  , m_widgetID(-1)
	{

        }
	~WrtLite()
	{
	  if(callbackContainer) {
		ASSERT(callbackContainer->isEmpty());
		delete callbackContainer;
	  }
	  WrtDestroyTableEntry(m_runtimeID,m_widgetID);
	}
	bool installCustomObjects(v8::Handle<v8::Context>);
	HashMap<CallbackStructure*, WebCore::Frame*>* callBackContainer() { return callbackContainer; }
	void frameCleared(WebCore::Frame* frame);
	int getRuntimeID() { return m_runtimeID; }
	int getWidgetID() { return m_widgetID; }

	bool setWidgetAndRuntimeID(int rtID, int wgtID) {
	   m_runtimeID = rtID;
	   m_widgetID =  wgtID;
	return true;
	}

	private:
	HashMap<CallbackStructure*, WebCore::Frame*>* callbackContainer;
	void* wrtObj;
	int m_runtimeID;
	int m_widgetID;
};
}
//#endif
#endif
