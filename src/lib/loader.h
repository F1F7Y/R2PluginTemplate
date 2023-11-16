#pragma once

#include "pch.h"

EXPORT void PLUGIN_INIT(PluginInitFuncs* funcs, PluginNorthstarData* data);

EXPORT void PLUGIN_INIT_SQVM_CLIENT(SquirrelFunctions* funcs);
EXPORT void PLUGIN_INIT_SQVM_SERVER(SquirrelFunctions* funcs);
EXPORT void PLUGIN_INFORM_SQVM_CREATED(ScriptContext context, CSquirrelVM* sqvm);

EXPORT void PLUGIN_INFORM_DLL_LOAD(PluginLoadDLL dll, void* data);