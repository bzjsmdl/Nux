#if !defined(__linux__) 
	#error "Nux targets Linux only. Please use Linux environment"
#endif
// 					Nux - A Minecraft Bedrock Server
//				  (This's really a big big big updating)
//							LICENSE: AGPL 
// ======================================================================
// |				PART 1			Include Libraries					|
// ======================================================================
	// Include Started Library
	#include <bits/stdc++.h>	// Only GNU? I don't know.
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/epoll.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <assert.h>
	#include <stddef.h>
	#include <alloca.h>
	#include <immintrin.h>
	#include <netdb.h>
	#include <fcntl.h>

	// Include Thrid-Party Library
	// get more information: `include/README.md`!
	#include "include/zconf.h"
	#include "include/zlib.h"
	#include "include/json.hpp"


// ======================================================================
// |				PART 2			Some Marcos							|
// ======================================================================
	typedef struct stat status;
	typedef unsigned char byte;

	// `sfs` 是 `std::filesystem` 的缩写 (I can't speak English well)
	namespace sfs = std::filesystem;

	using namespace nlohmann;

	#define strequ(s, S) (strcmp(s, S) == 0)

	#define KB(n) (1024 * n)
	#define MB(n) (KB(1024) * n)
	#define GB(n) (MB(1024) * n)

	#define DEFAULT_CONFIG_CONTENT "{\"server-name\":\"Dedicated Server\",\"gamemode\":\"survival\",\"force-gamemode\":false,\"difficulty\":\"easy\",\"allow-cheats\":false,\"max-players\":10,\"online-mode\":true,\"allow-list\":true,\"server-port\":19132,\"server-portv6\":19133,\"transport\":\"nethernet\",\"enable-lan-visibility\":true,\"view-distance\":32,\"tick-distance\":4,\"player-idle-timeout\":30,\"max-threads\":8,\"level-name\":\"Bedrock level\",\"level-seed\":\"\",\"default-player-permission-level\":\"member\",\"texturepack-required\":false,\"content-log-file-enabled\":false,\"content-log-console-output-enabled\":false,\"content-log-level\":\"info\",\"compression-threshold\":1,\"compression-algorithm\":\"zlib\",\"server-authoritative-movement-strict\":false,\"server-authoritative-dismount-strict\":false,\"server-authoritative-entity-interactions-strict\":false,\"player-position-acceptance-threshold\":0.5,\"player-movement-action-direction-threshold\":0.85,\"server-authoritative-block-breaking-pick-range-scalar\":1.5,\"chat-restriction\":\"None\",\"disable-player-interaction\":false,\"client-side-chunk-generation-enabled\":true,\"block-network-ids-are-hashes\":true,\"disable-persona\":false,\"disable-custom-skins\":false,\"server-build-radius-ratio\":\"Disabled\",\"allow-outbound-script-debugging\":false,\"allow-inbound-script-debugging\":false,\"script-debugger-auto-attach\":\"disabled\",\"server-ip\":\"\",\"server-udp-ports\":\"\",\"allow-player-joining\":true,\"force-inbound-debug-port\":19144,\"script-debugger-listen-address\":\"127.0.0.1\",\"script-debugger-auto-attach-connect-address\":\"localhost:19144\",\"script-debugger-auto-attach-timeout\":0,\"script-debugger-passcode\":\"\",\"script-watchdog-enable\":true,\"script-watchdog-enable-exception-handling\":true,\"script-watchdog-enable-shutdown\":true,\"script-watchdog-hang-exception\":true,\"script-watchdog-hang-threshold\":10000,\"script-watchdog-spike-threshold\":100,\"script-watchdog-slow-threshold\":10,\"script-watchdog-memory-warning\":100,\"script-watchdog-memory-limit\":250,\"diagnostics-capture-auto-start\":false,\"diagnostics-capture-max-files\":5,\"diagnostics-capture-max-file-size\":2097152,\"disable-client-vibrant-visuals\":true,\"sentry-rate-limit-window\":60,\"sentry-max-events-per-window\":10,\"enable-profiler\":true,\"enable-editor-network-metrics\":true,\"convert-world-to-editor-project\":false}"
	#define DEFAULT_CONFIG_SIZE 2213

	// Visibility Flag Macro
	// they are only "tip" in namepspace
	#define pub
	#define priv
	#define prot

	// Error Info
	#define ALLOCATE_MEMORY_FAIL(debug_msg) {\
		nux::log::error("Server failed when it tries to allocate memory");\
		if (t->debug) nux::log::debug(debug_msg);\
	}

	#define CANNOT_OPEN_FILE(file, debug_msg) {\
		nux::log::error((std::string)"Server failed when it tries to open file " + file);\
		if (t->debug) nux::log::debug(debug_msg);\
	}

	#define CPPRT_THROW_EXCEPTION_FAIL(msg) {\
		nux::log::error((std::string)"Server failed because " + msg);\
	}

	#define CONFIG_PARSE_WARN(debug_msg) {\
		nux::log::warning((std::string)"Server threw a warning when it tried to parse config file");\
		if (t->debug) nux::log::debug(debug_msg);\
	}

	#define CONFIG_PARSE_FAIL(msg) {\
		nux::log::error((std::string)"Server failed because " + msg);\
	}

	#define CLIENT_SERVER_PROTOCOL_VERSION_NOT_MATCH_FAIL() {\
		nux::log::error((std::string)"Server failed because client's and server's protocol version don't match");\
	}

	// Bedrock RakNet Marco
	#define ID_UNCONNECTED_PING 0x1
	#define ID_UNCONNECTED_PING_OPEN_CONNECTIONS 0x2

// ======================================================================
// |				PART 2			Constants							|
// ======================================================================
	const byte BEDROCK_RAKNET_OFFLINE_MESSAGE_DATA_ID[16] = {0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78}; 
	const byte NULL_ADDRESS[7] = {0x04, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};

// ======================================================================
// |				PART 3		Some Structions							|
// ======================================================================
	struct Event {
		enum Type {
			SendAck,
			SendNack
		} type;
		enum ClientVersion {
			OldBedrockMinecraft,	// RakNet
			NewBedrockMinecraft,	// NetherNet
			NeteaseBedrockMinecraft
		} clientVersion;
		byte* RawPacket;
		size_t RawPacketSize;
	};
	typedef struct NuxTable {
		bool debug = false;
		bool color = isatty(fileno(stdout));
		bool LoadPlugin = false;
		bool werr = false;
		bool ipv6 = false;
		bool security = false;
		bool encryption = false;
		std::string plugins_path;
		std::string data_path;
		std::string ServerRootDir = getcwd(nullptr, 0);
		std::vector<std::string> plugins;
	} Table;

	// get more information about keys of `ServerProperties`:
	// - [English](https://minecraft.wiki/w/Server.properties)
	// - [中文](https://zh.minecraft.wiki/w/%E6%9C%8D%E5%8A%A1%E7%AB%AF%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6%E6%A0%BC%E5%BC%8F)
	struct ServerProperties {
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
	};

// ======================================================================
// |				PART 4		Global Variable							|
// ======================================================================
	// Global Table
	Table* t = nullptr;

	constexpr std::array<const char*, 70> options = {"server-name", "gamemode", "force-gamemode", "difficulty", "allow-cheats", "max-players", "online-mode", "allow-list", "server-port", "server-portv6", "enable-lan-visibility", "view-distance", "tick-distance", "player-idle-timeout", "allow-player-joining", "max-threads", "level-name", "level-seed", "default-player-permission-level", "texturepack-required", "content-log-file-enabled", "content-log-console-output-enabled", "content-log-level", "compression-threshold", "compression-algorithm", "server-authoritative-movement-strict", "server-authoritative-dismount-strict", "server-authoritative-entity-interactions-strict", "player-position-acceptance-threshold", "player-movement-action-direction-threshold", "server-authoritative-block-breaking-pick-range-scalar", "chat-restriction", "disable-player-interaction", "client-side-chunk-generation-enabled", "block-network-ids-are-hashes", "disable-persona", "disable-custom-skins", "server-build-radius-ratio", "allow-outbound-script-debugging", "allow-inbound-script-debugging", "force-inbound-debug-port", "script-debugger-auto-attach", "script-debugger-auto-attach-connect-address", "script-debugger-auto-attach-timeout", "script-debugger-passcode", "script-watchdog-enable", "script-watchdog-enable-exception-handling", "script-watchdog-enable-shutdown", "script-watchdog-hang-exception", "script-watchdog-hang-threshold", "script-watchdog-spike-threshold", "script-watchdog-slow-threshold", "script-watchdog-memory-warning", "script-watchdog-memory-limit", "diagnostics-capture-auto-start", "diagnostics-capture-max-files", "diagnostics-capture-max-file-size", "disable-client-vibrant-visuals", "sentry-rate-limit-window", "sentry-max-events-per-window", "enable-profiler", "enable-editor-network-metrics", "level-type", "server-authoritative-block-breaking", "emit-server-telemetry", "isHardcore", "language", "op-permission-level", "netease-support", "only-netease"};

	enum ReliabilityType {
		Unreliable,
		UnreliableSequenced,
		Reliable,	
		ReliableOrdered,
		ReliableSequenced,
		UnreliableAndAckReceipt,
		ReliableAndAckReceipt,
		ReliableOrderedAndAckReceipt,
	};

// ======================================================================
// |				PART 5			Utils Function						|
// ======================================================================
	namespace nux::fmt {
		static std::mutex FmtMutex;
		/*	$ + command_charater
				-- Color Text --
					Black	0
					Red		1
					Green	2
					Yellow	3
					Blue	4
					Purple	5
					Cyan	6
					White	7
					Default	9
				---- Contorl Text ----
					Strong      	S
				StrikeThrough   	s
					Underline   	u
				Reset All States	r
			*/
		void print(std::string __msg) {
			std::lock_guard<std::mutex> lock(FmtMutex);
			unsigned long long int len = __msg.size();
			std::string msg;
			bool strong = false, strikethrough = false, underline = false, bg = false;
			unsigned int color_code = 9;
			for (unsigned long long int i = 0; i < len; i++) {
				if (__msg[i] == '$') {
					switch (__msg[++i]) {
						case 'S':
							strong = true;
							break;
						case 's':
							strikethrough = true;
							break;
						case 'u':
							underline = true;
							break;	
						case 'r':
							msg.append("\x1b[0m");
							continue;
						default:
							char tmp_str[2] = {__msg[i], 0};
							if (isdigit(__msg[i])) color_code = atoi(tmp_str);
							color_code += ((strong) ? 90 : 30);
							if (__msg[i + 1] == 'b') color_code += 10;
					}
					if (__msg[i + 1] != '$') {
						msg.append("\x1b[");
						if (strong) msg += "1;";
						if (underline) msg += "4;";
						if (strikethrough) msg += "9;";
						msg += (std::to_string(color_code) + "m");
					}
				}
				else if (__msg[i] == '\\') {
					switch (__msg[++i]) {
						case '\\':
							msg.append("\\");
							break;
						case 'n':
							msg.append("\n");
							break;
						case '\'':
							msg.append("\'");
							break;
						case '\"':
							msg.append("\"");
							break;
						case 'a':
							msg.append("\a");
							break;
						case 'r':
							msg.append("\r");
							break;
						case 'v':
							msg.append("\v");
							break;
						case 't':
							msg.append("\t");
							break;
						case 'b':
							msg.append("\b");
							break;
						case '$':
							msg.append("$");
							break;
					}
				}
				else msg += __msg[i];
			}
			printf("%s", msg.c_str());
		}
		std::string input(std::string tip) {
			printf("%s", tip.c_str());
			std::string msg;
			std::getline(std::cin, msg);
			return msg;
		}

		std::istream& input(std::string tip, std::string& msg) {
			printf("%s", tip.c_str());
			return std::getline(std::cin, msg);
		}
		inline void show_bytes(byte* __ptr, size_t __len) {
			for (size_t i = 0; i < __len; i++) printf(" %.2X", __ptr[i]);
		}
	}

	namespace nux::fs {
		inline bool exists(std::string p) {
			status st;
			return stat(p.c_str(), &st) == 0;
		}
		inline bool isdir(std::string p) {
			status st;
			return (stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) ? true : false;
		}
		inline size_t flen(FILE* f) {
			size_t nfp = ftell(f);
			fseek(f, 0, SEEK_END);
			size_t len = ftell(f);
			fseek(f, nfp, SEEK_SET);
			return len;
		}
	}

	namespace nux::log {
		static std::mutex LogMutex;

		// don't use it!!! we assume it's private!!!
		priv static std::string time_format() {
			time_t __time = time(nullptr);
			tm *_time = localtime(&__time);
			std::string time_str;
			if (_time) {
				std::string sec = std::to_string(_time->tm_sec),
							min = std::to_string(_time->tm_min),
							hour = std::to_string(_time->tm_hour);
				if (sec.length() == 1) sec.insert(sec.begin(), '0');
				if (min.length() == 1) min.insert(min.begin(), '0');
				if (hour.length() == 1) hour.insert(hour.begin(), '0');
				time_str += std::to_string(_time->tm_year + 1900) + '-' + std::to_string(_time->tm_mon + 1) + '-' + std::to_string(_time->tm_mday) + ' ' + hour + ':' + min + ':' + sec;
			}
			else {
				time_str += "XXXX-X-X XX:XX:XX";
			}
			return time_str;
		}

		// don't use it!!! we assume it's private!!!
		priv static void no_color(std::string& msg) {
			for (size_t i = 0; i < msg.size(); i++) {
				if (msg[i] == '$' && ((i != 0 && msg[i - 1] != '\\') || i == 0)) {
					msg.erase(msg.begin() + i, msg.begin() + i + 2);
					i--;
				}
			}
		}

		void debug(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			std::string msg = "$3$S" + time_format() + "$6 [DE2BUG]$r " + __msg + "\n";
			if (!t->color) no_color(msg);
			nux::fmt::print(msg);
		}

		void info(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			std::string msg = "$3$S" + time_format() + "$4 [INFO]$r " + __msg + "\n";
			if (!t->color) no_color(msg);
			nux::fmt::print(msg);
		}

		void error(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			std::string msg = "$3$S" + time_format() + "$1 [ERROR]$r " + __msg + "\n";
			if (!t->color) no_color(msg);
			nux::fmt::print(msg);
		}

		void warning(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			std::string msg = "$3$S" + time_format() + "$5 [WARNING]$r " + __msg + "\n";
			if (!t->color) no_color(msg);
			nux::fmt::print(msg);
		}
	}

	namespace nux::math {
		inline uint64_t randint(uint64_t min = 0, uint64_t max = (uint64_t)-1) {
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<> dis(min, max);
			return dis(gen);
		}

		// Generate UUID
		inline std::string uuid() {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, 15),\
											dis2(8, 11);
			std::string ss;
			for (int i = 0; i < 8; i++) ss += dis(gen);
			ss += "-";
			for (int i = 0; i < 4; i++) ss += dis(gen);
			ss += "-";
			for (int i = 0; i < 3; i++) ss += dis(gen);
			ss += "-";
			ss += dis2(gen);
			for (int i = 0; i < 3; i++) ss += dis(gen);
			ss += "-";
			for (int i = 0; i < 12; i++) ss += dis(gen);
			
			return ss;
		}
	}

	namespace nux {
		class Pool {
				// Memory Pool
				private:
				void* ptr = nullptr;
				size_t len = 1024;
				size_t _used = 0;
				bool trust = true;
				bool child = false;

				public:
				Pool(size_t N) : len(N) {
					this->len = ((N >> 6) + 1) * 64;
					this->ptr = malloc(this->len);
				}

				Pool() {
					this->ptr = malloc(this->len);
				}

				Pool(void* __ptr, size_t __len) : ptr(__ptr), len(__len), child(true) {}

				Pool(const Pool&) = delete;
				Pool& operator=(const Pool&) = delete;

				void* allocate(size_t __size) {
					// you can go to https://github.com/bzjsmdl/Nux/blob/main/src/nux.cpp#L594 if you want to know why i write `__size > this->len - this->used`
					if (__size > this->len - this->_used) return nullptr;
					void* ret = (char*)this->ptr + this->_used;
					this->_used += __size;
					return ret;
				}

				void* data() {
					this->trust = false;
					return this->ptr;
				}

				size_t size() { return this->len; }
				
				size_t used() { return this->_used; }

				void clear() {
					memset(this->ptr, 0, (this->trust) ? this->_used : this->len);
					this->_used = 0;
				}

				void clear(size_t start) {
					memset((byte*)this->ptr + start, 0, ((this->trust) ? this->_used : this->len));
					this->_used = 0;
				}

				~Pool() {
					if (!child) free(ptr);
				}
		};
	}
	
	// # don't use it currently
	// namespace nux {
	// 	class Reader {
	// 			private:
	// 				const uint8_t* data;
	// 				size_t pool_size;
	// 				size_t pos = 0;

	// 				template <typename T>
	// 				T read(size_t size) {
	// 					// check the range of reading.
	// 					// if use `this->pos + size >= this->pool_size`:
	// 					// if the size of pool is 8, this memory's layout: |0|1|2|3|4|5|6|7|
	// 					// if this->pos == 0, read<int>() => sizeof(int) = 4, this->pos + 4 = 4
	// 					// and if this->pos == -4, read<int>() => sizeof(int) = 4, this->pos + 4 = 0
	// 					// in this expression, it's valid -- but we expect it's invalid!!!
	// 					// if use `size > this->pool_size - this->pos`
	// 					// when this->pos == 0, read<int>() => sizeof(int) = 4, this->pool_size + this->pos = 8 - 0 = 4 (valid)
	// 					// when this->pos == -4, read<int>() => sizeof(int) = 4, this->pool_size + this->pos = 8 - (-4) = 12 (invalid)
	// 					// because of this, use `size > this->pool_size - this->pos` in here
	// 					if (size > this->pool_size - this->pos) throw ErrorCode::VectorTooSmall;
	// 					T ret;
	// 					memcpy(&ret, (this->data + this->pos), size);

	// 					// i promise it don't throw an error~ 绝对!!!
	// 					seek(size);
	// 					return ret;
	// 				}
	// 			public:
	// 				Reader(Pool& __data, size_t __pos = 0) : pos(__pos) {
	// 					this->pool_size = __data.size();
	// 					this->data = (const uint8_t*)__data.data();
	// 				}

	// 				void seek(signed long int offset) {
	// 					this->pos += offset;
	// 					if ((signed long int)this->pos < 0) throw ErrorCode::UnsignedWrapAround;
	// 					if (this->pos > this->pool_size) throw ErrorCode::VectorTooSmall;
	// 				}

	// 				size_t tell() {
	// 					return this->pos;
	// 				}

	// 				template <typename T>
	// 				T read() {
	// 					return read<T>(sizeof(T));
	// 				}
	// 		};
	// }
		
// ======================================================================
// |				PART 6			Server								|
// ======================================================================
	class Server {
		private:
			// static member
			static std::mutex ServerMutex;

			bool error = false;		// Running Error
			uint64_t server_guid = nux::math::randint();
			ServerProperties data;
			std::atomic<unsigned long long int> tickcnt {0};
			std::atomic<bool> running {false};
			size_t player_cnt = 0;
			byte ProtocolVersion = 0;
			uint32_t ServerIP = 0;
			byte ServerIPv6[16] = {0};

			size_t size = MB(32);
			nux::Pool buffer = nux::Pool(size);

			bool is_cookie = false;
			bool SecurityChallenge = false;

			void run() {
				while (this->running) {
					auto next_time = std::chrono::system_clock::now() + std::chrono::milliseconds(50);
					this->tick();
					std::this_thread::sleep_until(next_time);
				}
				
			}

			void tick() {
				tickcnt++;
			}

			void LoadPlugins() {
				if (t->LoadPlugin) {
					this->EnsureDirExists(t->plugins_path);
				}
			}

			void Network() {
				// std::thread t1();
				BedrockRakNet();
			}

			void InitWorlds() {
				std::string worlds_path = t->data_path + "worlds/",\
							player_path = t->data_path + "players/";

				this->EnsureDirExists(worlds_path);
				this->EnsureDirExists(player_path);
			}

			void LoadConfig() {
				std::string config_file = t->data_path + "server.json";
				
				this->EnsureDirExists(t->data_path);

				json config; FILE* hd_config = nullptr;
				if (!nux::fs::exists(config_file)) {
					hd_config = fopen(config_file.c_str(), "wb");
					if (errno != 0 && hd_config == nullptr) {
						nux::log::error("Failed to create file " + config_file + ". Because " + (std::string)strerror(errno));
						this->error = true;	return;
					}
					nux::log::info("Successfully create file " +  config_file);
				}
				else {
					hd_config = fopen(config_file.c_str(), "rb");
					if (errno != 0 && hd_config == nullptr) {
						nux::log::error("Failed to open file " + config_file + ". Because " + (std::string)strerror(errno));
						this->error = true;	return;
					}
					nux::log::info("Successfully open file " +  config_file);
				}
				if (sfs::is_empty(config_file)) {
					nux::log::info("Try to write default config into " +  config_file);
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
					nux::log::info("Try to read file " +  config_file);
					size_t len = nux::fs::flen(hd_config);
					char* text = (char*)buffer.allocate(len + 1); text[len] = 0;
					// I think this function don't check return value because I want to fast finish it.
					fread(text, 1, len, hd_config);
					// but writing a comment is best in here
					// 是的是的

					// parse file
					try {
						config = json::parse((std::string)text);
					}
					catch (const std::exception& e) {
						CPPRT_THROW_EXCEPTION_FAIL("\"" + e.what() + "\"");
						this->error = true; return;
					}
				}
				fclose(hd_config);
				nux::log::info("Finish parsing config file");

				// load into memory
				for (size_t i = 0; i < options.size() && !config.empty(); i++) {
					try {
						if (config.contains(options[i])) {
							std::string key = options[i];
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
							else if (key == "difficulty" && config[key].is_number_integer()) {
								switch ((int)config[key]) {
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
							else if (config[key].is_null()) {
								CONFIG_PARSE_FAIL(key + "'s value from null is not valid");
								this->error = true; return;
							}
							else this->data[i] = config[key];
						}
					}
					catch (const std::exception& e) {
						CPPRT_THROW_EXCEPTION_FAIL("\"" + e.what() + "\"");
						this->error = true; return;
					}	
				}
			}

			bool EnsureDirExists(const std::string& path) {
				if (!nux::fs::exists(path)) {
					errno = 0;
					mkdirat(AT_FDCWD, path.c_str(), S_IRWXU);
					if (errno != 0) {
						nux::log::error("Failed to create directory " + path + ". Because " + (std::string)strerror(errno));
						this->error = true; return false;
					}
					nux::log::info("Successfully create directory " + path);
				}
				return true;
			}
		public:
			Server() {
				nux::log::info("Server started");
				{
					std::thread t1(&Server::LoadConfig, this);
					std::thread t2(&Server::LoadPlugins, this);
					t1.join(); t2.join();
				}
				

				if (this->error) goto end;
				// InitWorlds();
				
				{
					this->running = true;
					std::thread t1(&Server::run, this);
					std::thread t2(&Server::Network, this);
					t1.join(); t2.join();
				}
				

				end:
					nux::log::info("Server shut down");
			}

			inline const ServerProperties GetServerProperties() {
				return this->data;
			}

			inline uint64_t GetServerGuid() {
				return this->server_guid;
			}

			inline bool GetError() {
				return this->error;
			}

			inline void stop() {
				this->running = false;
			}

			inline size_t GetPlayerCount() {
				return this->player_cnt;
			}

// ======================================================================
// |				PART 7			Network								|
// ======================================================================
		private:
		std::queue<Event> event_queue;
		uint64_t* ClientGuid = nullptr;
		uint32_t* ClientIP = nullptr;
		byte* ClientIPv6;

		sockaddr_in client_in;
		socklen_t clen = sizeof(client_in);
		int listenfd = -1;

		void BedrockRakNet() {
			// 0. initial
			// i'm annoyed and tired... don't support ipv6 currently... (2026.10.18, bzjsmdl)

			// create an arena of this thread
			size_t localsize = MB(4);
			nux::Pool localbuffer = nux::Pool((byte*)buffer.allocate(localsize), MB(4));
			nux::Pool SokectBuffer = nux::Pool((byte*)buffer.allocate(localsize), MB(4));
			byte* ptr = (byte*)SokectBuffer.data();

			// clear `errno`
			errno = 0;

			// other initialization
			static uint64_t cidx;
			sockaddr_in client_in; socklen_t clen = sizeof(client_in);

			epoll_event* events =  (epoll_event*)localbuffer.allocate(sizeof(epoll_event) * 16);
			this->ClientGuid = (uint64_t*)localbuffer.allocate(sizeof(uint64_t) * this->data.max_players);
			this->ClientIP = (uint32_t*)localbuffer.allocate(sizeof(uint32_t) * this->data.max_players);
			this->ClientIPv6 = (byte*)localbuffer.allocate(sizeof(byte) * 16 * this->data.max_players);
			size_t tmp_base_offset = localbuffer.used();
			
			// 1. create a UDP socket and an epoll instance
			// 1.1 create a UDP socket 
			int listenfd = -1, epollfd = -1;
			listenfd = socket((t->ipv6) ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
			if (listenfd == -1) goto err;

			// 1.2 create an epoll instance (I'm so bored...)
			epollfd = epoll_create1(0);
			if (epollfd == -1) goto err;

			nux::log::info("Server has created a UDP socket");

			// 2. bind a fixed port
			client_in.sin_family = (t->ipv6) ? AF_INET6 : AF_INET;
			client_in.sin_port = htons((t->ipv6) ? data.server_portv6 : data.server_port);
			client_in.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(listenfd, (const sockaddr*)&client_in, clen) == -1) goto err;
			nux::log::info("Server has successfully bound port " + std::to_string(((t->ipv6) ? data.server_portv6 : data.server_port)));
			
			{
				int current_flags = fcntl(listenfd, F_GETFL, 0);
				// socket is set as non-blocking
				fcntl(listenfd, F_SETFL, current_flags | O_NONBLOCK);

				// `listenfd` is registered in epoll
				epoll_event ev = *(epoll_event*)localbuffer.allocate(sizeof(epoll_event));
				ev.events = EPOLLIN | EPOLLET;
				// EPOLLIN : read; EPOLLET: ET mode
				ev.data.fd = listenfd;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
			}	
			
			nux::log::info("Try to build connection with client");
			// 4. build connection with client
			
			while (true) {
				int seed = 0; bool first = true;
				ssize_t n = epoll_wait(epollfd, events, 16, 5000);
				if (n == 0) {
					seed = nux::math::randint();
					continue;
				}
				if (n == -1) {
					if (errno == EINTR || errno == EWOULDBLOCK) continue;
					goto err;
				}
				

				for (ssize_t i = 0; i < n; i++) {
					if (events[i].data.fd == listenfd) {
						int current_seed = 0;
						int expected_cookie = 0;
						build_connect: {
							// Receive Client Data
							n = recvfrom(listenfd, ptr, size, 0, (sockaddr*)&client_in, &clen);
							if (n == -1) {
								if (errno == EINTR || errno == EWOULDBLOCK) goto build_connect;
								break;
							}

							uint16_t mtu = 0;
							switch (ptr[0]) {
								case ID_UNCONNECTED_PING_OPEN_CONNECTIONS:
									if (this->GetPlayerCount() >= data.max_players) break;
								case ID_UNCONNECTED_PING: {
									// Build an Unconnected Pong
									ptr[0] = 0x1c;
									*(uint64_t*)(ptr + 1) = server_guid;
									memcpy(ptr + 9, BEDROCK_RAKNET_OFFLINE_MESSAGE_DATA_ID, 16);

									std::string cpy = data.gamemode;
									cpy[0] |= 0x20;
									int gmnum = 0;
									switch (cpy[0]) {
										case 'A':
											gmnum++;
										case 'C':
											gmnum++;
											break;
										case 'S':
											if (cpy[1] == 'u') break;
										default:
											gmnum = -1; 
									}
										
									std::string server_id_str = "MCPE;";
									uint16_t slen = server_id_str.length();
									server_id_str += data.server_name + ";" + "766;1.21.50;" + std::to_string(this->GetPlayerCount()) +\
													";" + std::to_string(data.max_players) + ";" + std::to_string(server_guid) + ";" +\
													data.level_name + ";" + data.gamemode + ";" + ((gmnum == -1) ? "" : std::to_string(gmnum)) +\
													std::to_string(data.server_port) + ";" + std::to_string(data.server_portv6);
									*(uint16_t*)(ptr + 25) = htons(slen);
									strcpy((char*)ptr + 27, server_id_str.c_str());

									// Send to client
									n = sendto(listenfd, ptr, 27 + slen, 0, (sockaddr*)&client_in, clen);
									if (n == -1) goto err;
									break;
								}
								case 0x05: {
									this->ProtocolVersion = ptr[17];

									// start to build
									mtu = n; ptr[0]++;
									*(uint64_t*)(ptr + 17) = this->server_guid;
									ptr[25] = t->security;
									size_t mtu_fbase = 26;
									if (ptr[25]) {
										current_seed = seed; expected_cookie = current_seed ^ client_in.sin_addr.s_addr;
										*(int*)(ptr + mtu_fbase) = seed ^ client_in.sin_addr.s_addr;
										mtu_fbase += 4;
										this->is_cookie = true;
									}
									*(uint16_t*)(ptr + mtu_fbase) = mtu;

									n = sendto(listenfd, ptr, (ptr[25] ? 32 : 28), 0, (struct sockaddr*)&client_in, clen);
									if (n == -1) goto err;
									break;
								}
								case 0x07: {
									size_t ServerAddrBase = 17;
									
									if (is_cookie) {
										this->SecurityChallenge = ptr[21];
										ServerAddrBase = 22;
										if (current_seed ^ client_in.sin_addr.s_addr != expected_cookie) goto end;
									}

									if (ptr[ServerAddrBase] == 0x04) {
										this->ServerIP = ntohl(~(*(uint32_t*)(ptr + ServerAddrBase + 1)));
										this->ClientIP[cidx] = client_in.sin_addr.s_addr;
										// this->ClientIPv6;
										// ;
									}

									this->ClientGuid[cidx] = *(uint64_t*)(ptr + ServerAddrBase + 9); 

									// Start to build `Open Connection Reply 2`
									ptr[0]++; *(uint64_t*)(ptr + 17) = this->server_guid;
									ptr[25] = 0x04; *(uint32_t*)(ptr + 26) = htonl(~(this->ClientIP[cidx]));
									*(uint16_t*)(ptr + 30) = htons(client_in.sin_port);
									*(uint16_t*)(ptr + 34) = mtu; ptr[34] = t->encryption;

									n = sendto(listenfd, ptr, (t->ipv6 ? 57 : 35), 0, (struct sockaddr*)&client_in, clen);
									if (n == -1) goto err;
									break;
								}
								case 0x19: {
									if (memcmp(ptr + 2, BEDROCK_RAKNET_OFFLINE_MESSAGE_DATA_ID, 16) == 0 && *(uint64_t*)(ptr + 18) == this->server_guid) {
										CLIENT_SERVER_PROTOCOL_VERSION_NOT_MATCH_FAIL();
										goto err;
									}
									break;
								}
								default:
									if ((ptr[0] & 0x80) == 0x80 && (ptr[0] & 0x60) == 0) {
										// local variable `reliability_type` equals a
										#define rte(a) (reliability_type == a)
										uint32_t SequenceNumber = 0; memcpy(&SequenceNumber, ptr + 1, 3); 
										byte reliability_type = ptr[4] >> 5; bool is_slice = ptr[4] & 0x10 == 0x10;
										uint16_t len_in_bits = ntohs(*(uint16_t*)(ptr + 5)); byte OrderChannel = 0;
										uint32_t ReliableFrameIndex = 0, SequencedFrameIndex = 0, OrderedFrameIndex = 0;
										size_t pos = 7;
										// uint32_t CompoundSize = 0, SliceIndex = 0;
										// std::unordered_map<uint16_t, uint32_t> Compound;
										// nux::Pool pool = nux::Pool(this->buffer.allocate(MB(8)), MB(8));

										if (rte(ReliabilityType::ReliableSequenced) || rte(ReliabilityType::Reliable) ||\
											rte(ReliabilityType::ReliableAndAckReceipt) || rte(ReliabilityType::ReliableOrderedAndAckReceipt) ||\
											rte(ReliabilityType::ReliableOrdered)
										) {
											memcpy(&ReliableFrameIndex, ptr + pos, 3);
											pos += 3;
										}
										if (rte(ReliabilityType::ReliableSequenced) || rte(ReliabilityType::UnreliableSequenced)) {
											memcpy(&SequencedFrameIndex, ptr + pos, 3);
											pos += 3;
										}
										if (rte(ReliabilityType::ReliableOrdered) || rte(ReliabilityType::ReliableOrderedAndAckReceipt)) {
											memcpy(&OrderedFrameIndex, ptr + pos, 3);
											pos += 3; OrderChannel = ptr[pos];
										}
										if (is_slice) {
											;
										}

										uint32_t tmp_sn = SequenceNumber - 1;
										byte* ack_packet = (byte*)localbuffer.allocate(7);	
										ack_packet[0] = 0xC0; *(uint16_t*)(ack_packet + 1) = 1;
										ack_packet[3] = 0x01; memcpy(ack_packet + 4, &tmp_sn, 3);
										ssize_t n2 = sendto(listenfd, ptr, 7, 0, (struct sockaddr*)&client_in, clen);

										SequenceNumber++;
										memcpy(ptr + 1, &SequenceNumber, 3);

										localbuffer.clear(tmp_base_offset);

										printf("\n\nClient: ");
										nux::fmt::show_bytes(ptr, n);

										switch (ptr[pos]) {
											case 0x09: {
												uint64_t RequestTime = *(uint64_t*)(ptr + pos + 9);

												ptr[pos] = 0x10; ptr[pos + 1] = (t->ipv6 ? 0x6 : 0x4);
												*(uint32_t*)(ptr + pos + 2) = ~this->ClientIP[cidx];
												*(uint16_t*)(ptr + pos + 6) = cidx; *(uint64_t*)(ptr + pos + 78) = RequestTime;

												for (size_t j = 0; j < 10; j++)
													memcpy(ptr + 8 + pos + (7 * j), NULL_ADDRESS, 7);					
												*(uint64_t*)(ptr + pos + 86) = std::chrono::duration_cast<std::chrono::milliseconds>(
													std::chrono::system_clock::now().time_since_epoch()
												).count();

												size_t size = pos + (t->ipv6 ? 335 : 93) + 1;

												*(uint16_t*)(ptr + 5) = htons((size - pos) << 3);

												n = sendto(listenfd, ptr, size, 0, (struct sockaddr*)&client_in, clen);
												if (n == -1) goto err;
												break;
											}
											case 0x00: {	// I don't know it can run here, or cannot run here
												// Receive Connected Ping and Send Connected Pong
												uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
													std::chrono::system_clock::now().time_since_epoch()
												).count();
												*(uint64_t*)(ptr + 9) = now;
												ptr[0] = 0x03;
												n = sendto(listenfd, ptr, 17, 0, (sockaddr*)&client_in, clen);
												if (n == -1) goto err;
												goto build_connect;
											}
											case 0x13: {
												printf("Can Run!\n");
												break;
											}
											default: break;
										}
										#undef rte
									}
							}
							goto build_connect;
							end: ;
						}
					}
				}
			}
			return;
			err:
				this->error = true;
				close(listenfd);
				CPPRT_THROW_EXCEPTION_FAIL("\"" + strerror(errno) + "\"");
				this->stop();
				return;
		}
	};

// ======================================================================
// |				THE LAST PART		Nux Server Entry				|
// ======================================================================
int main(int argc, const char** argv) {
	t = new Table();
	t->data_path = t->ServerRootDir + "/data/"; t->plugins_path = t->ServerRootDir + "/plugins/";
	for (int i = 0; i < argc; i++) {
		if (strequ(argv[i], "--load-plugins")) t->LoadPlugin = true;
		else if (strequ("-debug", argv[i])) t->debug = true;
		else if (strequ("-werr", argv[i])) t->werr = true;
		else if (strequ(argv[i], "--enable-in-network")) {
			i++;
			if (strequ(argv[i], "security")) t->security = true;
			else if (strequ(argv[i], "encryption")) t->encryption = true;
			else if (strequ(argv[i], "ipv6")) t->ipv6 = true;
		}
		else if (strequ("-v", argv[i]) || strequ("--version", argv[i])) printf("Nux Version - alpha 1.0.0\n");
	}
	Server server = Server();
	return server.GetError() ? 1 : 0;
}
