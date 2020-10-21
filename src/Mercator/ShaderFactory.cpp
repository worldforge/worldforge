// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "ShaderFactory_impl.h"

#include "DepthShader.h"
#include "FillShader.h"
#include "GrassShader.h"
#include "ThresholdShader.h"

#include <cassert>

namespace Mercator {

ShaderKit::ShaderKit() = default;

ShaderKit::~ShaderKit() = default;

ShaderFactories::ShaderFactories()
{
	m_factories.emplace("grass", std::make_unique<ShaderFactory<GrassShader>>());
	m_factories.emplace("depth", std::make_unique<ShaderFactory<DepthShader>>());
	m_factories.emplace("fill", std::make_unique<ShaderFactory<FillShader>>());
	m_factories.emplace("high", std::make_unique<ShaderFactory<HighShader>>());
	m_factories.emplace("low", std::make_unique<ShaderFactory<LowShader>>());
	m_factories.emplace("band", std::make_unique<ShaderFactory<BandShader>>());
}

ShaderFactories::~ShaderFactories() = default;


/// \brief Create a shader of the specified type.
///
/// @param type a string giving the type of shader.
/// @param params a map of the parameters for the shader
/// @returns a pointer to the new shader object
std::unique_ptr<Shader> ShaderFactories::newShader(const std::string & type,
                                    const Shader::Parameters & params) const
{
    auto I = m_factories.find(type);
    if (I == m_factories.end()) {
        return nullptr;
    }
    assert(I->second);
    return I->second->newShader(params);
}

} // namespace Mercator
