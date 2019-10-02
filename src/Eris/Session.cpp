/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Session.h"
#include "EventService.h"

#include <boost/asio/io_service.hpp>

namespace Eris {

Session::Session()
		: m_io_service(new boost::asio::io_service()),
		  m_event_service(new EventService(*m_io_service)) {
}

Session::~Session() {
	delete m_event_service;
	delete m_io_service;
}

boost::asio::io_service& Session::getIoService() {
	return *m_io_service;
}

EventService& Session::getEventService() {
	return *m_event_service;
}


}
