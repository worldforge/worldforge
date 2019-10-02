// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_SHADER_FACTORY_IMPL_H
#define MERCATOR_SHADER_FACTORY_IMPL_H

#include "ShaderFactory.h"

namespace Mercator {

template <class T>
ShaderFactory<T>::ShaderFactory()
{
}

template <class T>
ShaderFactory<T>::~ShaderFactory()
{
}

template <class T>
Shader * ShaderFactory<T>::newShader(const Shader::Parameters & params) const
{
    return new T(params);
}

} // namespace Mercator

#endif // MERCATOR_SHADER_FACTORY_IMPL_H
