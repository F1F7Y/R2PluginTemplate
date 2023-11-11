#pragma once

#include "plugin_abi.h"
#include "loader.h"

typedef void (*logger_t)(void*);


class Plugin {
	public:
		logger_t logger;
		int handle;

		void Init(PluginInitFuncs* funcs, PluginNorthstarData* data);

		void Main(); // To be specified by the plugin developer

		void DLLLoadEngine(); // To be specified by the plugin developer
		void DLLLoadClient(); // To be specified by the plugin 
		void DLLLoadServer(); // To be specified by the plugin developer

	protected:
		PLUGIN_RELAY_INVITE_TYPE _RelayInvite;

	public:
		void RelayInvite(const char* invite);
};

extern Plugin* g_pPlugin;