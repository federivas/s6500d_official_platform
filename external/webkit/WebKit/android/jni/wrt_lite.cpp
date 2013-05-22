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
 * @file		wrt_lite.cpp
 * @brief		This class implements wrtlite interface for BONDI, JIL and other custom objects
 * @author
 * @version		1.0
 * @date		2009.11.02	v1.0		Created
 */
#if ENABLE(WRT)
#include "wrt_lite.h"
#include "wrt_callbackstruct.h"
#include "v8.h"
#include "wrt_v8widget_object.h"
#include "wrt_v8wac_deviceapis.h"

namespace WebCore {
class Frame;
}

using namespace WebCore;
using namespace WTF;

namespace WRTLite {

bool WrtLite::installCustomObjects(v8::Handle<v8::Context> context)
{
	if(!callbackContainer)
		callbackContainer = new HashMap<CallbackStructure*, Frame*>;

	if(!wrtObj)
	{
		//v8::installJilWidget(context,this, m_runtimeID, m_widgetID);
		//v8::installBondiOrg(context,this, m_runtimeID, m_widgetID);
		v8::installWacDeviceApis(context,this, m_runtimeID, m_widgetID);
		v8::installV8WidgetObject(context,this, m_runtimeID, m_widgetID);
	}
	return true;

}

void WrtLite::frameCleared(WebCore::Frame* frame)
{
	if(callbackContainer) {
		HashMap<CallbackStructure*, Frame*>::iterator itrBegin = callbackContainer->begin();
		HashMap<CallbackStructure*, Frame*>::iterator itrEnd = callbackContainer->end();
		for (; itrBegin != itrEnd; ) {
			if(itrBegin->second == frame) {
				CallbackStructure* callBack = itrBegin->first;
				callBack->wrtLite()->getRuntimeID();
				WrtAbortLookUp(callBack->wrtLite()->getRuntimeID()
						,callBack->wrtLite()->getWidgetID(),callBack);
				delete itrBegin->first;
				callBack = NULL;
				itrBegin->first = NULL;
				callbackContainer->remove(itrBegin);
				//After removing a element, iterator becomes inconsistent, hence we need to reset it.
				itrBegin = callbackContainer->begin();
				itrEnd = callbackContainer->end();
				continue;
			}
			++itrBegin;
		}
	}
	if(wrtObj)
		wrtObj = 0;
}

}
#endif