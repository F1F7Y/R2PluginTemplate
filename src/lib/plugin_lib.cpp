#include "lib.h"
#include "plugin_abi.h"
#include "squirrelautobind.h"

#include "../pch.h"

Plugin* g_pPlugin;

void Plugin::Init(PluginInitFuncs* funcs, PluginNorthstarData* data) 
{
	handle = data->pluginHandle;
	_RelayInvite = funcs->relayInviteFunc;
}

void Plugin::RelayInvite(const char* invite) {
	spdlog::info("Join Secret: {}", invite);
	if (_RelayInvite != nullptr)
		_RelayInvite(invite);
}