#ifndef ERIS_LOG_H
#define ERIS_LOG_H

#include <sigc++/signal.h>
#include <spdlog/logger.h>
#include <string>

namespace Eris {
extern std::shared_ptr<spdlog::logger> logger;
}

#include "LogStream.h"

#endif
