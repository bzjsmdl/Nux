#include "include/server.h"

using namespace nlohmann;

std::mutex LogMutex;

static constexpr std::array<const char*, 72> options = {"server-name", "gamemode", "force-gamemode", "difficulty", "allow-cheats", "max-players", "online-mode", "allow-list", "server-port", "server-portv6", "enable-lan-visibility", "view-distance", "tick-distance", "player-idle-timeout", "allow-player-joining", "max-threads", "level-name", "level-seed", "default-player-permission-level", "texturepack-required", "content-log-file-enabled", "content-log-console-output-enabled", "content-log-level", "compression-threshold", "compression-algorithm", "server-authoritative-movement-strict", "server-authoritative-dismount-strict", "server-authoritative-entity-interactions-strict", "player-position-acceptance-threshold", "player-movement-action-direction-threshold", "server-authoritative-block-breaking-pick-range-scalar", "chat-restriction", "disable-player-interaction", "client-side-chunk-generation-enabled", "block-network-ids-are-hashes", "disable-persona", "disable-custom-skins", "server-build-radius-ratio", "allow-outbound-script-debugging", "allow-inbound-script-debugging", "force-inbound-debug-port", "script-debugger-auto-attach", "script-debugger-auto-attach-connect-address", "script-debugger-auto-attach-timeout", "script-debugger-passcode", "script-watchdog-enable", "script-watchdog-enable-exception-handling", "script-watchdog-enable-shutdown", "script-watchdog-hang-exception", "script-watchdog-hang-threshold", "script-watchdog-spike-threshold", "script-watchdog-slow-threshold", "script-watchdog-memory-warning", "script-watchdog-memory-limit", "diagnostics-capture-auto-start", "diagnostics-capture-max-files", "diagnostics-capture-max-file-size", "disable-client-vibrant-visuals", "sentry-rate-limit-window", "sentry-max-events-per-window", "enable-profiler", "enable-editor-network-metrics", "level-type", "server-authoritative-block-breaking", "emit-server-telemetry", "isHardcore", "language", "op-permission-level", "netease-support", "only-netease"};

Server::Server() {
	log::info("Server started");

	std::thread t1(&Server::LoadConfig, this);
	std::thread t2(&Server::LoadPlugins, this);
	t1.join(); t2.join(); 

	if (this->error) goto end;
	LoadWorlds();
	end:
		log::info("Server shut down");
}

bool Server::GetError() {
	return this->error;
}

void Server::LoadPlugins() {
	if (t->LoadPlugin && !fs::exists(t->plugins_path)) {
		errno = 0;
		mkdirat(AT_FDCWD, t->plugins_path.c_str(), S_IRWXU | S_IROTH);
		if (errno != 0) {
			log::error("Failed to create directory " + t->plugins_path + ". Because " + (std::string)strerror(errno));
			this->error = true; return;
		}
		log::info("Successfully create directory " + t->plugins_path);
	}
	
}

void Server::LoadWorlds() {
	std::string worlds_path = t->data_path + "worlds/",\
				player_path = t->data_path + "players/";

	this->EnsureDirExists(worlds_path);
	this->EnsureDirExists(player_path);
}

void Server::LoadConfig() {
	nux::Pool LocalPool(MB(8));
	FILE* hd_config = (FILE*)LocalPool.allocate(sizeof(FILE*));

	std::string config_file = t->data_path + "server.json";
	
	this->EnsureDirExists(t->data_path);

	json config;
	if (!fs::exists(config_file)) {
		hd_config = fopen(config_file.c_str(), "wb");
		if (errno != 0 && hd_config == nullptr) {
			log::error("Failed to create file " + config_file + ". Because " + (std::string)strerror(errno));
			this->error = true;	return;
		}
		log::info("Successfully create file " +  config_file);
	}
	else {
		hd_config = fopen(config_file.c_str(), "rb");
		if (errno != 0 && hd_config == nullptr) {
			log::error("Failed to open file " + config_file + ". Because " + (std::string)strerror(errno));
			this->error = true;	return;
		}
		log::info("Successfully open file " +  config_file);
	}
	if (sfs::is_empty(config_file)) {
		log::info("Try to write default config into " +  config_file);
		size_t ret = fwrite(DEFAULT_CONFIG_CONTENT, 1, DEFAULT_CONFIG_SIZE, hd_config);
		if (ret != DEFAULT_CONFIG_SIZE) {
			if (ferror(hd_config)) CONFIG_PARSE_FAIL("CRT: " + (std::string)(strerror(errno)))
			else CONFIG_PARSE_FAIL("Expected " + std::to_string(DEFAULT_CONFIG_SIZE) + " elements, but got " + std::to_string(ret) + " elements")
			this->error = true; return;
		}
		
		// parse file
		try {
			config = json::parse((std::string)DEFAULT_CONFIG_CONTENT);
		}
		catch (const std::exception& e) {
			CPPRT_THROW_EXCEPTION_FAIL("\"" + e.what() + "\"");
			this->error = true; return;
		}
	}
	else {
		log::info("Try to read file " +  config_file);
		size_t len = fs::flen(hd_config);
		char* text = (char*)LocalPool.allocate(len + 1); text[len] = 0;
		// I think this function don't check return value because I want to fast finish it.
		fread(text, 1, len, hd_config);
		// but writing a comment is best in here
		// 是的是的

		// parse file
		try {
			if (!text) printf("null\n");
			config = json::parse((std::string)text);
		}
		catch (const std::exception& e) {
			CPPRT_THROW_EXCEPTION_FAIL("\"" + e.what() + "\"");
			this->error = true; return;
		}
	}
	fclose(hd_config);

	// load into memory
	for (size_t i = 0; i < options.size() && !config.empty(); i++) {
		try {
			std::string key = options[i];
			if (config.contains(key)) {
				if (key == "gamemode" && config[key].is_number_integer()) {
					switch ((int)config[key]) {
						case 0: this->data[i] = "survival"; break;
						case 1: this->data[i] = "creative"; break;
						case 2: this->data[i] = "adventure"; break;
						case 5: this->data[i] = "default"; break;
						default: {
							if (t->werr) {
								CONFIG_PARSE_FAIL("`gamemode` must be not " + std::to_string((int)config[options[i]]) + " (it can only be 0, 1, 2 or 5)");
								this->error = true; return;
							}
							else CONFIG_PARSE_WARN("`gamemode` must be not " + std::to_string((int)config[options[i]]) + " (it can only be 0, 1, 2 or 5)\nserver used the default value \"survival\"(0)");
						}
					}
				}
				else if (options[i] == "difficulty" && config[options[i]].is_number_integer()) {
					switch ((int)config[options[i]]) {
						case 0: this->data[i] = "peaceful"; break;
						case 1: this->data[i] = "easy"; break;
						case 2: this->data[i] = "normal"; break;
						case 3: this->data[i] = "hard"; break;
						default: {
							if (t->werr) {
								CONFIG_PARSE_FAIL("`difficulty` must be not " + std::to_string((int)config[options[i]]) + " (it can only be 0, 1, 2 or 3)");
								this->error = true; return;
							}
							else CONFIG_PARSE_WARN("`difficulty` must be not " + std::to_string((int)config[options[i]]) + " (it can only be 0, 1, 2 or 3)\nserver used the default value \"easy\"(1)");
						}
					}
				}
				else if (key == "server-build-radius-ratio" && config[key].is_number_float()) {
					this->data.server_build_radius_ratio = std::to_string((float)config[key]);
				}
				else this->data[i] = config[options[i]];
			}
		}
		catch (const std::exception& e) {
			CPPRT_THROW_EXCEPTION_FAIL("\"" + e.what() + "\"");
			this->error = true; return;
		}	
	}
}