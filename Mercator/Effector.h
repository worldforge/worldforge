// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2010 Alistair Riddoch

#ifndef MERCATOR_EFFECTOR_H
#define MERCATOR_EFFECTOR_H

#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>

namespace Mercator
{

class Segment;
class Shader;

/// \brief Device which effects a change in the terrain
///
/// Classes which inherit from this interface change the terrain in
/// some way within the region given by the box member. The exact
/// shape of the area affected and the nature of the change depends
/// on the subclass.
class Effector
{
  public:
    class Context {
      public:
        Context();

        virtual ~Context();

        const std::string & id() { return m_id; }

        void setId(const std::string &);
      protected:
        std::string m_id;
    };

    Context * context() const { return m_context; }

    void setContext(Context *);

    /// Accessor for the bounding box of the geometric shape.
    const WFMath::AxisBox<2> & bbox() const
    {
        return m_box;
    }

    virtual ~Effector() = 0;

    virtual bool checkIntersects(const Segment& s) const = 0;

    virtual int addToSegment(Segment &) const = 0;
    virtual void updateToSegment(Segment &) const = 0;
    virtual void removeFromSegment(Segment &) const = 0;
    
  protected:
    /// \brief Constructor
    Effector();

    /// \brief Copy constructor
    Effector(const Effector &);

    /// \brief Assignment
    Effector & operator=(const Effector &);

    /// The bounding box of the geometric shape.
    WFMath::AxisBox<2> m_box;
    
    /// The application context of this effector
    Context * m_context;
};

}

#endif // of MERCATOR_EFFECTOR_H
