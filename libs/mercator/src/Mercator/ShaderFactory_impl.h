// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_SHADER_FACTORY_IMPL_H
#define MERCATOR_SHADER_FACTORY_IMPL_H

#include "ShaderFactory.h"

namespace Mercator {

template<class T>
ShaderFactory<T>::ShaderFactory() = default;

template<class T>
ShaderFactory<T>::~ShaderFactory() = default;

template<class T>
std::unique_ptr<Shader> ShaderFactory<T>::newShader(const Shader::Parameters& params) const {
	return std::make_unique<T>(params);
}

} // namespace Mercator

#endif // MERCATOR_SHADER_FACTORY_IMPL_H
