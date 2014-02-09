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

#ifndef SESSION_H_
#define SESSION_H_

namespace boost
{
namespace asio
{
class io_service;
}
}

namespace Eris
{

class EventService;

class Session
{
public:
    Session();
    virtual ~Session();

    boost::asio::io_service& getIoService();
    EventService& getEventService();

protected:
    boost::asio::io_service* m_io_service;
    EventService* m_event_service;
};

}
#endif /* SESSION_H_ */
