// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_TASK_H
#define ATLAS_TASK_H

namespace Atlas {

/** Atlas task

FIXME document this class

*/

class Task
{
    public:

    virtual void Poll() = 0;
    virtual void Run() = 0;
};

} // Atlas namespace

#endif
