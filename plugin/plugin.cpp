#include "plugin.h"

#include "pch.h"
#include "lib.h"

void Plugin::Main() {}
void Plugin::DLLLoadEngine() {
//-----------------------------------------------------------------------------
// Custom ConCommand example
//-----------------------------------------------------------------------------
	RegisterConCommand("cplug_test", [](const CCommand& command){ spdlog::info("Gaming"); }, "", 0);
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Custom ConVar example
//-----------------------------------------------------------------------------
	ConVar *pCustomConVar = new ConVar( "CPLUG_CONVAR", "Very cool", FCVAR_CLIENTDLL, "Example convar" );
//-----------------------------------------------------------------------------
}

void Plugin::DLLLoadClient() {}
void Plugin::DLLLoadServer() {}


//-----------------------------------------------------------------------------
// Custom squirrel function example
//-----------------------------------------------------------------------------
#include "squirrel.h"
#include "squirrelautobind.h"

ADD_SQFUNC("void", CPlugTest, "", "", ScriptContext::CLIENT | ScriptContext::SERVER | ScriptContext::UI)
{
	spdlog::info( "Hi :)" );
	return SQRESULT_NULL;
}
//-----------------------------------------------------------------------------