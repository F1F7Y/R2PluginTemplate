#include "loader.h"
#include "lib.h"

#include "squirrel.h"
#include <iostream>

#include "logging.h"

#include "concommand.h"
#include "convar.h"
#include "sourceinterface.h"

void PLUGIN_INIT(PluginInitFuncs* funcs, PluginNorthstarData* data) {
	g_pPlugin = new Plugin;
	g_pPlugin->handle = data->pluginHandle;
	g_pPlugin->logger = (logger_t)funcs->logger;
	spdlog::default_logger()->sinks().pop_back();
	spdlog::default_logger()->sinks().push_back(std::make_shared<my_sink>());
	spdlog::info("CPluginTest succesfully initialised!");
	g_pPlugin->Init(funcs, data);
	g_pPlugin->Main();
}

void PLUGIN_INIT_SQVM_CLIENT(SquirrelFunctions* funcs) {
	InitializeSquirrelVM_CLIENT(funcs);
}

void PLUGIN_INIT_SQVM_SERVER(SquirrelFunctions* funcs) {
	InitializeSquirrelVM_SERVER(funcs);
}

void PLUGIN_INFORM_SQVM_CREATED(ScriptContext context, CSquirrelVM* sqvm) {
	switch (context)
	{
	case ScriptContext::CLIENT:
		g_pSquirrel<ScriptContext::CLIENT>->VMCreated(sqvm);
		break;
	case ScriptContext::SERVER:
		g_pSquirrel<ScriptContext::SERVER>->VMCreated(sqvm);
		break;
	case ScriptContext::UI:
		g_pSquirrel<ScriptContext::UI>->VMCreated(sqvm);
		break;
	default:
		spdlog::warn("PLUGIN_INFORM_SQVM_CREATED called with unknown ScriptContext {}", (int)context);
		break;
	}
}

void PLUGIN_INFORM_SQVM_DESTROYED(ScriptContext context) {
	switch (context)
	{
	case ScriptContext::CLIENT:
		g_pSquirrel<ScriptContext::CLIENT>->VMDestroyed();
		break;
	case ScriptContext::SERVER:
		g_pSquirrel<ScriptContext::SERVER>->VMDestroyed();
		break;
	case ScriptContext::UI:
		g_pSquirrel<ScriptContext::UI>->VMDestroyed();
		break;
	default:
		spdlog::warn("PLUGIN_INFORM_SQVM_DESTROYED called with unknown ScriptContext {}", (int)context);
		break;
	}
}

void LoadDLLEngine(PluginEngineData* data) {
	ConCommandConstructor = (ConCommandConstructorType)(data->ConCommandConstructor);
	conVarMalloc = (ConVarMallocType)(data->conVarMalloc);
	conVarRegister = (ConVarRegisterType)(data->conVarRegister);

	g_pConVar_Vtable = data->ConVar_Vtable;
	g_pIConVar_Vtable = data->IConVar_Vtable;

	g_pPlugin->DLLLoadEngine();
}

void LoadDLLClient() {
	g_pPlugin->DLLLoadClient();
}

void LoadDLLServer() {
	g_pPlugin->DLLLoadServer();
}

void PLUGIN_INFORM_DLL_LOAD(PluginLoadDLL dll, void* data) {
	switch (dll) {
		case PluginLoadDLL::ENGINE:
			LoadDLLEngine(static_cast<PluginEngineData*>(data));
			break;
		case PluginLoadDLL::CLIENT:
			LoadDLLClient();
			break;
		case PluginLoadDLL::SERVER:
			LoadDLLServer();
			break;
		default:
			spdlog::warn("PLUGIN_INFORM_SQVM_DESTROYED called with unknown PluginLoadDLL type {}", (int)dll);
			break;
	}
}