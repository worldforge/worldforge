/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_CYPY_ENTITYLOCATION_H
#define CYPHESIS_CYPY_ENTITYLOCATION_H

#include "rules/EntityLocation.h"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
template<typename EntityT, typename PythonEntityT>
class CyPy_EntityLocation : public WrapperBase<EntityLocation<EntityT>, CyPy_EntityLocation<EntityT, PythonEntityT>> {
public:
	CyPy_EntityLocation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

	CyPy_EntityLocation(Py::PythonClassInstance* self, EntityLocation<EntityT> value);

	static void init_type();

	Py::Object getattro(const Py::String&) override;

	int setattro(const Py::String& name, const Py::Object& attr) override;

//        Py::Object repr() override;
//
//        virtual Py::Object number_subtract(const Py::Object&);
private:

	Py::Object copy();

};


#endif //CYPHESIS_CYPY_ENTITYLOCATION_H
