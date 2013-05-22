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
 * @file		wrt_callbackstruct.h
 * @brief		This class implements the callback structure interface for
 *              BONDI, JIL and other custom objects
 * @author
 * @version		1.0
 * @date		2009.11.02	v1.0		Created
 */

#ifndef CallBackStructure_h
#define CallBackStructure_h
//#if ENABLE(WRT)
#include "v8.h"
using namespace WTF;

namespace WebCore
{
    class Frame;
}

namespace WRTLite
{

class WrtLite;

class CallbackStructure
{
    public:
        CallbackStructure(v8::Handle<v8::Context> context, WebCore::Frame* frame, WrtLite* wrtLite,
            v8::Handle<v8::Value> successFunction = v8::Null(),
            v8::Handle<v8::Value> errorFunction = v8::Null())
        : m_frame(frame)
        , m_wrtLite(wrtLite)
        {
		m_succFunc = v8::Persistent<v8::Value>::New(successFunction);
		m_errFunc = v8::Persistent<v8::Value>::New(errorFunction);
		m_context = v8::Persistent<v8::Context>::New(context);
        }

        ~CallbackStructure()
        {
            m_context.Dispose();
	    m_context.Clear();
            m_succFunc.Dispose();
            m_succFunc.Clear();
            m_errFunc.Dispose();
            m_errFunc.Clear();
        }
        v8::Handle<v8::Value> successCallback() { return m_succFunc; }
        v8::Handle<v8::Value> errorCallback() { return m_errFunc; }
	
        v8::Handle<v8::Context> getCallbackContext() { return m_context; }
        WrtLite* wrtLite() { return m_wrtLite; }
        WebCore::Frame* frame() { return m_frame; }

    private:
        v8::Persistent<v8::Context> m_context;
        WebCore::Frame* m_frame;
        WRTLite::WrtLite* m_wrtLite;
        v8::Persistent<v8::Value> m_succFunc;
        v8::Persistent<v8::Value> m_errFunc;
};
}
//#endif
#endif //CallBackStructure_h
