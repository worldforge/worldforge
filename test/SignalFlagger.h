// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef ERIS_TEST_SIGNAL_FLAGGER_H
#define ERIS_TEST_SIGNAL_FLAGGER_H

#include <sigc++/trackable.h>

class SignalFlagger : public sigc::trackable {
  protected:
    bool m_emitted;

  public:
    SignalFlagger() : m_emitted(false) { }

    void set() { m_emitted = true; }

    bool flagged() const { return m_emitted; }
};

#endif // ERIS_TEST_SIGNAL_FLAGGER_H
