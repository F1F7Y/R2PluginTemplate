#include "plugin.h"

#include "pch.h"
#include "lib.h"

void Plugin::Main() {}
void Plugin::DLLLoadEngine() {
	RegisterConCommand("cplug_test", [](const CCommand& command){ spdlog::info("Gaming"); }, "", 0);
}
void Plugin::DLLLoadClient() {}
void Plugin::DLLLoadServer() {}