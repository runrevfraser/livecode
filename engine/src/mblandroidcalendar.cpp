/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#include "prefix.h"

#include "globdefs.h"
#include "filedefs.h"
#include "objdefs.h"
#include "parsedef.h"

#include "mcerror.h"
#include "execpt.h"
#include "printer.h"
#include "globals.h"
#include "dispatch.h"
#include "stack.h"
#include "image.h"
#include "player.h"
#include "param.h"
#include "eventqueue.h"
#include "osspec.h"

#include "date.h"

#include "mbldc.h"

#include "mblandroidutil.h"
#include "mblandroidjava.h"

#include "mblsyntax.h"
#include "mblcalendar.h"

#include <string.h>

#include <jni.h>

typedef enum
{
    kMCAndroidCalendarEventWaiting,
    kMCAndroidCalendarEventDone,
    kMCAndroidCalendarEventCanceled,
} MCAndroidCalendarEventStatus;

static MCAndroidCalendarEventStatus s_calendar_event_status = kMCAndroidCalendarEventWaiting; 
static MCString s_calendar_event_selected = "";
static MCString s_calendar_events_selected = "";
static MCCalendar s_calendar_event_data;

bool MCSystemShowEvent(MCStringRef p_calendar_event_id, MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    MCLog("MCSystemShowCalendarEvent", NULL);
    MCAndroidEngineRemoteCall("showCalendarEvent", "vx", nil, p_calendar_event_id);
    s_calendar_event_status = kMCAndroidCalendarEventWaiting;
    while (s_calendar_event_status == kMCAndroidCalendarEventWaiting)
        MCscreen->wait(60.0, False, True);
    MCStringCreateWithCString(s_calendar_event_selected.getstring(), r_result);
    MCLog("MCSystemShowCalendarEvent - finished", NULL);
    return true;
}

void MCAndroidShowCalendarEventDone(MCStringRef p_calendar_event_id)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidShowCalendarEventDone() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventDone;
}

void MCAndroidShowCalendarEventCanceled(MCStringRef p_calendar_event_id)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidShowCalendarEventCanceled() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventCanceled;
}

bool MCSystemCreateEvent(MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    MCLog("MCSystemCreateCalendarEvent", NULL);
    MCAndroidEngineRemoteCall("createCalendarEvent", "v", nil);
    s_calendar_event_status = kMCAndroidCalendarEventWaiting;
    while (s_calendar_event_status == kMCAndroidCalendarEventWaiting)
        MCscreen->wait(60.0, False, True);
    MCLog("MCSystemCreateCalendarEvent - finished", NULL);
    return true;
}

void MCAndroidCreateCalendarEventDone(MCStringRef p_calendar_event_id)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidCreateCalendarEventDone() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventDone;
}

void MCAndroidCreateCalendarEventCanceled(MCStringRef p_calendar_event_id)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidCreateCalendarEventCanceled() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventCanceled;
}

bool MCSystemUpdateEvent(MCStringRef p_new_calendar_event_data, MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    bool t_result = false;
    MCLog("MCSystemUpdateCalendarEvent", NULL);
    MCAndroidEngineRemoteCall("updateCalendarEvent", "vx", nil, p_new_calendar_event_data);
    s_calendar_event_status = kMCAndroidCalendarEventWaiting;
    while (s_calendar_event_status == kMCAndroidCalendarEventWaiting)
        MCscreen->wait(60.0, False, True);
    
    t_result = MCSCreateWithCString(s_calendar_event_selected.clone(), r_result);
    MCLog("MCSystemUpdateCalendarEvent - finished", NULL);
    return t_result;
}

void MCAndroidUpdateCalendarEventDone(MCStringRef p_calendar_event_id)
{
// STILL TO BE IMPLEMENTED IPA LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidUpdateCalendarEventDone() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventDone;
}

void MCAndroidUpdateCalendarEventCanceled(MCStringRef p_calendar_event_id)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    s_calendar_event_selected = MCStringGetOldString(p_calendar_event_id);
    MCLog("MCAndroidUpdateCalendarEventCanceled() called %s", MCStringGetCString(p_calendar_event_id));
	s_calendar_event_status = kMCAndroidCalendarEventCanceled;
}

bool MCSystemGetEventData(MCExecContext &r_ctxt, MCStringRef p_calendar_event_id, MCArrayRef &r_calendar_event_data)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    MCLog("MCSystemGetEventData: %s", MCStringGetCString(p_calendar_event_id));
    MCAndroidEngineRemoteCall("getCalendarEventData", "vx", nil, p_calendar_event_id);
    MCCalendarToArrayData (r_ctxt, s_calendar_event_data, r_calendar_event_data); 
    return true;
}

bool MCSystemRemoveEvent(MCStringRef p_calendar_event_id, bool p_reoccurring, MCStringRef& r_calendar_event_id_deleted)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    MCLog("MCSystemRemoveCalendarEvent: %s", MCStringGetCString(p_calendar_event_id));
    MCAndroidEngineRemoteCall("removeCalendarEvent", "xx", r_calendar_event_id_deleted, p_calendar_event_id);
    
    return MCStringCreateWithCString(s_calendar_event_selected.clone(), r_calendar_event_id_deleted);
}

bool MCSystemAddEvent(MCCalendar p_new_calendar_event_data, MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    MCLog("MCSystemAddCalendarEvent", NULL);
    MCAndroidEngineRemoteCall("addCalendarEvent", "xxxxxbbbbiixiix", &r_result,
                              p_new_calendar_event_data.mceventid, p_new_calendar_event_data.mctitle,
                              p_new_calendar_event_data.mcnote, p_new_calendar_event_data.mclocation,
                              p_new_calendar_event_data.mcalldayset, p_new_calendar_event_data.mcallday,
                              p_new_calendar_event_data.mcstartdateset,
                              p_new_calendar_event_data.mcenddateset,
                              p_new_calendar_event_data.mcalert1, p_new_calendar_event_data.mcalert2, 
                              p_new_calendar_event_data.mcfrequency, p_new_calendar_event_data.mcfrequencycount,
                              p_new_calendar_event_data.mcfrequencyinterval, p_new_calendar_event_data.mccalendar);    
    return MCStringCreateWithCString(s_calendar_event_selected.clone(), r_result);
}

bool MCSystemFindEvent(MCDateTime p_start_date, MCDateTime p_end_date, MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
    if(MCStringCreateWithCString(s_calendar_events_selected.clone(), r_result));
    {
        MCLog("MCSystemFindCalendarEvent result: %s", r_result);
        return true;
    }
    
    return false;
}

bool MCSystemGetCalendarsEvent(MCStringRef& r_result)
{
	// TODO - IMPLEMENT SUPPORT FOR API LEVEL 14
}

// Get data from Java and assign the values to class values that are then returned to the
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doShowCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doGetCalendarEventData(JNIEnv *env, jobject object,
                                                                                        jstring p_eventid, jstring p_title,
                                                                                        jstring p_note, jstring p_location,
                                                                                        bool p_alldayset, bool p_allday,
                                                                                        bool p_startdateset, jint p_startdate, 
                                                                                        bool p_enddateset, jint p_enddate, 
                                                                                        jint p_alert1, jint p_alert2, 
                                                                                        jstring p_frequency, jint p_frequencycount,
                                                                                        jint p_frequencyinterval, jstring p_calendar) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doUpdateCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doUpdateCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doCreateCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doCreateCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doShowCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doShowCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doAddCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doFindCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_events_found) __attribute__((visibility("default")));
extern "C" JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doRemoveCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_event_id) __attribute__((visibility("default")));

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doGetCalendarEventData(JNIEnv *env, jobject object,
                                                                             jstring p_eventid, jstring p_title,
                                                                             jstring p_note, jstring p_location,
                                                                             bool p_alldayset, bool p_allday,
                                                                             bool p_startdateset, int p_startdate, 
                                                                             bool p_enddateset, int p_enddate,
                                                                             jint p_alert1, jint p_alert2, 
                                                                             jstring p_frequency, jint p_frequencycount,
                                                                             jint p_frequencyinterval, jstring p_calendar)
{
    char *t_temp_string;
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_eventid, t_temp_string);
//    s_calendar_event_data.mceventid = t_temp_string;
    MCValueRelease(s_calendar_event_data.mceventid);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mceventid);
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_title, t_temp_string);
//    s_calendar_event_data.mctitle = t_temp_string;
    MCValueRelease(s_calendar_event_data.mctitle);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mctitle);
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_note, t_temp_string);
//    s_calendar_event_data.mcnote = t_temp_string;
    MCValueRelease(s_calendar_event_data.mcnote);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mcnote);
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_location, t_temp_string);
//    s_calendar_event_data.mclocation = t_temp_string;
    MCValueRelease(s_calendar_event_data.mclocation);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mclocation);
    
    s_calendar_event_data.mcalert1 = p_alert1;
    s_calendar_event_data.mcalert2 = p_alert2;
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_frequency, t_temp_string);
//    s_calendar_event_data.mcfrequency = t_temp_string;
    MCValueRelease(s_calendar_event_data.mcfrequency);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mcfrequency);
    
    s_calendar_event_data.mcfrequencycount = p_frequencycount;
    s_calendar_event_data.mcfrequencyinterval = p_frequencyinterval;
    
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar, t_temp_string);
//    s_calendar_event_data.mccalendar = t_temp_string;
    MCValueRelease(s_calendar_event_data.mccalendar);
    MCStringCreateWithCString(t_temp_string.getstring(), s_calendar_event_data.mccalendar);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doUpdateCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidUpdateCalendarEventDone (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doUpdateCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidUpdateCalendarEventCanceled (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doCreateCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidCreateCalendarEventDone (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doCreateCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidCreateCalendarEventCanceled (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doShowCalendarEventDone(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidShowCalendarEventDone (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doShowCalendarEventCanceled(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    MCAndroidShowCalendarEventCanceled (t_temp_string);
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doAddCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    s_calendar_event_selected = t_temp_string;
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doFindCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_events_found)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_events_found, t_temp_string);
    s_calendar_events_selected = t_temp_string;
}

JNIEXPORT void JNICALL Java_com_runrev_android_Engine_doRemoveCalendarEvent(JNIEnv *env, jobject object, jstring p_calendar_event_id)
{
    char *t_temp_string;
    MCJavaStringToNative(MCJavaGetThreadEnv(), p_calendar_event_id, t_temp_string);
    s_calendar_event_selected = t_temp_string;
}