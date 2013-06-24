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

#ifndef	COMMANDS_H
#define	COMMANDS_H

#include "statemnt.h"
#include "objdefs.h"
#include "util.h"
#include "uidc.h"

// general commands in cmds.cc

class MCChoose : public MCStatement
{
	Tool littool;
	MCExpression *etool;
public:
	MCChoose()
	{
		littool = T_UNDEFINED;
		etool = NULL;
	}
	virtual ~MCChoose();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCConvert : public MCStatement
{
	MCChunk *container;
	MCExpression *source;
	MCVarref *it;
	Convert_form fform;
	Convert_form fsform;
	Convert_form pform;
	Convert_form sform;
public:
	MCConvert()
	{
		container = NULL;
		source = NULL;
		it = NULL;
		fform = CF_UNDEFINED;
		fsform = CF_UNDEFINED;
		pform = CF_UNDEFINED;
		sform = CF_UNDEFINED;
	}
	virtual ~MCConvert();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
	Parse_stat parsedtformat(MCScriptPoint &sp, Convert_form &firstform,
	                         Convert_form &secondform);
};

class MCDo : public MCStatement
{
	MCHandler *h;
	MCExpression *source;
	MCExpression *alternatelang;
protected:
	bool browser : 1;
	Boolean debug : 1;
	bool caller : 1;
public:
	MCDo()
	{
		source = NULL;
		alternatelang = NULL;
		browser = false;
		debug = False;
		caller = false;
	}
	virtual ~MCDo();
	virtual Parse_stat parse(MCScriptPoint &);
	void deletestatements(MCStatement *statements);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCDebugDo : public MCDo
{
public:
	MCDebugDo()
	{
		debug = True;
	}
};

class MCRegularDo : public MCDo
{
public:
	MCRegularDo()
	{
		debug = False;
	}
};

class MCDoMenu : public MCStatement
{
	MCExpression *source;
public:
	MCDoMenu()
	{
		source = NULL;
	}
	const char *lookup(const MCString &s);
	virtual ~MCDoMenu();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCEdit : public MCStatement
{
	MCChunk *target;
public:
	MCEdit()
	{
		target = NULL;
	}
	virtual ~MCEdit();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCFind : public MCStatement
{
	Find_mode mode;
	MCExpression *tofind;
	MCChunk *field;
public:
	MCFind()
	{
		mode = FM_NORMAL;
		tofind = NULL;
		field = NULL;
	}
	virtual ~MCFind();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCGet : public MCStatement
{
	MCExpression *value;
	MCVarref *it;
public:
	MCGet()
	{
		value = NULL;
		it = NULL;
	}
	virtual ~MCGet();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCMarking : public MCStatement
{
	MCExpression *where;
	MCExpression *tofind;
	MCChunk *field;
	MCChunk *card;
	Find_mode mode;
protected:
	Boolean mark;
public:
	MCMarking()
	{
		where = tofind = NULL;
		mode = FM_NORMAL;
		field = card = NULL;
	}
	virtual ~MCMarking();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
};

class MCMarkCommand : public MCMarking
{
public:
	MCMarkCommand()
	{
		mark = True;
	}
};

class MCUnmark : public MCMarking
{
public:
	MCUnmark()
	{
		mark = False;
	}
};

class MCPost : public MCStatement
{
	MCExpression *source;
	MCExpression *dest;
	MCVarref *it;
public:
	MCPost()
	{
		source = dest = NULL;
		it = NULL;
	}
	virtual ~MCPost();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPut : public MCStatement
{
	MCExpression *source;
	MCChunk *dest;
	Preposition_type prep : 8;
	// MW-2012-02-23: [[ UnicodePut ]] Indicates if the 'unicode' adjective
	//   was present.
	bool is_unicode : 1;
	bool overlap : 1;

	//cookie
	MCExpression *name;
	MCExpression *path;
	MCExpression *domain;
	MCExpression *expires;
	bool is_secure;
	bool is_httponly;
public:
	MCPut()
	{
		source = NULL;
		// MW-2011-06-22: [[ SERVER ]] Make a distinction between 'put' and 'put .. into msg'
		prep = PT_UNDEFINED;
		dest = NULL;
		overlap = false;
		// cookie
		name = NULL;
		path = NULL;
		domain = NULL;
		expires = NULL;
		is_secure = false;
		is_httponly = false;
		
		// MW-2012-02-23: [[ UnicodePut ]] Assume non-unicode to begin with.
		is_unicode = false;
	}
	virtual ~MCPut();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
	
	Exec_stat exec_cookie(MCExecPoint &);
};

class MCQuit : public MCStatement
{
	MCExpression *retcode;
public:
	MCQuit()
	{
		retcode = NULL;
	}
	virtual ~MCQuit();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCReset : public MCStatement
{
	Reset_type which;
public:
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCReturn : public MCStatement
{
	MCExpression *source;
	MCExpression *url;
	MCVarref *var;
public:
	MCReturn()
	{
		source = url = NULL;
		var = NULL;
	}
	virtual ~MCReturn();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual uint4 linecount();
	virtual void compile(MCSyntaxFactoryRef);
};

// relayer <control> ( before | after ) layer <expr>
// relayer <control> ( before | after ) <control>
// relayer <control> ( before | after ) owner
// relayer <control> to ( front | back ) of layer <expr>
// relayer <control> to ( front | back ) of <control>
// relayer <control> to ( front | back ) of owner
enum MCRelayerForm
{
	kMCRelayerFormNone,
	kMCRelayerFormRelativeToLayer,
	kMCRelayerFormRelativeToControl,
	kMCRelayerFormRelativeToOwner,
};

enum MCRelayerRelation
{
	kMCRelayerRelationNone,
	kMCRelayerRelationBefore,
	kMCRelayerRelationAfter,
	kMCRelayerRelationFront,
	kMCRelayerRelationBack
};

class MCRelayer : public MCStatement
{
	MCRelayerForm form : 3;
	MCRelayerRelation relation : 4;
	MCChunk *control;
	union
	{
		MCExpression *layer;
		MCChunk *target;
	};

public:
	MCRelayer(void);
	virtual ~MCRelayer(void);

	virtual Parse_stat parse(MCScriptPoint& sp);
	virtual Exec_stat exec(MCExecPoint& ep);
};

class MCScriptError : public MCStatement
{
public:
	virtual Parse_stat parse(MCScriptPoint &);
};

class MCSet : public MCStatement
{
	MCProperty *target;
	MCExpression *value;
public:
	MCSet()
	{
		target = NULL;
		value = NULL;
	}
	virtual ~MCSet();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
};

class MCSort : public MCStatement
{
	MCChunk *of;
	Chunk_term chunktype;
	Sort_type direction;
	Sort_type format;
	MCExpression *by;
public:
	MCSort()
	{
		of = NULL;
		chunktype = CT_FIELD;
		direction = ST_ASCENDING;
		format = ST_TEXT;
		by = NULL;
	}
	virtual ~MCSort();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	static Exec_stat sort_container(MCExecPoint &p_exec_point, Chunk_term p_type, Sort_type p_direction, Sort_type p_form, MCExpression *p_by);
	static void additem(MCExecPoint &ep, MCSortnode *&items, uint4 &nitems, Sort_type form, MCString &s, MCExpression *by);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCWait : public MCStatement
{
	Repeat_form condition;
	MCExpression *duration;
	Functions units;
	Boolean messages;
public:
	MCWait()
	{
		duration = NULL;
		messages = False;
	}
	virtual ~MCWait();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// creation related commands in cmdsc.cc

class MCClone : public MCStatement
{
	MCChunk *source;
	MCExpression *newname;
	MCVarref *it;
	Boolean visible;
public:
	MCClone()
	{
		source = NULL;
		newname = NULL;
		it = NULL;
		visible = True;
	}
	virtual ~MCClone();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCClipboardCmd: public MCStatement
{
	MCChunk *targets;
	MCChunk *dest;
	MCVarref *it;

public:
	MCClipboardCmd(void)
	{
		targets = NULL;
		dest = NULL;
		it = NULL;
	}

	virtual ~MCClipboardCmd(void);

	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);

protected:
	virtual bool iscut(void) const = 0;

private:
	Exec_errors processtocontainer(MCObjectRef *p_objects, uint4 p_object_count, MCObject *p_dst);
	Exec_errors processtoclipboard(MCObjectRef *p_objects, uint4 p_object_count);
};

class MCCopyCmd: public MCClipboardCmd
{
protected:
	virtual bool iscut(void) const;
};

class MCCutCmd: public MCClipboardCmd
{
protected:
	virtual bool iscut(void) const;
};

// MW-2004-11-26: Initialise otype (VG)
class MCCreate : public MCStatement
{
	Chunk_term otype;
	MCExpression *newname;
	MCExpression *file;
	MCChunk *container;
	MCVarref *it;
	Boolean directory;
	Boolean visible;
	Boolean alias;
public:
	MCCreate()
	{
		otype = CT_UNDEFINED;
		newname = NULL;
		file = NULL;
		container = NULL;
		it = NULL;
		directory = False;
		alias = False;
		visible = True;
	}
	virtual ~MCCreate();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	MCControl *getobject(MCObject *&parent);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCCustomProp : public MCStatement
{
	MCExpression *prop;
	MCChunk *target;
protected:
	Boolean define;
public:
	MCCustomProp()
	{
		prop = NULL;
		target = NULL;
	}
	virtual ~MCCustomProp();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
};

class MCDefine : public MCCustomProp
{
public:
	MCDefine()
	{
		define = True;
	}
};

class MCUndefine : public MCCustomProp
{
public:
	MCUndefine()
	{
		define = False;
	}
};

class MCDelete : public MCStatement
{
	MCExpression *file;
	MCChunk *targets;
	Boolean directory;
	Boolean url;
	MCVarref *var;
	bool session;
public:
	MCDelete()
	{
		file = NULL;
		targets = NULL;
		directory = url = False;
		var = NULL;
		session = false;
	}
	virtual ~MCDelete();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCChangeProp : public MCStatement
{
protected:
	MCChunk *targets;
	Properties prop;
	Boolean value;
public:
	MCChangeProp()
	{
		targets = NULL;
	}
	virtual ~MCChangeProp();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCDisable : public MCChangeProp
{
public:
	MCDisable()
	{
		prop = P_DISABLED;
		value = True;
	}
};

class MCEnable : public MCChangeProp
{
public:
	MCEnable()
	{
		prop = P_DISABLED;
		value = False;
	}
};

class MCHilite : public MCChangeProp
{
public:
	MCHilite()
	{
		prop = P_HILITE;
		value = True;
	}
};

class MCUnhilite : public MCChangeProp
{
public:
	MCUnhilite()
	{
		prop = P_HILITE;
		value = False;
	}
};

class MCCrop : public MCStatement
{
	MCExpression *newrect;
	MCChunk *image;
public:
	MCCrop()
	{
		image = NULL;
		newrect = NULL;
	}
	virtual ~MCCrop();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCFlip : public MCStatement
{
	Flip_dir direction;
	MCChunk *image;
public:
	MCFlip()
	{
		direction = FL_UNDEFINED;
		image = NULL;
	}
	virtual ~MCFlip();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCGrab : public MCStatement
{
	MCChunk *control;
public:
	MCGrab()
	{
		control = NULL;
	}
	virtual ~MCGrab();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCLaunch : public MCStatement
{
	MCExpression *doc;
	MCExpression *app;
	bool as_url;

public:
	MCLaunch()
	{
		doc = app = NULL;
		as_url = false;
	}
	virtual ~MCLaunch();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCLoad : public MCStatement
{
	MCExpression *url;
	MCExpression *message;
public:
	MCLoad()
	{
		url = message = NULL;
	}
	virtual ~MCLoad();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCUnload : public MCStatement
{
	MCExpression *url;
public:
	MCUnload()
	{
		url = NULL;
	}
	virtual ~MCUnload();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCMakeGroup : public MCStatement
{
	MCChunk *targets;
public:
	MCMakeGroup()
	{
		targets = NULL;
	}
	virtual ~MCMakeGroup();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPasteCmd : public MCStatement
{
	MCVarref *it;
public:
	MCPasteCmd()
	{
		it = NULL;
	}
	virtual ~MCPasteCmd();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPlace : public MCStatement
{
	MCChunk *group;
	MCChunk *card;
public:
	MCPlace()
	{
		group = NULL;
		card = NULL;
	}
	virtual ~MCPlace();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRecord : public MCStatement
{
	MCExpression *file;
public:
	MCRecord()
	{
		file = NULL;
	}
	virtual ~MCRecord();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRedo : public MCStatement
{
public:
	virtual Exec_stat exec(MCExecPoint &);
};

class MCRemove : public MCStatement
{
	MCChunk *target;
	MCChunk *card;
	Boolean script;
	Boolean all;
	Insert_point where;
public:
	MCRemove()
	{
		target = NULL;
		card = NULL;
		script = all = False;
	}
	virtual ~MCRemove();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRename : public MCStatement
{
	MCExpression *source;
	MCExpression *dest;
public:
	MCRename()
	{
		source = dest = NULL;
	}
	virtual ~MCRename();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCReplace : public MCStatement
{
	MCExpression *pattern;
	MCExpression *replacement;
	MCChunk *container;
public:
	MCReplace()
	{
		pattern = replacement = NULL;
		container = NULL;
	}
	virtual ~MCReplace();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRevert : public MCStatement
{
public:
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRotate : public MCStatement
{
	MCExpression *angle;
	MCChunk *image;
public:
	MCRotate()
	{
		image = NULL;
		angle = NULL;
	}
	virtual ~MCRotate();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCSelect : public MCStatement
{
	Preposition_type where;
	Boolean text;
	MCChunk *targets;
public:
	MCSelect()
	{
		where = PT_AT;
		text = False;
		targets = NULL;
	}
	virtual ~MCSelect();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCUngroup : public MCStatement
{
	MCChunk *group;
public:
	MCUngroup()
	{
		group = NULL;
	}
	virtual ~MCUngroup();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCUndoCmd : public MCStatement
{
public:
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// event related comands in cmdse.cc

class MCAccept : public MCStatement
{
	MCExpression *port;
	MCExpression *message;
	Boolean datagram;
	Boolean secure;
	Boolean secureverify;
	MCExpression *certificate;
public:
	MCAccept()
	{
		port = message = NULL;
		datagram = False;
		secure = False;
		certificate = NULL;
		secureverify = False;
	}
	virtual ~MCAccept();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCBeep : public MCStatement
{
	MCExpression *times;
public:
	MCBeep()
	{
		times = NULL;
	}
	virtual ~MCBeep();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCBreakPoint : public MCStatement
{
public:
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCCancel : public MCStatement
{
	MCExpression *id;
public:
	MCCancel()
	{
		id = NULL;
	}
	virtual ~MCCancel();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCClickCmd : public MCStatement
{
	MCExpression *button;
	MCExpression *location;
	uint2 mstate;
	uint2 which;
public:
	MCClickCmd()
	{
		button = location = NULL;
		which = 1;
		mstate = 0;
	}
	virtual ~MCClickCmd();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCDrag : public MCStatement
{
	MCExpression *button;
	MCExpression *startloc;
	MCExpression *endloc;
	MCExpression *duration;
	uint2 mstate;
	uint2 which;
public:
	MCDrag()
	{
		button = startloc = endloc = duration = NULL;
		which = 1;
		mstate = 0;
	}
	virtual ~MCDrag();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// MW-2008-11-05: [[ Dispatch Command ]] The statement class for the 'dispatch' command.
//   'dispatch' [ command | function ] <message> [ 'to' <target> ] [ 'with' <arguments> ]
class MCDispatchCmd: public MCStatement
{
	MCExpression *message;
	MCChunk *target;
	MCParameter *params;
	MCVarref *it;
	bool is_function;

public:
	MCDispatchCmd(void)
	{
		message = NULL;
		target = NULL;
		params = NULL;
		it = NULL;
		is_function = false;
	}
	~MCDispatchCmd(void);
	
	virtual Parse_stat parse(MCScriptPoint& sp);
	virtual Exec_stat exec(MCExecPoint& ep);
};

class MCFocus : public MCStatement
{
	MCChunk *object;
public:
	MCFocus()
	{
		object = NULL;
	}
	virtual ~MCFocus();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCInsert : public MCStatement
{
	MCChunk *target;
	Insert_point where;
public:
	MCInsert()
	{
		target = NULL;
	}
	virtual ~MCInsert();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCMessage : public MCStatement
{
	MCHandler *h;
	MCExpression *message;
	MCExpression *eventtype;
	MCChunk *target;
	MCExpression *in;
	Functions units;
	Boolean program;
	Boolean reply;
protected:
	Boolean send;
public:
	MCMessage()
	{
		message = eventtype = NULL;
		target = NULL;
		in = NULL;
		program = False;
		reply = True;
	}
	virtual ~MCMessage();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCCall : public MCMessage
{
public:
	MCCall()
	{
		send = False;
	}
};

class MCSend : public MCMessage
{
public:
	MCSend()
	{
		send = True;
	}
};

class MCMove : public MCStatement
{
	MCChunk *object;
	MCExpression *startloc;
	MCExpression *endloc;
	MCExpression *durationexp;
	Functions units;
	Boolean relative;
	Boolean messages;
	Boolean waiting;
public:
	MCMove()
	{
		object = NULL;
		startloc = endloc = durationexp = NULL;
		relative = False;
		messages = waiting = True;
	}
	virtual ~MCMove();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCMM : public MCStatement
{
	MCExpression *clip;
	MCExpression *tempo;
	MCExpression *loc;
	MCExpression *options;
	MCChunk *stack;
	Boolean audio;
	Boolean video;
	Boolean player;
	Boolean pause;
	Boolean resume;
	Boolean stepforward;
	Boolean stepback;
	Boolean looping;
	Boolean stop;
	Boolean image;
	Boolean image_file;
	Chunk_term ptype;
	Chunk_term etype;
protected:
	Boolean prepare;
public:
	MCMM()
	{
		clip = tempo = loc = options = NULL;
		stack = NULL;
		stop = looping = audio = video = player = pause = resume
		                                 = stepforward = stepback = image = image_file = False;
		ptype = CT_CARD;
		etype = CT_EXPRESSION;
	}
	virtual ~MCMM();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPlay : public MCMM
{
public:
	MCPlay()
	{
		prepare = False;
	}
};

class MCPrepare : public MCMM
{
public:
	MCPrepare()
	{
		prepare = True;
	}
};

class MCReply : public MCStatement
{
	MCExpression *message;
	MCExpression *keyword;
	Boolean error;
public:
	MCReply()
	{
		message = keyword = NULL;
		error = False;
	}
	virtual ~MCReply();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRequest : public MCStatement
{
	MCExpression *message;
	MCExpression *program;
	MCVarref *it;
	Apple_event ae;
public:
	MCRequest()
	{
		message = program = NULL;
		it = NULL;
		ae = AE_UNDEFINED;
	}
	virtual ~MCRequest();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCStart : public MCStatement
{
	MCChunk *target;
	MCExpression *stack;
protected:
	Start_constants mode;
public:
	MCStart()
	{
		target = NULL;
		stack = NULL;
		mode = SC_UNDEFINED;
	}
	virtual ~MCStart();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCLibrary : public MCStart
{
public:
	MCLibrary()
	{
		mode = SC_USING;
	}
};

class MCStop : public MCStatement
{
	MCChunk *target;
	MCExpression *stack;
	Start_constants mode;
public:
	MCStop()
	{
		target = NULL;
		stack = NULL;
	}
	virtual ~MCStop();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCType : public MCStatement
{
	MCExpression *message;
	MCExpression *duration;
	uint2 mstate;
public:
	MCType()
	{
		message = duration = NULL;
		mstate = 0;
	}
	virtual ~MCType();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// file related comands in cmdsf.cc

class MCClose : public MCStatement
{
	Open_argument arg;
	MCExpression *fname;
	MCChunk *stack;
	Boolean immediate;
public:
	MCClose()
	{
		fname = NULL;
		stack = NULL;
		arg = OA_OBJECT;
	}
	virtual ~MCClose();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// MW-2004-11-26: Initialise format and sformat (VG)
class MCExport : public MCStatement
{
	Export_format format;
	Export_format sformat;
	MCExpression *exsrect;
	MCExpression *exsstack;
	MCExpression *exsdisplay;
	MCExpression *fname;
	MCExpression *mname;
	MCImagePaletteType palette_type;
	MCExpression *palette_color_list;
	MCExpression *palette_color_count;
	MCChunk *image;
	MCChunk *dest;
	MCExpression *size;
	bool with_effects : 1;
public:
	MCExport()
	{
		format = EX_UNDEFINED;
		sformat = EX_UNDEFINED;
		fname = NULL;
		mname = NULL;
		image = NULL;
		dest = NULL;
		exsstack = NULL;
		exsrect = NULL;
		exsdisplay = NULL;
		palette_type = kMCImagePaletteTypeEmpty;
		palette_color_list = NULL;
		palette_color_count = NULL;
		with_effects = false;
		size = NULL;
	}
	virtual ~MCExport();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCEncryptionOp : public MCStatement
{
	MCExpression *ciphername;
	MCExpression *source;
	MCExpression *keystr;
	MCExpression *keylen;
	MCExpression *salt;
	MCExpression *iv;
	Boolean ispassword;
	MCVarref *it;

	bool is_rsa;
	RSA_KEYTYPE rsa_keytype;
	MCExpression *rsa_key;
	MCExpression *rsa_passphrase;
protected:
	Boolean isdecrypt;
public:
	MCEncryptionOp()
	{
		source = ciphername = keystr = keylen = NULL;
		it = NULL;
		salt = NULL;
		iv = NULL;
		rsa_key = rsa_passphrase = NULL;
	}
	virtual ~MCEncryptionOp();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
//	virtual Exec_stat exec_rsa(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCCipherEncrypt : public MCEncryptionOp
{
public:
	MCCipherEncrypt()
	{
		isdecrypt = False;
	}
};

class MCCipherDecrypt : public MCEncryptionOp
{
public:
	MCCipherDecrypt()
	{
		isdecrypt = True;
	}
};

class MCFilter : public MCStatement
{
	MCChunk *container;
	MCExpression *pattern;
	Boolean out;
public:
	MCFilter()
	{
		container = NULL;
		pattern = NULL;
		out = False;
	}
	virtual ~MCFilter();
	Boolean match(char *s, char *p, Boolean casesensitive);
	char *filterlines(char *sstring, char *pstring, Boolean casesensitive);
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCImport : public MCStatement
{
	Export_format format;
	MCExpression *fname;
	MCExpression *mname;
	MCExpression *dname;
	MCChunk *container;
	MCExpression *size;
	bool with_effects : 1;
public:
	MCImport()
	{
		fname = mname = dname = NULL;
		container = NULL;
		size = NULL;
		with_effects = false;
	}
	virtual ~MCImport();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCKill : public MCStatement
{
	MCExpression *sig;
	MCExpression *pname;
public:
	MCKill()
	{
		sig = NULL;
		pname = NULL;
	}
	int4 lookup(const MCString &s);
	virtual ~MCKill();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCOpen : public MCStatement
{
	MCExpression *fname;
	MCExpression *message;
	union
	{
		MCExpression *certificate; // if open secure socket
		MCExpression *options; // if open printing to
	};
	MCGo *go;
	char *destination;
	Open_argument arg : 4;
	Open_mode mode : 4;
	Boolean textmode : 1;
	Boolean dialog : 1;
	Boolean datagram : 1;
	Boolean sheet : 1;
	Boolean secure : 1;
	Boolean secureverify : 1;

	// MW-2010-05-09: Indicates that the process should be opened with elevated
	//   (admin) permissions
	Boolean elevated : 1;
public:
	MCOpen()
	{
		fname = message = NULL;
		mode = OM_UPDATE;
		textmode = True;
		datagram = dialog = sheet = False;
		go = NULL;
		certificate = NULL;
		secure = False;
		secureverify = True;
		destination = NULL;
		elevated = False;
	}
	virtual ~MCOpen();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCRead : public MCStatement
{
	Open_argument arg;
	MCExpression *fname;
	Repeat_form cond;
	MCExpression *stop;
	File_unit unit;
	MCExpression *maxwait;
	Functions timeunits;
	MCVarref *it;
	MCExpression *at;
public:
	MCRead()
	{
		fname = NULL;
		maxwait = NULL;
		stop = NULL;
		unit = FU_CHARACTER;
		it = NULL;
		at = NULL;
	}
	virtual ~MCRead();
	IO_stat readfor(IO_handle stream, int4 pindex, File_unit unit,
	                uint4 bytes, MCExecPoint &ep, real8 duration);
	IO_stat readuntil(IO_handle stream, int4 pindex, uint4 count,
	                  const char *sptr, MCExecPoint &ep, Boolean words,
	                  real8 duration);
	IO_stat readuntil_binary(IO_handle stream, int4 pindex, uint4 count, const MCString &sptr, MCExecPoint &ep,Boolean words, real8 duration);
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCSeek : public MCStatement
{
	Preposition_type mode;
	MCExpression *where;
	MCExpression *fname;
public:
	MCSeek()
	{
		fname = NULL;
		where = NULL;
	}
	virtual ~MCSeek();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCWrite : public MCStatement
{
	MCExpression *source;
	Open_argument arg;
	MCExpression *fname;
	File_unit unit;
	MCExpression *at;
public:
	MCWrite()
	{
		source = NULL;
		fname = NULL;
		unit = FU_CHARACTER;
		at = NULL;
	}
	virtual ~MCWrite();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// math comands in cmdsm.cc

class MCAdd : public MCStatement
{
	MCExpression *source;
	MCChunk *dest;
	MCVarref *destvar;
	bool overlap;
public:
	MCAdd()
	{
		source = NULL;
		dest = NULL;
		destvar = NULL;
		overlap = false;
	}
	virtual ~MCAdd();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCDivide : public MCStatement
{
	MCExpression *source;
	MCChunk *dest;
	MCVarref *destvar;
	bool overlap;
public:
	MCDivide()
	{
		source = NULL;
		dest = NULL;
		destvar = NULL;
		overlap = false;
	}
	virtual ~MCDivide();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCMultiply : public MCStatement
{
	MCExpression *source;
	MCChunk *dest;
	MCVarref *destvar;
	bool overlap;
public:
	MCMultiply()
	{
		source = NULL;
		dest = NULL;
		destvar = NULL;
		overlap = false;
	}
	virtual ~MCMultiply();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCSubtract : public MCStatement
{
	MCExpression *source;
	MCChunk *dest;
	MCVarref *destvar;
	bool overlap;
public:
	MCSubtract()
	{
		source = NULL;
		dest = NULL;
		destvar = NULL;
		overlap = false;
	}
	virtual ~MCSubtract();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCArrayOp : public MCStatement
{
	MCVarref *destvar;
	MCExpression *element;
	MCExpression *key;
protected:
	enum
	{
		TYPE_USER,
		TYPE_ROW,
		TYPE_COLUMN,
		TYPE_LINE,
		TYPE_ITEM,
		TYPE_WORD,
		TYPE_TOKEN,
		TYPE_CHARACTER,
		TYPE_MASK = 0xf,
		TYPE_IS_COMBINE = 1U << 31
	};

	enum
	{
		FORM_NONE,
		FORM_SET
	};

	bool is_combine : 1;
	unsigned mode : 4;
	unsigned form : 4;
public:
	MCArrayOp()
	{
		destvar = NULL;
		element = key = NULL;
		mode = TYPE_USER;
		form = FORM_NONE;
	}
	virtual ~MCArrayOp();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCCombine : public MCArrayOp
{
public:
	MCCombine()
	{
		is_combine = true;
	}
};

class MCSplit : public MCArrayOp
{
public:
	MCSplit()
	{
		is_combine = false;
	}
};

class MCSetOp : public MCStatement
{
	MCVarref *destvar;
	MCExpression *source;
protected:
	Boolean intersect : 1;
public:
	MCSetOp()
	{
		source = NULL;
		destvar = NULL;
	}
	virtual ~MCSetOp();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCArrayIntersectCmd : public MCSetOp
{
public:
	MCArrayIntersectCmd()
	{
		intersect = True;
	}
};

class MCArrayUnionCmd : public MCSetOp
{
public:
	MCArrayUnionCmd()
	{
		intersect = False;
	}
};

// MCStack manipulation comands in cmdss.cc

class MCCompact : public MCStatement
{
	MCChunk *target;
public:
	MCCompact()
	{
		target = NULL;
	}
	virtual ~MCCompact();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCGo : public MCStatement
{
	MCCRef *background;
	MCCRef *stack;
	MCCRef *card;
	MCExpression *window;
	Window_mode mode;
	Boolean marked;
	Boolean visible;
	Boolean thisstack;
public:
	MCGo()
	{
		mode = WM_LAST;
		stack = background = card = NULL;
		window = NULL;
		marked = thisstack = False;
		visible = True;
	}
	virtual ~MCGo();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
#ifdef OLD_EXEC
	MCStack *findstack(MCExecPoint &ep, Chunk_term etype, MCCard *&cptr);
#endif
	MCStack *findstack(MCExecPoint &ep, Chunk_term etype, MCCard *&cptr, bool &binary_fail);
};

class MCHide : public MCStatement
{
	Show_object which;
	MCChunk *object;
	Boolean card;
	MCVisualEffect *effect;
public:
	MCHide()
	{
		which = SO_OBJECT;
		object = NULL;
		effect = NULL;
	}
	virtual ~MCHide();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCLock : public MCStatement
{
	Lock_constants which;
	// MW-2011-09-24: [[ Effects ]] Holds the rect expr for 'lock screen for visual effect in <rect>'.
	MCExpression *rect;
public:
	MCLock(void)
	{
		which = LC_UNDEFINED;
		rect = nil;
	}
	virtual ~MCLock(void);
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPop : public MCStatement
{
	Preposition_type prep;
	MCChunk *dest;
public:
	MCPop()
	{
		dest = NULL;
	}
	virtual ~MCPop();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCPush : public MCStatement
{
	MCChunk *card;
	Boolean recent;
public:
	MCPush()
	{
		card = NULL;
		recent = False;
	}
	virtual ~MCPush();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCSave : public MCStatement
{
	MCChunk *target;
	MCExpression *filename;
public:
	MCSave()
	{
		target = NULL;
		filename = NULL;
	}
	virtual ~MCSave();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCShow : public MCStatement
{
	Show_object which;
	Boolean card;
	MCChunk *ton;
	MCExpression *location;
	Boolean all;
	MCVisualEffect *effect;
public:
	MCShow()
	{
		which = SO_OBJECT;
		ton = NULL;
		location = NULL;
		effect = NULL;
	}
	virtual ~MCShow();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCSubwindow : public MCStatement
{
	MCChunk *target;
	Boolean thisstack;
	MCExpression *parent;
	MCExpression *at;
	MCExpression *aligned;
protected:
	Window_mode mode;
public:
	MCSubwindow()
	{
		target = NULL;
		at = NULL;
		parent = NULL;
		thisstack = False;
		aligned = NULL;
	}
	virtual ~MCSubwindow();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

class MCTopLevel : public MCSubwindow
{
public:
	MCTopLevel()
	{
		mode = WM_TOP_LEVEL;
	}
};

class MCModal : public MCSubwindow
{
public:
	MCModal()
	{
		mode = WM_MODAL;
	}
};

class MCModeless : public MCSubwindow
{
public:
	MCModeless()
	{
		mode = WM_MODELESS;
	}
};

class MCOption : public MCSubwindow
{
public:
	MCOption()
	{
		mode = WM_OPTION;
	}
};

class MCPalette : public MCSubwindow
{
public:
	MCPalette()
	{
		mode = WM_PALETTE;
	}
};

class MCPopup : public MCSubwindow
{
public:
	MCPopup()
	{
		mode = WM_POPUP;
	}
};

class MCPulldown : public MCSubwindow
{
public:
	MCPulldown()
	{
		mode = WM_PULLDOWN;
	}
};

class MCSheet : public MCSubwindow
{
public:
	MCSheet()
	{
		mode = WM_SHEET;
	}
};


class MCDrawer : public MCSubwindow
{
public:
	MCDrawer()
	{
		mode = WM_DRAWER;
	}
};

class MCUnlock : public MCStatement
{
	Lock_constants which;
	MCVisualEffect *effect;
public:
	MCUnlock()
	{
		effect = NULL;
	}
	virtual ~MCUnlock();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
};

// MCPrinting cmdsp.cc

class MCPrint : public MCStatement
{
	Print_mode mode;
	MCChunk *target;
	MCExpression *from;
	MCExpression *to;
	MCExpression *rect;
	MCExpression *initial_state;
	bool bookmark_closed;
public:
	MCPrint();
	virtual ~MCPrint();
	virtual Parse_stat parse(MCScriptPoint &);
	virtual Exec_stat exec(MCExecPoint &);
	virtual void compile(MCSyntaxFactoryRef);
private:
	Exec_stat evaluate_src_rect(MCExecPoint& ep, MCPoint& r_from, MCPoint& r_to);
};

class MCInclude: public MCStatement
{
public:
	MCInclude(bool p_is_require)
	{
		is_require = p_is_require;
		filename = NULL;
	}
	
	virtual ~MCInclude(void);
	virtual Parse_stat parse(MCScriptPoint& sp);
	virtual Exec_stat exec(MCExecPoint& ep);
	virtual void compile(MCSyntaxFactoryRef);

private:
	bool is_require;
	MCExpression *filename;
};

class MCEcho: public MCStatement
{
public:
	MCEcho(void)
	{
	}
	
	virtual ~MCEcho(void);
	virtual Parse_stat parse(MCScriptPoint& sp);
	virtual Exec_stat exec(MCExecPoint& ep);
	virtual void compile(MCSyntaxFactoryRef);

private:
	MCString data;
};

#endif
