#pragma once

#include "pch.h"
#include <iostream>
#include "lib.h"
#include "spdlog/sinks/base_sink.h"

template<typename Mutex>
class sink : public spdlog::sinks::base_sink <Mutex>
{
public:

	void sink_it_(const spdlog::details::log_msg& in_msg) override
	{
		LogMsg msg{};
		std::string payload(in_msg.payload.data(), in_msg.payload.size());
		msg.level = (int)in_msg.level;
		msg.msg = payload.c_str();
		msg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(in_msg.time.time_since_epoch()).count();
		msg.source.file = in_msg.source.filename;
		msg.source.func = in_msg.source.funcname;
		msg.source.line = in_msg.source.line;
		msg.pluginHandle = g_pPlugin->handle;
		g_pPlugin->logger(&msg);
	}

	void flush_() override
	{
		std::cout << std::flush;
	}

protected:
	// sink log level - default is all
	spdlog::level_t level_{ spdlog::level::trace };
};

using my_sink = sink<std::mutex>;