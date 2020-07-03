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

ShaderFactories * ShaderFactories::m_instance = nullptr;

ShaderFactories::ShaderFactories()
{
    m_factories["grass"] = new ShaderFactory<GrassShader>;
    m_factories["depth"] = new ShaderFactory<DepthShader>;
    m_factories["fill"] = new ShaderFactory<FillShader>;
    m_factories["high"] = new ShaderFactory<HighShader>;
    m_factories["low"] = new ShaderFactory<LowShader>;
    m_factories["band"] = new ShaderFactory<BandShader>;
}

ShaderFactories::~ShaderFactories() = default;

/// \brief Construct a singleton instance if required.
///
/// @returns a reference to the instance.
ShaderFactories & ShaderFactories::instance()
{
    if (!m_instance) {
        m_instance = new ShaderFactories();
    }
    return *m_instance;
}

/// \brief Delete the instance
void ShaderFactories::del()
{
    delete m_instance;
}

/// \brief Create a shader of the specified type.
///
/// @param type a string giving the type of shader.
/// @param params a map of the parameters for the shader
/// @returns a pointer to the new shader object
Shader * ShaderFactories::newShader(const std::string & type,
                                    const Shader::Parameters & params) const
{
    auto I = m_factories.find(type);
    if (I == m_factories.end()) {
        return nullptr;
    }
    assert(I->second != 0);
    return I->second->newShader(params);
}

} // namespace Mercator
