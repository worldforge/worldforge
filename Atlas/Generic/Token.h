// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_GENERIC_TOKEN_H
#define ATLAS_GENERIC_TOKEN_H

namespace Atlas { namespace Generic {

/** A simple class that can be used in template class members to identify a
 * particular type.
 *
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
template<typename T> class Token { };

} } // namespace Atlas::Generic

#endif
