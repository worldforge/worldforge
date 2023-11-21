#ifndef ERIS_LOG_H
#define ERIS_LOG_H

#include <sigc++/signal.h>
#include <spdlog/logger.h>
#include <string>

namespace Eris {

inline static std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("eris");;

}

#include "LogStream.h"

#endif
