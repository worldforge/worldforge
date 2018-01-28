// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#ifndef ATLAS_TASK_H
#define ATLAS_TASK_H

namespace Atlas {

/* Atlas task

A task can be polled. Currently, Codecs and Negotiates are both Tasks. This is
a rather minimal interface and may perhaps fade away.

@see Codec
@see Negotiate
*/

class Task
{
    public:

    virtual void poll(bool can_get) = 0;
};

} // Atlas namespace

#endif
