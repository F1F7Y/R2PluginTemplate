#include "lib.h"
#include "plugin_abi.h"
#include "squirrelautobind.h"

#include "../pch.h"

Plugin* g_pPlugin;

void Plugin::Init(PluginInitFuncs* funcs, PluginNorthstarData* data) 
{
	handle = data->pluginHandle;
	presence = new GameStatePresenceClass();
	presence->state = GameState::LOADING;
	_RelayInvite = funcs->relayInviteFunc;
}

void Plugin::RelayInvite(const char* invite) {
	spdlog::info("Join Secret: {}", invite);
	if (_RelayInvite != nullptr)
		_RelayInvite(invite);
}

void Plugin::LoadPresence(PluginGameStatePresence* data) {
	std::unique_lock lock(presence->mutex);

	presence->id = data->id;
	presence->name = data->name;
	presence->description = data->description;
	presence->password = data->password;


	presence->is_local = data->is_local;
	presence->is_server = data->is_server;
	presence->state = data->state;

	presence->map = data->map;
	presence->map_displayname = data->map_displayname;
	presence->playlist = data->playlist;
	presence->playlist_displayname = data->playlist_displayname;

	presence->current_players = data->current_players;
	presence->max_players = data->max_players;
	presence->own_score = data->own_score;
	presence->other_highest_score = data->other_highest_score;
	presence->max_score = data->max_score;

	presence->timestamp_end = data->timestamp_end;
}