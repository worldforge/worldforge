
// C++ Interface: ConfigBoundLogObserver
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBERConfigBoundLogObserver_H
#define EMBERConfigBoundLogObserver_H

#include "services/config/ConfigListenerContainer.h"
#include <spdlog/spdlog.h>

namespace Ember {

class ConfigService;

/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 *
 * @brief A logging observer which is bound to the configuration and changes behaviour in reaction to changed configuration values.
 *
 */
class ConfigBoundLogObserver
{
public:
    explicit ConfigBoundLogObserver(spdlog::sink_ptr sink);

    ~ConfigBoundLogObserver() = default;;

protected:

	ConfigListenerContainer mConfigListener;
	
};

}

#endif
