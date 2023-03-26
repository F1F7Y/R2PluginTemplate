#include "../pch.h"

#include <shared_mutex>

class GameStatePresenceClass
 {
	public:
		std::string id;
		std::string name;
		std::string description;
		std::string password; // NOTE: May be empty

		bool is_server;
		bool is_local;
		GameState state;

		std::string map;
		std::string map_displayname;
		std::string playlist;
		std::string playlist_displayname;

		int current_players;
		int max_players;

		int own_score;
		int other_highest_score; // NOTE: The highest score OR the second highest score if we have the highest
		int max_score;

		int timestamp_end;

		std::shared_mutex mutex;
};