// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Mercator/ShaderFactory_impl.h>

#include <Mercator/DepthShader.h>
#include <Mercator/FillShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/ThresholdShader.h>

#include <cassert>

namespace Mercator {

ShaderKit::ShaderKit()
{
}

ShaderKit::~ShaderKit()
{
}

ShaderFactories * ShaderFactories::m_instance = 0;

ShaderFactories::ShaderFactories()
{
    m_factories["grass"] = new ShaderFactory<GrassShader>;
    m_factories["depth"] = new ShaderFactory<DepthShader>;
    m_factories["fill"] = new ShaderFactory<FillShader>;
    m_factories["high"] = new ShaderFactory<HighShader>;
    m_factories["low"] = new ShaderFactory<LowShader>;
    m_factories["band"] = new ShaderFactory<BandShader>;
}

ShaderFactories::~ShaderFactories()
{
}

/// \brief Construct a singleton instance if required.
///
/// @returns a reference to the instance.
ShaderFactories & ShaderFactories::instance()
{
    if (m_instance == 0) {
        m_instance = new ShaderFactories();
    }
    return *m_instance;
}

/// \brief Delete the instance
void ShaderFactories::del()
{
    if (m_instance != 0) {
        delete m_instance;
    }
}

/// \brief Create a shader of the specified type.
///
/// @param type a string giving the type of shader.
/// @param params a map of the parameters for the shader
/// @returns a pointer to the new shader object
Shader * ShaderFactories::newShader(const std::string & type,
                                    const Shader::Parameters & params) const
{
    FactoryMap::const_iterator I = m_factories.find(type);
    if (I == m_factories.end()) {
        return 0;
    }
    assert(I->second != 0);
    return I->second->newShader(params);
}

} // namespace Mercator
