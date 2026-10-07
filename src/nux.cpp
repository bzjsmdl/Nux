#include "include/cdlib.h"
#include "include/rt.h"
#include "include/server.h"
#include "include/console.h"

std::string plugins_path;
std::vector<std::string> plugins;
Table* t = nullptr;

int main(int argc, const char** argv) {
	t = new Table();
	t->data_path = t->ServerRootDir + "/data/"; t->plugins_path = t->ServerRootDir + "/plugins/";
	for (int i = 0; i < argc; i++) {
		if (strequ(argv[i], "--load-plugins")) t->LoadPlugin = true;
		else if (strequ("-debug", argv[i])) t->debug = true;
		else if (strequ("-werr", argv[i])) t->werr = true;
		else if (strequ("-v", argv[i]) || strequ("--version", argv[i])) printf("Nux Version - alpha 1.0.0\n");
	}
	Server server = Server();
	return server.GetError() ? 1 : 0;
}