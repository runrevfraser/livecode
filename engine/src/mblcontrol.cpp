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
//#include "execpt.h"
#include "printer.h"
#include "globals.h"
#include "dispatch.h"
#include "stack.h"
#include "image.h"
#include "player.h"
#include "param.h"
#include "eventqueue.h"
#include "exec.h"

#include "mblcontrol.h"

////////////////////////////////////////////////////////////////////////////////

MCPropertyInfo MCNativeControl::kProperties[] =
{
    DEFINE_RW_CTRL_PROPERTY(P_NAME, OptionalString, MCNativeControl, Name)
    DEFINE_RO_CTRL_PROPERTY(P_ID, UInt32, MCNativeControl, Id)
};

MCObjectPropertyTable MCNativeControl::kPropertyTable =
{
	nil,
	sizeof(kProperties) / sizeof(kProperties[0]),
	&kProperties[0],
};

MCNativeControlActionInfo MCNativeControl::kActions[] =
{
};

MCNativeControlActionTable MCNativeControl::kActionTable =
{
/*
	&MCNativeControl::kActionTable,
	sizeof(kActions) / sizeof(kActions[0]),
	&kActions[0],
 */
    nil,
    0,
    nil,
};

////////////////////////////////////////////////////////////////////////////////

static MCNativeControl *s_native_controls = nil;
static uint32_t s_last_native_control_id = 0;

static MCNativeControl *s_native_control_target = nil;

// MM-2012-02-22: Added module initialisation and clean up so static variables are managed correctly on Android
void MCNativeControlInitialize(void)
{
    s_native_controls = nil;
    s_last_native_control_id = 0;
    s_native_control_target = nil;
}

void MCNativeControlFinalize(void)
{
    MCNativeControl::Finalize();
    s_native_controls = nil;
    s_last_native_control_id = 0;
    s_native_control_target = nil;
}

////////////////////////////////////////////////////////////////////////////////


MCNativeControl::MCNativeControl(void)
{
	m_references = 1;
	m_id = ++s_last_native_control_id;
	m_name = MCValueRetain(kMCEmptyString);
	m_object = nil;
	m_next = nil;
    m_deleted = false;
}

MCNativeControl::~MCNativeControl(void)
{
	if (m_object != nil)
	{
		m_object -> Release();
		m_object = nil;
	}
	
	if (!MCStringIsEmpty(m_name))
	{
		MCValueRelease(m_name);
	}
    
	if (s_native_controls == this)
		s_native_controls = m_next;
	else
		for(MCNativeControl *t_previous = s_native_controls; t_previous != nil; t_previous = t_previous -> m_next)
			if (t_previous -> m_next == this)
			{
				t_previous -> m_next = m_next;
				break;
			}
}

void MCNativeControl::Retain(void)
{
	m_references += 1;
}

void MCNativeControl::Release(void)
{
	m_references -= 1;
	if (m_references == 0)
    {
        // IM 2012-01-27
        // need to move Delete() call here as virtual functions cannot be called from the destructor
        // (nor should they be called from non-virtual functions called by the destructor)
        Delete();
		delete this;
    }
}

uint32_t MCNativeControl::GetId(void)
{
	return m_id;
}

void MCNativeControl::GetName(MCStringRef &r_name)
{
        r_name = MCValueRetain(m_name);
}

MCObject *MCNativeControl::GetOwner(void)
{
	return m_object != nil ? m_object -> Get() : nil;
}

void MCNativeControl::SetOwner(MCObject *p_owner)
{
	if (m_object != nil)
		m_object -> Release();
	m_object = p_owner -> gethandle();
}

bool MCNativeControl::SetName(MCStringRef p_name)
{
	if (!MCStringIsEmpty(m_name))
	{
		MCValueRelease(m_name);
		m_name = MCValueRetain(kMCEmptyString);
	}
	
	if (p_name != nil)
		MCValueAssign(m_name, p_name);
	
	return true;
}

MCNativeControl *MCNativeControl::ChangeTarget(MCNativeControl *p_new_target)
{
	MCNativeControl *t_last_target;
	t_last_target = s_native_control_target;
	s_native_control_target = p_new_target;
	return t_last_target;
}

MCNativeControl *MCNativeControl::CurrentTarget(void)
{
	return s_native_control_target;
}

////////////////////////////////////////////////////////////////////////////////

static struct {const char *name; MCNativeControlType type;} s_native_control_types[] =
{
	{"browser", kMCNativeControlTypeBrowser},
	{"scroller", kMCNativeControlTypeScroller},
	{"player", kMCNativeControlTypePlayer},
	{"input", kMCNativeControlTypeInput},
	{"multiline", kMCNativeControlTypeMultiLineInput},
	{nil, kMCNativeControlTypeUnknown}
};

static struct {const char *name; Properties property;} s_native_control_properties[] =
{
	{"id", P_ID},
	{"name", P_NAME},
	
	{"rect", P_RECTANGLE},
	{"rectangle", P_RECTANGLE},
	{"visible", P_VISIBLE},
	{"opaque", P_OPAQUE},
	{"alpha", P_ALPHA},
	{"backgroundColor", P_BACKGROUND_COLOR},
	
	{"dataDetectorTypes", P_DATA_DETECTOR_TYPES},
	
	{"url", P_URL},
	{"canadvance", P_CAN_ADVANCE},
	{"canretreat", P_CAN_RETREAT},
	{"autofit", P_AUTO_FIT},
	{"delayrequests", P_DELAY_REQUESTS},
	{"allowsInlineMediaPlayback", P_ALLOWS_INLINE_MEDIA_PLAYBACK},
	{"mediaPlaybackRequiresUserAction", P_MEDIA_PLAYBACK_REQUIRES_USER_ACTION},
	
	{"contentrect", P_CONTENT_RECT},
	{"contentrectangle", P_CONTENT_RECT},
	{"canbounce", P_CAN_BOUNCE},
	{"vscroll", P_VSCROLL},
	{"hscroll", P_HSCROLL},
	{"canscrolltotop", P_CAN_SCROLL_TO_TOP},
	{"cancanceltouches", P_CAN_CANCEL_TOUCHES},
	{"delaytouches", P_DELAY_TOUCHES},
	{"decelerationrate", P_DECELERATION_RATE},
	{"indicatorstyle", P_INDICATOR_STYLE},
	{"indicatorinsets", P_INDICATOR_INSETS},
	{"pagingenabled", P_PAGING_ENABLED},
	{"scrollingenabled", P_SCROLLING_ENABLED},
	{"hIndicator", P_SHOW_HORIZONTAL_INDICATOR},
	{"vIndicator", P_SHOW_VERTICAL_INDICATOR},
	{"lockdirection", P_LOCK_DIRECTION},
	{"tracking", P_TRACKING},
	{"dragging", P_DRAGGING},
	{"decelerating", P_DECELERATING},
	
	{"filename", P_CONTENT},
	{"naturalsize", P_NATURAL_SIZE},
	{"fullscreen", P_FULLSCREEN},
	{"preserveaspect", P_PRESERVE_ASPECT},
	{"showcontroller", P_SHOW_CONTROLLER},
	{"duration", P_DURATION},
	{"playableduration", P_PLAYABLE_DURATION},
	{"starttime", P_START_TIME},
	{"endtime", P_END_TIME},
	{"currenttime", P_CURRENT_TIME},
	{"autoplay", P_SHOULD_AUTOPLAY},
	{"looping", P_LOOPING},
	
	{"playbackstate", P_PLAYBACK_STATE},
    
    // MM-2013-02-21: [[ Bug 10632 ]] Added playRate property for native player.
    {"playrate", P_PLAY_RATE},
    
	{"loadstate", P_LOAD_STATE},
	{"useapplicationaudiosession", P_USE_APPLICATION_AUDIO_SESSION},
	{"allowsairplay", P_ALLOWS_AIR_PLAY},
	
	{"enabled", P_ENABLED},
	
	{"text", P_TEXT},
	{"unicodetext", P_UNICODE_TEXT},
	{"textcolor", P_TEXT_COLOR},
	{"textalign", P_TEXT_ALIGN},
	{"fontname", P_FONT_NAME},
	{"fontsize", P_FONT_SIZE},
	{"editing", P_EDITING},
    
	{"minimumfontsize", P_MINIMUM_FONT_SIZE},
	{"autoclear", P_AUTO_CLEAR},
	{"clearbuttonmode", P_CLEAR_BUTTON_MODE},
	{"borderstyle", P_BORDER_STYLE},
	{"verticaltextalign", P_VERTICAL_TEXT_ALIGN},
	
	{"editable", P_EDITABLE},
	{"selectedrange", P_SELECTED_RANGE},
	
	{"autocapitalizationtype", P_AUTO_CAPITALIZATION_TYPE},
	{"autocorrectiontype", P_AUTOCORRECTION_TYPE},
	{"managereturnkey", P_MANAGE_RETURN_KEY},
	{"keyboardtype", P_KEYBOARD_TYPE},
	{"keyboardstyle", P_KEYBOARD_STYLE},
	{"returnkeytype", P_RETURN_KEY_TYPE},
	{"contenttype", P_CONTENT_TYPE},
	
    {"multiline", P_MULTI_LINE},
    
	{nil, P_UNDEFINED}
};

static struct {const char *name; MCNativeControlAction action;} s_native_control_actions[] =
{
	{"advance", kMCNativeControlActionAdvance},
	{"retreat", kMCNativeControlActionRetreat},
	{"reload", kMCNativeControlActionReload},
	{"stop", kMCNativeControlActionStop},
	{"execute", kMCNativeControlActionExecute},
	{"load", kMCNativeControlActionLoad},
	
	{"flashscrollindicators", kMCNativeControlActionFlashScrollIndicators},
	
	{"play", kMCNativeControlActionPlay},
	{"pause", kMCNativeControlActionPause},
	{"prepare", kMCNativeControlActionPrepareToPlay},
	{"begin seeking backward", kMCNativeControlActionBeginSeekingBackward},
	{"begin seeking forward", kMCNativeControlActionBeginSeekingForward},
	{"end seeking", kMCNativeControlActionEndSeeking},
	{"snapshot", kMCNativeControlActionSnapshot},
	{"snapshot exactly", kMCNativeControlActionSnapshotExactly},
	
	{"focus", kMCNativeControlActionFocus},
	
	{"scrollrangetovisible", kMCNativeControlActionScrollRangeToVisible},
	
	{nil, kMCNativeControlActionUnknown}
};

bool MCNativeControl::LookupProperty(MCStringRef p_property, Properties& r_prop)
{
	for(uint32_t i = 0; s_native_control_properties[i] . name != nil; i++)
		if (MCStringIsEqualToCString(p_property, s_native_control_properties[i] . name, kMCCompareCaseless))
		{
			r_prop = s_native_control_properties[i] . property;
			return true;
		}
    
	return false;
}

bool MCNativeControl::LookupAction(MCStringRef p_action, MCNativeControlAction& r_action)
{
	for(uint32_t i = 0; s_native_control_actions[i] . name != nil; i++)
		if (MCStringIsEqualToCString(p_action, s_native_control_actions[i] . name, kMCCompareCaseless))
		{
			r_action = s_native_control_actions[i] . action;
			return true;
		}
	return false;
}

bool MCNativeControl::LookupType(MCStringRef p_type, MCNativeControlType& r_type)
{
	for(uint32_t i = 0; s_native_control_types[i] . name != nil; i++)
		if (MCStringIsEqualToCString(p_type, s_native_control_types[i] . name, kMCCompareCaseless))
		{
			r_type = s_native_control_types[i] . type;
			return true;
		}
	return false;
}

bool MCNativeControl::FindByNameOrId(MCStringRef p_name, MCNativeControl*& r_control)
{
	integer_t t_id;
	if (/* CTXT */ MCStringToInteger(p_name, t_id))
		return FindById(t_id, r_control);
	
	for(MCNativeControl *t_control = s_native_controls; t_control != nil; t_control = t_control -> m_next)
    {
        MCAutoStringRef t_name;
        t_control -> GetName(&t_name);
		if (!t_control -> m_deleted && !MCStringIsEmpty(*t_name) && MCStringIsEqualTo(p_name, *t_name, kMCCompareCaseless))
		{
			r_control = t_control;
			return true;
		}
    }
	
	return false;
}

bool MCNativeControl::FindById(uint32_t p_id, MCNativeControl*& r_control)
{
	for(MCNativeControl *t_control = s_native_controls; t_control != nil; t_control = t_control -> m_next)
		if (!t_control -> m_deleted && t_control -> GetId() == p_id)
		{
			r_control = t_control;
			return true;
		}
	
	return false;
}

void MCNativeControl::Finalize(void)
{
    for(MCNativeControl *t_control = s_native_controls; t_control != nil;)
    {
        MCNativeControl *t_next_control;
        t_next_control = t_control->m_next;
        delete t_control;
        t_control = t_next_control;
    }    
}

////////////////////////////////////////////////////////////////////////////////

bool MCNativeControl::List(MCNativeControlListCallback p_callback, void *p_context)
{
	// MERG-2013-06-10: [[ Bug 10945 ]] Make sure we only list controls that haven't
	//   been deleted.
	for(MCNativeControl *t_control = s_native_controls; t_control != nil; t_control = t_control -> m_next)
		if (!t_control -> m_deleted)
            if (!p_callback(p_context, t_control))
                return false;
	
	return true;
}

bool MCNativeControl::GetControlList(MCStringRef& r_list)
{
    bool t_success;
    t_success = true;
    
    MCAutoListRef t_list;
    MCListCreateMutable('\n', &t_list);
	for(MCNativeControl *t_control = s_native_controls; t_success && t_control != nil; t_control = t_control -> m_next)
    {
        MCAutoStringRef t_name, t_control_string;
        t_control -> GetName(&t_name);
        if (!MCStringIsEmpty(*t_name))
            t_control_string = MCValueRetain(*t_name);
        else
            t_success = MCStringFormat(&t_control_string, "%u");
        
        if (t_success)
            t_success = MCListAppend(*t_list, *t_control_string);
    }
    
    MCListCopyAsString(*t_list, r_list);
}


////////////////////////////////////////////////////////////////////////////////

extern bool MCNativeBrowserControlCreate(MCNativeControl*& r_control);
extern bool MCNativeScrollerControlCreate(MCNativeControl *&r_control);
extern bool MCNativePlayerControlCreate(MCNativeControl *&r_control);
extern bool MCNativeInputControlCreate(MCNativeControl *&r_control);
extern bool MCNativeMultiLineInputControlCreate(MCNativeControl *&r_control);

bool MCNativeControl::CreateWithType(MCNativeControlType p_type, MCNativeControl*& r_control)
{
    bool t_success = true;
    MCNativeControl *t_control = nil;
	switch(p_type)
	{
		case kMCNativeControlTypeBrowser:
			t_success =  MCNativeBrowserControlCreate(t_control);
            break;
		case kMCNativeControlTypeScroller:
			t_success = MCNativeScrollerControlCreate(t_control);
            break;
		case kMCNativeControlTypePlayer:
			t_success = MCNativePlayerControlCreate(t_control);
            break;
		case kMCNativeControlTypeInput:
			t_success = MCNativeInputControlCreate(t_control);
            break;
		case kMCNativeControlTypeMultiLineInput:
			t_success = MCNativeMultiLineInputControlCreate(t_control);
			break;
            
		default:
            t_success = false;
			break;
	}
    
    if (t_success)
        t_success = t_control->Create();
    
    if (t_success)
    {
        t_control->m_next = s_native_controls;
        s_native_controls = t_control;
        
        r_control = t_control;
    }
    else if (t_control != nil)
        t_control->Release();
    
	return t_success;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef LEGACY_EXEC
bool MCExecPointSetRect(MCExecPoint &ep, int2 p_left, int2 p_top, int2 p_right, int2 p_bottom)
{
	char *t_buffer = nil;
	if (!MCCStringFormat(t_buffer, "%d,%d,%d,%d", p_left, p_top, p_right, p_bottom))
		return false;
	
	ep.grabbuffer(t_buffer, MCCStringLength(t_buffer));
	return true;
}

static bool MCParseRGBA(MCStringRef p_data, bool p_require_alpha, uint1 &r_red, uint1 &r_green, uint1 &r_blue, uint1 &r_alpha)
{
	bool t_success = true;
	Boolean t_parsed;
	uint2 r, g, b, a;
   
    MCAutoPointer<char> temp;
    /* UNCHECKED */ MCStringConvertToCString(p_data, &temp);
	const char *t_data = *temp;
	uint32_t l = MCStringGetLength(p_data);
	if (t_success)
	{
		r = MCU_max(0, MCU_min(255, MCU_strtol(t_data, l, ',', t_parsed)));
		t_success = t_parsed;
	}
	if (t_success)
	{
		g = MCU_max(0, MCU_min(255, MCU_strtol(t_data, l, ',', t_parsed)));
		t_success = t_parsed;
	}
	if (t_success)
	{
		b = MCU_max(0, MCU_min(255, MCU_strtol(t_data, l, ',', t_parsed)));
		t_success = t_parsed;
	}
	if (t_success)
	{
		a = MCU_max(0, MCU_min(255, MCU_strtol(t_data, l, ',', t_parsed)));
		if (!t_parsed)
		{
			if (p_require_alpha)
				t_success = false;
			else
				a = 255;
		}
	}
	
	if (t_success)
	{
		r_red = r;
		r_green = g;
		r_blue = b;
		r_alpha = a;
	}
	return t_success;
}

bool MCNativeControl::ParseColor(MCExecPoint &ep, uint16_t &r_red, uint16_t &r_green, uint16_t &r_blue, uint16_t &r_alpha)
{
    uint8_t t_r8, t_g8, t_b8, t_a8;
    MCColor t_color;

    char *t_name = nil;
    MCAutoStringRef t_value;
    ep . copyasstringref(&t_value);
    if (MCParseRGBA(*t_value, false, t_r8, t_g8, t_b8, t_a8))
    {
        r_red = (t_r8 << 8) | t_r8;
        r_green = (t_g8 << 8) | t_g8;
        r_blue = (t_b8 << 8) | t_b8;
        r_alpha = (t_a8 << 8) | t_a8;
        return true;
    }
    else if (MCscreen->parsecolor(*t_value, t_color, NULL))
    {
        r_red = t_color.red;
        r_green = t_color.green;
        r_blue = t_color.blue;
        r_alpha = 0xFFFF;
        return true;
    }
    else
        return false;
}

bool MCNativeControl::FormatColor(MCExecPoint& ep, uint16_t p_red, uint16_t p_green, uint16_t p_blue, uint16_t p_alpha)
{
    char *t_colorstring = nil;

    p_red >>= 8;
    p_green >>= 8;
    p_blue >>= 8;
    p_alpha >>= 8;
    
    if (p_alpha != 255)
        MCCStringFormat(t_colorstring, "%u,%u,%u,%u", p_red, p_green, p_blue, p_alpha);
    else
        MCCStringFormat(t_colorstring, "%u,%u,%u", p_red, p_green, p_blue);
    ep.grabbuffer(t_colorstring, MCCStringLength(t_colorstring));
	
	return true;
}

bool MCNativeControl::ParseBoolean(MCExecPoint& ep, bool& r_value)
{
	Boolean t_bool;
	if (!MCU_stob(ep.getsvalue(), t_bool))
	{
		MCeerror->add(EE_OBJECT_NAB, 0, 0, ep.getsvalue());
		return false;
	}
	r_value = t_bool == True;
	return true;
}

bool MCNativeControl::FormatBoolean(MCExecPoint& ep, bool p_value)
{
	ep . setsvalue(MCU_btos(p_value));
	return true;
}

bool MCNativeControl::ParseInteger(MCExecPoint& ep, int32_t& r_value)
{
	if (!MCU_stoi4(ep . getsvalue(), r_value))
	{
		MCeerror->add(EE_OBJECT_NAN, 0, 0, ep.getsvalue());
		return false;
	}
	return true;
}

bool MCNativeControl::FormatInteger(MCExecPoint& ep, int32_t p_value)
{
	ep . setnvalue(p_value);
	return true;
}

bool MCNativeControl::ParseUnsignedInteger(MCExecPoint& ep, uint32_t& r_value)
{
	if (!MCU_stoui4(ep . getsvalue(), r_value))
	{
		MCeerror->add(EE_OBJECT_NAN, 0, 0, ep.getsvalue());
		return false;
	}
	return true;
}

bool MCNativeControl::ParseReal(MCExecPoint& ep, double& r_value)
{
	if (!MCU_stor8(ep . getsvalue(), r_value))
	{
		MCeerror->add(EE_OBJECT_NAN, 0, 0, ep.getsvalue());
		return false;
	}
	return true;
}

bool MCNativeControl::FormatReal(MCExecPoint& ep, double p_value)
{
	ep . setnvalue(p_value);
	return true;
}

bool MCNativeControl::ParseEnum(MCExecPoint& ep, MCNativeControlEnumEntry *p_entries, int32_t& r_value)
{
	for(uint32_t i = 0; p_entries[i] . key != nil; i++)
		if (MCCStringEqualCaseless(p_entries[i] . key, ep . getcstring()))
		{
			r_value = p_entries[i] . value;
			return true;
		}
	
	MCeerror->add(EE_OBJECT_BADSTYLE, 0, 0, ep.getsvalue());
	return false;
}

bool MCNativeControl::FormatEnum(MCExecPoint& ep, MCNativeControlEnumEntry *p_entries, int32_t p_value)
{
	for(uint32_t i = 0; p_entries[i] . key != nil; i++)
		if (p_entries[i] . value == p_value)
		{
			ep . setsvalue(p_entries[i] . key);
			return true;
		}
	
	ep . clear();
	return true;
}

bool MCNativeControl::ParseSet(MCExecPoint& ep, MCNativeControlEnumEntry *p_entries, int32_t& r_value)
{
    bool t_success = true;
    
	char **t_members_array;
	uint32_t t_members_count;
	t_members_array = nil;
	t_members_count = 0;
	if (t_success)
		t_success = MCCStringSplit(ep.getcstring(), ',', t_members_array, t_members_count);
	
	int32_t t_members_set;
	t_members_set = 0;
	if (t_success)
		for(uint32_t i = 0; t_success && i < t_members_count; i++)
        {
            bool t_found = false;
			for(uint32_t j = 0; !t_found && p_entries[j].key != nil; j++)
            {
				if (MCCStringEqualCaseless(t_members_array[i], p_entries[j].key))
                {
					t_members_set |= p_entries[j].value;
                    t_found = true;
                }
            }
            if (!t_found)
                t_success = false;
        }
	
	for(uint32_t i = 0; i < t_members_count; i++)
		MCCStringFree(t_members_array[i]);
	MCMemoryDeleteArray(t_members_array);
    
    if (t_success)
        r_value = t_members_set;
    
	return t_success;
}

bool MCNativeControl::FormatSet(MCExecPoint& ep, MCNativeControlEnumEntry *p_entries, int32_t p_value)
{
	bool t_first;
	t_first = true;
	
	ep . clear();
	
	for(uint32_t i = 0; p_entries[i] . key != nil; i++)
		if ((p_value & p_entries[i] . value) != 0)
		{
			ep . concatcstring(p_entries[i] . key, EC_COMMA, t_first);
			t_first = false;
		}
	
	return true;
}

bool MCNativeControl::ParseRectangle(MCExecPoint& ep, MCRectangle& r_rect)
{
	int16_t t_left, t_top, t_right, t_bottom;
	if (!MCU_stoi2x4(ep . getsvalue(), t_left, t_top, t_right, t_bottom))
	{
		MCeerror->add(EE_OBJECT_NAR, 0, 0, ep.getsvalue());
		return false;
	}
	
	MCU_set_rect(r_rect, t_left, t_top, t_right - t_left, t_bottom - t_top);
	return true;
}

bool MCNativeControl::ParseRectangle32(MCExecPoint& ep, MCRectangle32& r_rect)
{
	int32_t t_left, t_top, t_right, t_bottom;
	if (!MCU_stoi4x4(ep . getsvalue(), t_left, t_top, t_right, t_bottom))
	{
		MCeerror->add(EE_OBJECT_NAR, 0, 0, ep.getsvalue());
		return false;
	}
	
	MCU_set_rect(r_rect, t_left, t_top, t_right - t_left, t_bottom - t_top);
	return true;
}

bool MCNativeControl::ParseRange(MCExecPoint &ep, uint32_t &r_start, uint32_t &r_length)
{
	const char *sptr = ep.getsvalue().getstring();
	uint4 l = ep.getsvalue().getlength();
	uint32_t d1, d2;
	Boolean done;
	d1 = MCU_strtol(sptr, l, ',', done, True, False);
	if (!done || l == 0)
		return false;
	d2 = MCU_strtol(sptr, l, '\0', done, True, False);
	if (!done || l != 0)
		return false;
    
    r_start = d1;
    r_length = d2;
    
    return true;
}

bool MCNativeControl::FormatRange(MCExecPoint &ep, uint32_t p_start, uint32_t p_length)
{
    ep.setnvalue(p_start);
	ep.concatint(p_length, EC_COMMA, false);
    
    return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////

void MCNativeControl::GetId(MCExecContext& ctxt, uinteger_t& r_id)
{
    r_id = m_id;
}

void MCNativeControl::GetName(MCExecContext& ctxt, MCStringRef& r_name)
{
    if (!MCStringIsEmpty(m_name))
        r_name = MCValueRetain(m_name);
      
    return;  
}

void MCNativeControl::SetName(MCExecContext& ctxt, MCStringRef p_name)
{
    if (!MCStringIsEmpty(m_name))
	{
		MCValueRelease(m_name);
		m_name = MCValueRetain(kMCEmptyString);
	}
	
	if (p_name != nil)
		m_name = MCValueRetain(p_name);

    return;
}

////////////////////////////////////////////////////////////////////////////////

// MM-2013-11-26: [[ Bug 11485 ]] Added functions for converting between user and device space.

MCGRectangle MCNativeControlUserRectToDeviceRect(const MCGRectangle &p_user_rect)
{
    MCGAffineTransform t_transform;
    t_transform = MCNativeControlUserToDeviceTransform();
    return MCGRectangleApplyAffineTransform(p_user_rect, t_transform);
}

MCGRectangle MCNativeControlUserRectFromDeviceRect(const MCGRectangle &p_device_rect)
{
    MCGAffineTransform t_transform;
    t_transform = MCNativeControlUserFromDeviceTransform();
    return MCGRectangleApplyAffineTransform(p_device_rect, t_transform);
}

MCGPoint MCNativeControlUserPointToDevicePoint(const MCGPoint &p_user_point)
{
    MCGAffineTransform t_transform;
    t_transform = MCNativeControlUserToDeviceTransform();
    return MCGPointApplyAffineTransform(p_user_point, t_transform);
}

MCGPoint MCNativeControlUserPointFromDevicePoint(const MCGPoint &p_device_point)
{
    MCGAffineTransform t_transform;
    t_transform = MCNativeControlUserFromDeviceTransform();
    return MCGPointApplyAffineTransform(p_device_point, t_transform);
}

int32_t MCNativeControlUserXLocToDeviceXLoc(int32_t p_user_x_loc)
{
    MCGPoint t_loc;
    t_loc = MCNativeControlUserPointToDevicePoint(MCGPointMake((MCGFloat) p_user_x_loc, 0.0f));
    return (int32_t) t_loc . x;
}

int32_t MCNativeControlUserXLocFromDeviceXLoc(int32_t p_device_x_loc)
{
    MCGPoint t_loc;
    t_loc = MCNativeControlUserPointFromDevicePoint(MCGPointMake((MCGFloat) p_device_x_loc, 0.0f));
    return (int32_t) t_loc . x;
}

int32_t MCNativeControlUserYLocToDeviceYLoc(int32_t p_user_y_loc)
{
    MCGPoint t_loc;
    t_loc = MCNativeControlUserPointToDevicePoint(MCGPointMake(0.0f, (MCGFloat) p_user_y_loc));
    return (int32_t) t_loc . y;
}

int32_t MCNativeControlUserYLocFromDeviceYLoc(int32_t p_device_y_loc)
{
    MCGPoint t_loc;
    t_loc = MCNativeControlUserPointFromDevicePoint(MCGPointMake(0.0f, (MCGFloat) p_device_y_loc));
    return (int32_t) t_loc . y;
}

////////////////////////////////////////////////////////////////////////////////
