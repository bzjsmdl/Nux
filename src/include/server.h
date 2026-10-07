#pragma once

#include "cdlib.h"
#include "rt.h"
#include "error.h"
#include "console.h"

extern Table* t;

using namespace nux;

// get more information about keys of `server.properties`:
// - [English](https://minecraft.wiki/w/Server.properties)
// - [中文](https://zh.minecraft.wiki/w/%E6%9C%8D%E5%8A%A1%E7%AB%AF%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6%E6%A0%BC%E5%BC%8F)
typedef struct ServerProperties {
	std::string server_name = "Dedicated Server";
	std::string gamemode = "survival";
	bool force_gamemode = false;
	std::string difficulty = "easy";
	bool allow_cheats = false;
	size_t max_players = 10;
	bool online_mode = true;
	bool allow_list = true;
	size_t server_port = 19132;
	size_t server_portv6 = 19133;
	bool enable_lan_visibility = true;
	size_t view_distance = 32;
	size_t tick_distance = 4;
	size_t player_idle_timeout = 30;
	bool allow_player_joining = true;
	size_t max_threads = 8;
	std::string level_name = "Bedrock level";
	signed long long int seed;	// default null.
	std::string default_player_permission_level = "member";
	bool texturepack_required = false;
	bool content_log_file_enabled = false;
	bool content_log_console_output_enabled = false;
	std::string content_log_level = "info";
	size_t compression_threshold = 1;
	std::string compression_algorithm = "zlib";
	bool server_authoritative_movement_strict = false;
	bool server_authoritative_dismount_strict = false;
	bool server_authoritative_entity_interactions_strict = false;
	float player_position_acceptance_threshold = 0.5;
	float player_movement_action_direction_threshold = 0.85;
	float server_authoritative_block_breaking_pick_range_scalar = 1.5;
	std::string chat_restriction = "None";
	bool disable_player_interaction = false;
	bool client_side_chunk_generation_enabled = true;
	bool block_network_ids_are_hashes = true;
	bool disable_persona = false;
	bool disable_custom_skins = false;

	std::string server_build_radius_ratio;	// defaults disable

	bool allow_outbound_script_debugging = false;
	bool allow_inbound_script_debugging = false;
	size_t force_inbound_debug_port = 19144;
	std::string script_debugger_auto_attach = "disabled";
	std::string script_debugger_auto_attach_connect_address = "localhost:19144";
	size_t script_debugger_auto_attach_timeout = 0;
	std::string script_debugger_passcode;
	bool script_watchdog_enable = true;
	bool script_watchdog_enable_exception_handling = true;
	bool script_watchdog_enable_shutdown = true;
	bool script_watchdog_hang_exception = true;
	size_t script_watchdog_hang_threshold = 10000;
	size_t script_watchdog_spike_threshold = 100;
	size_t script_watchdog_slow_threshold = 10;
	size_t script_watchdog_memory_warning = 100;
	size_t script_watchdog_memory_limit = 100;
	bool diagnostics_capture_auto_start = false;
	size_t diagnostics_capture_max_files = 5;
	size_t diagnostics_capture_max_file_size = 2097152;
	bool disable_client_vibrant_visuals = false;
	size_t sentry_rate_limit_window = 60;
	size_t sentry_max_events_per_window = 10;
	bool enable_profiler = true;
	bool enable_editor_network_metrics = true;
	std::string level_type = "DEFAULT";
	bool server_authoritative_block_breaking = true;
	bool emit_server_telemetry = false;
	bool isHardcore = false;
	std::string language = "en_US";
	size_t op_permission_level = 1;

	// we defined several options for netease minecraft (我们定义了一些有关于网易我的世界的选项)
	// emm...these options may look like Nukkit-MOT...
	bool netease_support = false;
	bool only_netease = false;
	
	std::any operator[] (size_t n) {
		switch (n) {
			case 0: return std::ref(server_name);
			case 1: return std::ref(gamemode);
			case 2: return std::ref(force_gamemode);
			case 3: return std::ref(difficulty);
			case 4: return std::ref(allow_cheats);
			case 5: return std::ref(max_players);
			case 6: return std::ref(online_mode);
			case 7: return std::ref(allow_list);
			case 8: return std::ref(server_port);
			case 9: return std::ref(server_portv6);
			case 10: return std::ref(enable_lan_visibility);
			case 11: return std::ref(view_distance);
			case 12: return std::ref(tick_distance);
			case 13: return std::ref(player_idle_timeout);
			case 14: return std::ref(allow_player_joining);
			case 15: return std::ref(max_threads);
			case 16: return std::ref(level_name);
			case 17: return std::ref(seed);
			case 18: return std::ref(default_player_permission_level);
			case 19: return std::ref(texturepack_required);
			case 20: return std::ref(content_log_file_enabled);
			case 21: return std::ref(content_log_console_output_enabled);
			case 22: return std::ref(content_log_level);
			case 23: return std::ref(compression_threshold);
			case 24: return std::ref(compression_algorithm);
			case 25: return std::ref(server_authoritative_movement_strict);
			case 26: return std::ref(server_authoritative_dismount_strict);
			case 27: return std::ref(server_authoritative_entity_interactions_strict);
			case 28: return std::ref(player_position_acceptance_threshold);
			case 29: return std::ref(player_movement_action_direction_threshold);
			case 30: return std::ref(server_authoritative_block_breaking_pick_range_scalar);
			case 31: return std::ref(chat_restriction);
			case 32: return std::ref(disable_player_interaction);
			case 33: return std::ref(client_side_chunk_generation_enabled);
			case 34: return std::ref(block_network_ids_are_hashes);
			case 35: return std::ref(disable_persona);
			case 36: return std::ref(disable_custom_skins);
			case 37: return std::ref(server_build_radius_ratio);
			case 38: return std::ref(allow_outbound_script_debugging);
			case 39: return std::ref(allow_inbound_script_debugging);
			case 40: return std::ref(force_inbound_debug_port);
			case 41: return std::ref(script_debugger_auto_attach);
			case 42: return std::ref(script_debugger_auto_attach_connect_address);
			case 43: return std::ref(script_debugger_auto_attach_timeout);
			case 44: return std::ref(script_debugger_passcode);
			case 45: return std::ref(script_watchdog_enable);
			case 46: return std::ref(script_watchdog_enable_exception_handling);
			case 47: return std::ref(script_watchdog_enable_shutdown);
			case 48: return std::ref(script_watchdog_hang_exception);
			case 49: return std::ref(script_watchdog_hang_threshold);
			case 50: return std::ref(script_watchdog_spike_threshold);
			case 51: return std::ref(script_watchdog_slow_threshold);
			case 52: return std::ref(script_watchdog_memory_warning);
			case 53: return std::ref(script_watchdog_memory_limit);
			case 54: return std::ref(diagnostics_capture_auto_start);
			case 55: return std::ref(diagnostics_capture_max_files);
			case 56: return std::ref(diagnostics_capture_max_file_size);
			case 57: return std::ref(disable_client_vibrant_visuals);
			case 58: return std::ref(sentry_rate_limit_window);
			case 59: return std::ref(sentry_max_events_per_window);
			case 60: return std::ref(enable_profiler);
			case 61: return std::ref(enable_editor_network_metrics);
			case 62: return std::ref(level_type);
			case 63: return std::ref(server_authoritative_block_breaking);
			case 64: return std::ref(emit_server_telemetry);
			case 65: return std::ref(isHardcore);
			case 66: return std::ref(language);
			case 67: return std::ref(op_permission_level);
			case 68: return std::ref(netease_support);
			case 69: return std::ref(only_netease);
			default: throw std::out_of_range("invalid index");
		}
	}
} spd;

class Server {
	private:
		bool error = false;		// Running Error
		ServerProperties data;

		void LoadWorlds();
		void LoadConfig();
		void LoadPlugins();

		bool EnsureDirExists(const std::string& path) {
			if (!fs::exists(path)) {
				errno = 0;
				mkdirat(AT_FDCWD, path.c_str(), S_IRWXU);
				if (errno != 0) {
					log::error("Failed to create directory " + path + ". Because " + (std::string)strerror(errno));
					this->error = true; return false;
				}
				log::info("Successfully create directory " + path);
			}
			return true;
		}
	public:
		Server();
		bool GetError();
		
};

#define DEFAULT_CONFIG_CONTENT "{\"server-name\":\"Dedicated Server\",\"gamemode\":\"survival\",\"force-gamemode\":false,\"difficulty\":\"easy\",\"allow-cheats\":false,\"max-players\":10,\"online-mode\":true,\"allow-list\":true,\"server-port\":19132,\"server-portv6\":19133,\"transport\":\"nethernet\",\"enable-lan-visibility\":true,\"view-distance\":32,\"tick-distance\":4,\"player-idle-timeout\":30,\"max-threads\":8,\"level-name\":\"Bedrock level\",\"level-seed\":\"\",\"default-player-permission-level\":\"member\",\"texturepack-required\":false,\"content-log-file-enabled\":false,\"content-log-console-output-enabled\":false,\"content-log-level\":\"info\",\"compression-threshold\":1,\"compression-algorithm\":\"zlib\",\"server-authoritative-movement-strict\":false,\"server-authoritative-dismount-strict\":false,\"server-authoritative-entity-interactions-strict\":false,\"player-position-acceptance-threshold\":0.5,\"player-movement-action-direction-threshold\":0.85,\"server-authoritative-block-breaking-pick-range-scalar\":1.5,\"chat-restriction\":\"None\",\"disable-player-interaction\":false,\"client-side-chunk-generation-enabled\":true,\"block-network-ids-are-hashes\":true,\"disable-persona\":false,\"disable-custom-skins\":false,\"server-build-radius-ratio\":\"Disabled\",\"allow-outbound-script-debugging\":false,\"allow-inbound-script-debugging\":false,\"script-debugger-auto-attach\":\"disabled\",\"server-ip\":\"\",\"server-udp-ports\":\"\",\"allow-player-joining\":true,\"force-inbound-debug-port\":19144,\"script-debugger-listen-address\":\"127.0.0.1\",\"script-debugger-auto-attach-connect-address\":\"localhost:19144\",\"script-debugger-auto-attach-timeout\":0,\"script-debugger-passcode\":\"\",\"script-watchdog-enable\":true,\"script-watchdog-enable-exception-handling\":true,\"script-watchdog-enable-shutdown\":true,\"script-watchdog-hang-exception\":true,\"script-watchdog-hang-threshold\":10000,\"script-watchdog-spike-threshold\":100,\"script-watchdog-slow-threshold\":10,\"script-watchdog-memory-warning\":100,\"script-watchdog-memory-limit\":250,\"diagnostics-capture-auto-start\":false,\"diagnostics-capture-max-files\":5,\"diagnostics-capture-max-file-size\":2097152,\"disable-client-vibrant-visuals\":true,\"sentry-rate-limit-window\":60,\"sentry-max-events-per-window\":10,\"enable-profiler\":true,\"enable-editor-network-metrics\":true,\"convert-world-to-editor-project\":false}"
#define DEFAULT_CONFIG_SIZE 2213