#include "../pch.h"
#include <map>
#include <string>
#include "squirrel.h"
#include "lib.h"
#include "plugin_abi.h"

const char* GetContextName(ScriptContext context)
{
	switch (context)
	{
	case ScriptContext::CLIENT:
		return "CLIENT";
	case ScriptContext::SERVER:
		return "SERVER";
	case ScriptContext::UI:
		return "UI";
	default:
		return "UNKNOWN";
	}
}

eSQReturnType SQReturnTypeFromString(const char* pReturnType)
{
	static const std::map<std::string, eSQReturnType> sqReturnTypeNameToString = {
		{"bool", eSQReturnType::Boolean},
		{"float", eSQReturnType::Float},
		{"vector", eSQReturnType::Vector},
		{"int", eSQReturnType::Integer},
		{"entity", eSQReturnType::Entity},
		{"string", eSQReturnType::String},
		{"array", eSQReturnType::Arrays},
		{"asset", eSQReturnType::Asset},
		{"table", eSQReturnType::Table}};

	if (sqReturnTypeNameToString.find(pReturnType) != sqReturnTypeNameToString.end())
		return sqReturnTypeNameToString.at(pReturnType);
	else
		return eSQReturnType::Default; // previous default value
}

const char* SQTypeNameFromID(int type)
{
	switch (type)
	{
	case OT_ASSET:
		return "asset";
	case OT_INTEGER:
		return "int";
	case OT_BOOL:
		return "bool";
	case SQOBJECT_NUMERIC:
		return "float or int";
	case OT_NULL:
		return "null";
	case OT_VECTOR:
		return "vector";
	case 0:
		return "var";
	case OT_USERDATA:
		return "userdata";
	case OT_FLOAT:
		return "float";
	case OT_STRING:
		return "string";
	case OT_ARRAY:
		return "array";
	case 0x8000200:
		return "function";
	case 0x8100000:
		return "structdef";
	case OT_THREAD:
		return "thread";
	case OT_FUNCPROTO:
		return "function";
	case OT_CLAAS:
		return "class";
	case OT_WEAKREF:
		return "weakref";
	case 0x8080000:
		return "unimplemented function";
	case 0x8200000:
		return "struct instance";
	case OT_TABLE:
		return "table";
	case 0xA008000:
		return "instance";
	case OT_ENTITY:
		return "entity";
	}
	return "";
}

// needed to define implementations for squirrelmanager outside of squirrel.h without compiler errors
template class SquirrelManager<ScriptContext::SERVER>;
template class SquirrelManager<ScriptContext::CLIENT>;
template class SquirrelManager<ScriptContext::UI>;

template <ScriptContext context> void SquirrelManager<context>::VMCreated(CSquirrelVM* newSqvm)
{
	m_pSQVM = newSqvm;

	for (SQFuncRegistration* funcReg : m_funcRegistrations)
	{
		spdlog::info("Registering {} function {}", GetContextName(context), funcReg->squirrelFuncName);
		RegisterSquirrelFunc(m_pSQVM, funcReg, 1);
	}
	g_pSquirrel<context>->messageBuffer = new SquirrelMessageBuffer();
}

template <ScriptContext context> void SquirrelManager<context>::VMDestroyed()
{
	m_pSQVM = nullptr;
}

template <ScriptContext context> void SquirrelManager<context>::ExecuteCode(const char* pCode)
{
	if (!m_pSQVM || !m_pSQVM->sqvm)
	{
		spdlog::error("Cannot execute code, {} squirrel vm is not initialised", GetContextName(context));
		return;
	}

	spdlog::info("Executing {} script code {} ", GetContextName(context), pCode);

	std::string strCode(pCode);
	CompileBufferState bufferState = CompileBufferState(strCode);

	SQRESULT compileResult = compilebuffer(&bufferState, "console");
	spdlog::info("sq_compilebuffer returned {}", PrintSQRESULT.at(compileResult));

	if (compileResult != SQRESULT_ERROR)
	{
		pushroottable(m_pSQVM->sqvm);
		SQRESULT callResult = _call(m_pSQVM->sqvm, 0);
		spdlog::info("sq_call returned {}", PrintSQRESULT.at(callResult));
	}
}

template <ScriptContext context> void SquirrelManager<context>::AddFuncRegistration(
	std::string returnType, std::string name, std::string argTypes, std::string helpText, SQFunction func)
{
	SQFuncRegistration* reg = new SQFuncRegistration;

	reg->squirrelFuncName = new char[name.size() + 1];
	strcpy((char*)reg->squirrelFuncName, name.c_str());
	reg->cppFuncName = reg->squirrelFuncName;

	reg->helpText = new char[helpText.size() + 1];
	strcpy((char*)reg->helpText, helpText.c_str());

	reg->returnTypeString = new char[returnType.size() + 1];
	strcpy((char*)reg->returnTypeString, returnType.c_str());
	reg->returnType = SQReturnTypeFromString(returnType.c_str());

	reg->argTypes = new char[argTypes.size() + 1];
	strcpy((char*)reg->argTypes, argTypes.c_str());

	reg->funcPtr = func;

	m_funcRegistrations.push_back(reg);
}

template <ScriptContext context> void SquirrelManager<context>::AddFuncOverride(std::string name, SQFunction func)
{
	m_funcOverrides[name] = func;
}

// hooks
bool IsUIVM(ScriptContext context, HSquirrelVM* pSqvm)
{
	return context != ScriptContext::SERVER && g_pSquirrel<ScriptContext::UI>->m_pSQVM &&
		   g_pSquirrel<ScriptContext::UI>->m_pSQVM->sqvm == pSqvm;
}

// literal class type that wraps a constant expression string
template <size_t N> struct TemplateStringLiteral
{
	constexpr TemplateStringLiteral(const char (&str)[N])
	{
		std::copy_n(str, N, value);
	}

	char value[N];
};

void InitializeSquirrelVM_CLIENT(SquirrelFunctions* funcs)
{
	g_pSquirrel<ScriptContext::CLIENT> = new SquirrelManager<ScriptContext::CLIENT>;
	g_pSquirrel<ScriptContext::UI> = new SquirrelManager<ScriptContext::UI>;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_defconst = funcs->__sq_defconst;
	g_pSquirrel<ScriptContext::UI>->__sq_defconst = g_pSquirrel<ScriptContext::CLIENT>->__sq_defconst;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_compilebuffer = funcs->__sq_compilebuffer;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushroottable = funcs->__sq_pushroottable;
	g_pSquirrel<ScriptContext::UI>->__sq_compilebuffer = g_pSquirrel<ScriptContext::CLIENT>->__sq_compilebuffer;
	g_pSquirrel<ScriptContext::UI>->__sq_pushroottable = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushroottable;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_call = funcs->__sq_call;
	g_pSquirrel<ScriptContext::UI>->__sq_call = g_pSquirrel<ScriptContext::CLIENT>->__sq_call;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_newarray = funcs->__sq_newarray;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_arrayappend = funcs->__sq_arrayappend;
	g_pSquirrel<ScriptContext::UI>->__sq_newarray = g_pSquirrel<ScriptContext::CLIENT>->__sq_newarray;
	g_pSquirrel<ScriptContext::UI>->__sq_arrayappend = g_pSquirrel<ScriptContext::CLIENT>->__sq_arrayappend;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_newtable = funcs->__sq_newtable;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_newslot = funcs->__sq_newslot;
	g_pSquirrel<ScriptContext::UI>->__sq_newtable = g_pSquirrel<ScriptContext::CLIENT>->__sq_newtable;
	g_pSquirrel<ScriptContext::UI>->__sq_newslot = g_pSquirrel<ScriptContext::CLIENT>->__sq_newslot;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushstring = funcs->__sq_pushstring;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushinteger = funcs->__sq_pushinteger;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushfloat = funcs->__sq_pushfloat;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushbool = funcs->__sq_pushbool;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushasset = funcs->__sq_pushasset;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushvector = funcs->__sq_pushvector;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_pushobject = funcs->__sq_pushobject;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_raiseerror = funcs->__sq_raiseerror;
	g_pSquirrel<ScriptContext::UI>->__sq_pushstring = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushstring;
	g_pSquirrel<ScriptContext::UI>->__sq_pushinteger = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushinteger;
	g_pSquirrel<ScriptContext::UI>->__sq_pushfloat = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushfloat;
	g_pSquirrel<ScriptContext::UI>->__sq_pushbool = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushbool;
	g_pSquirrel<ScriptContext::UI>->__sq_pushvector = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushvector;
	g_pSquirrel<ScriptContext::UI>->__sq_pushasset = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushasset;
	g_pSquirrel<ScriptContext::UI>->__sq_pushobject = g_pSquirrel<ScriptContext::CLIENT>->__sq_pushobject;
	g_pSquirrel<ScriptContext::UI>->__sq_raiseerror = g_pSquirrel<ScriptContext::CLIENT>->__sq_raiseerror;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_getstring = funcs->__sq_getstring;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getinteger = funcs->__sq_getinteger;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getfloat = funcs->__sq_getfloat;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getbool = funcs->__sq_getbool;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_get = funcs->__sq_get;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getasset = funcs->__sq_getasset;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getuserdata = funcs->__sq_getuserdata;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getvector = funcs->__sq_getvector;
	g_pSquirrel<ScriptContext::UI>->__sq_getstring = g_pSquirrel<ScriptContext::CLIENT>->__sq_getstring;
	g_pSquirrel<ScriptContext::UI>->__sq_getinteger = g_pSquirrel<ScriptContext::CLIENT>->__sq_getinteger;
	g_pSquirrel<ScriptContext::UI>->__sq_getfloat = g_pSquirrel<ScriptContext::CLIENT>->__sq_getfloat;
	g_pSquirrel<ScriptContext::UI>->__sq_getbool = g_pSquirrel<ScriptContext::CLIENT>->__sq_getbool;
	g_pSquirrel<ScriptContext::UI>->__sq_get = g_pSquirrel<ScriptContext::CLIENT>->__sq_get;
	g_pSquirrel<ScriptContext::UI>->__sq_getasset = g_pSquirrel<ScriptContext::CLIENT>->__sq_getasset;
	g_pSquirrel<ScriptContext::UI>->__sq_getuserdata = g_pSquirrel<ScriptContext::CLIENT>->__sq_getuserdata;
	g_pSquirrel<ScriptContext::UI>->__sq_getvector = g_pSquirrel<ScriptContext::CLIENT>->__sq_getvector;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_createuserdata = funcs->__sq_createuserdata;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_setuserdatatypeid = funcs->__sq_setuserdatatypeid;
	g_pSquirrel<ScriptContext::UI>->__sq_createuserdata = g_pSquirrel<ScriptContext::CLIENT>->__sq_createuserdata;
	g_pSquirrel<ScriptContext::UI>->__sq_setuserdatatypeid = g_pSquirrel<ScriptContext::CLIENT>->__sq_setuserdatatypeid;


	// Message buffer stuff
	g_pSquirrel<ScriptContext::UI>->messageBuffer = g_pSquirrel<ScriptContext::CLIENT>->messageBuffer;
	g_pSquirrel<ScriptContext::CLIENT>->__sq_getfunction = funcs->__sq_getfunction;
	g_pSquirrel<ScriptContext::UI>->__sq_getfunction = g_pSquirrel<ScriptContext::CLIENT>->__sq_getfunction;

	g_pSquirrel<ScriptContext::CLIENT>->RegisterSquirrelFunc = funcs->RegisterSquirrelFunc;
	g_pSquirrel<ScriptContext::UI>->RegisterSquirrelFunc = funcs->RegisterSquirrelFunc;

	g_pSquirrel<ScriptContext::CLIENT>->__sq_schedule_call_external = funcs->__sq_schedule_call_external;
	g_pSquirrel<ScriptContext::UI>->__sq_schedule_call_external = funcs->__sq_schedule_call_external;

	if (g_pSqAutoBindContainer)
		for (auto& autoBindFunc : g_pSqAutoBindContainer->clientSqAutoBindFuncs)
			autoBindFunc();
}

void InitializeSquirrelVM_SERVER(SquirrelFunctions * funcs)
{
	g_pSquirrel<ScriptContext::SERVER> = new SquirrelManager<ScriptContext::SERVER>;

	g_pSquirrel<ScriptContext::SERVER>->__sq_defconst = funcs->__sq_defconst;

	g_pSquirrel<ScriptContext::SERVER>->__sq_compilebuffer = funcs->__sq_compilebuffer;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushroottable = funcs->__sq_pushroottable;
	g_pSquirrel<ScriptContext::SERVER>->__sq_call = funcs->__sq_call;

	g_pSquirrel<ScriptContext::SERVER>->__sq_newarray = funcs->__sq_newarray;
	g_pSquirrel<ScriptContext::SERVER>->__sq_arrayappend = funcs->__sq_arrayappend;

	g_pSquirrel<ScriptContext::SERVER>->__sq_newtable = funcs->__sq_newtable;
	g_pSquirrel<ScriptContext::SERVER>->__sq_newslot = funcs->__sq_newslot;

	g_pSquirrel<ScriptContext::SERVER>->__sq_pushstring = funcs->__sq_pushstring;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushinteger = funcs->__sq_pushinteger;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushfloat = funcs->__sq_pushfloat;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushbool = funcs->__sq_pushbool;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushasset = funcs->__sq_pushasset;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushvector = funcs->__sq_pushvector;
	g_pSquirrel<ScriptContext::SERVER>->__sq_pushobject = funcs->__sq_pushobject;

	g_pSquirrel<ScriptContext::SERVER>->__sq_raiseerror = funcs->__sq_raiseerror;

	g_pSquirrel<ScriptContext::SERVER>->__sq_getstring = funcs->__sq_getstring;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getinteger = funcs->__sq_getinteger;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getfloat = funcs->__sq_getfloat;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getbool = funcs->__sq_getbool;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getasset = funcs->__sq_getasset;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getuserdata = funcs->__sq_getuserdata;
	g_pSquirrel<ScriptContext::SERVER>->__sq_getvector = funcs->__sq_getvector;
	g_pSquirrel<ScriptContext::SERVER>->__sq_get = funcs->__sq_get;

	g_pSquirrel<ScriptContext::SERVER>->__sq_createuserdata = funcs->__sq_createuserdata;
	g_pSquirrel<ScriptContext::SERVER>->__sq_setuserdatatypeid = funcs->__sq_setuserdatatypeid;

	// Message buffer stuff
	g_pSquirrel<ScriptContext::SERVER>->__sq_getfunction = funcs->__sq_getfunction;

	g_pSquirrel<ScriptContext::SERVER>->RegisterSquirrelFunc = funcs->RegisterSquirrelFunc;

	g_pSquirrel<ScriptContext::SERVER>->__sq_schedule_call_external = funcs->__sq_schedule_call_external;

	if (g_pSqAutoBindContainer)
		for (auto& autoBindFunc : g_pSqAutoBindContainer->serverSqAutoBindFuncs)
			autoBindFunc();
}

template <ScriptContext context> void unwind_message(HSquirrelVM* sqvm) {
	g_pSquirrel<context>->messageBuffer->unwind();
}