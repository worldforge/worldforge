// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubCourse_custom.h file.

#ifndef STUB_PHYSICS_COURSE_H
#define STUB_PHYSICS_COURSE_H

#include "physics/Course.h"
#include "stubCourse_custom.h"

#ifndef STUB_Course_isEqualTo
//#define STUB_Course_isEqualTo
  template <typename dim,typename int,typename class,typename PathT>
  bool Course<dim,int,class,PathT>::isEqualTo(const Course& c, WFMath::CoordType epsilon ) const
  {
    return false;
  }
#endif //STUB_Course_isEqualTo

#ifndef STUB_Course_Course
//#define STUB_Course_Course
  template <typename dim,typename int,typename class,typename PathT>
   Course<dim,int,class,PathT>::Course()
  {
    
  }
#endif //STUB_Course_Course

#ifndef STUB_Course_Course
//#define STUB_Course_Course
  template <typename dim,typename int,typename class,typename PathT>
   Course<dim,int,class,PathT>::Course(const Course& l)
  {
    
  }
#endif //STUB_Course_Course

#ifndef STUB_Course_Course
//#define STUB_Course_Course
  template <typename dim,typename int,typename class,typename PathT>
   Course<dim,int,class,PathT>::Course(const PathT<dim> & l)
  {
    
  }
#endif //STUB_Course_Course

#ifndef STUB_Course_Course_DTOR
//#define STUB_Course_Course_DTOR
  template <typename dim,typename int,typename class,typename PathT>
   Course<dim,int,class,PathT>::~Course()
  {
    
  }
#endif //STUB_Course_Course_DTOR

#ifndef STUB_Course_toAtlas
//#define STUB_Course_toAtlas
  template <typename dim,typename int,typename class,typename PathT>
  WFMath::AtlasOutType Course<dim,int,class,PathT>::toAtlas() const
  {
    return *static_cast<WFMath::AtlasOutType*>(nullptr);
  }
#endif //STUB_Course_toAtlas

#ifndef STUB_Course_fromAtlas
//#define STUB_Course_fromAtlas
  template <typename dim,typename int,typename class,typename PathT>
  void Course<dim,int,class,PathT>::fromAtlas(const WFMath::AtlasInType & a)
  {
    
  }
#endif //STUB_Course_fromAtlas

#ifndef STUB_Course_rotateCorner
//#define STUB_Course_rotateCorner
  template <typename dim,typename int,typename class,typename PathT>
  Course& Course<dim,int,class,PathT>::rotateCorner(const WFMath::RotMatrix<dim>& m, size_t corner)
  {
    return *static_cast<Course*>(nullptr);
  }
#endif //STUB_Course_rotateCorner

#ifndef STUB_Course_rotateCenter
//#define STUB_Course_rotateCenter
  template <typename dim,typename int,typename class,typename PathT>
  Course& Course<dim,int,class,PathT>::rotateCenter(const WFMath::RotMatrix<dim>& m)
  {
    return *static_cast<Course*>(nullptr);
  }
#endif //STUB_Course_rotateCenter

#ifndef STUB_Course_rotatePoint
//#define STUB_Course_rotatePoint
  template <typename dim,typename int,typename class,typename PathT>
  Course& Course<dim,int,class,PathT>::rotatePoint(const WFMath::RotMatrix<dim>& m, const WFMath::Point<dim>& p)
  {
    return *static_cast<Course*>(nullptr);
  }
#endif //STUB_Course_rotatePoint

#ifndef STUB_Course_boundingBox
//#define STUB_Course_boundingBox
  template <typename dim,typename int,typename class,typename PathT>
  WFMath::AxisBox<dim> Course<dim,int,class,PathT>::boundingBox() const
  {
    return *static_cast<WFMath::AxisBox<dim>*>(nullptr);
  }
#endif //STUB_Course_boundingBox

#ifndef STUB_Course_boundingSphere
//#define STUB_Course_boundingSphere
  template <typename dim,typename int,typename class,typename PathT>
  WFMath::Ball<dim> Course<dim,int,class,PathT>::boundingSphere() const
  {
    return *static_cast<WFMath::Ball<dim>*>(nullptr);
  }
#endif //STUB_Course_boundingSphere

#ifndef STUB_Course_boundingShapeSloppy
//#define STUB_Course_boundingShapeSloppy
  template <typename dim,typename int,typename class,typename PathT>
  WFMath::Ball<dim> Course<dim,int,class,PathT>::boundingShapeSloppy() const
  {
    return *static_cast<WFMath::Ball<dim>*>(nullptr);
  }
#endif //STUB_Course_boundingShapeSloppy


#endif