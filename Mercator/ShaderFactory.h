// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_SHADER_FACTORY_H
#define MERCATOR_SHADER_FACTORY_H

#include <Mercator/Shader.h>

namespace Mercator {

class iShaderFactory {
  protected:
    explicit iShaderFactory();
  public:
    virtual ~iShaderFactory();

    /// \brief Create a new Shader instance
    ///
    /// @return a pointer to the new Shader.
    virtual Shader * newShader(const Shader::Parameters &) const = 0;
};

template <class T>
class ShaderFactory : public iShaderFactory {
  public:
    ShaderFactory();
    ~ShaderFactory();

    virtual Shader * newShader(const Shader::Parameters &) const;
};

class ShaderFactories {
  private:
    typedef std::map<std::string, iShaderFactory *> FactoryMap;

    FactoryMap m_factories;

    static ShaderFactories * m_instance;

    ShaderFactories();
    ~ShaderFactories();
  public:
    static ShaderFactories & instance();
    static void del();

    Shader * newShader(const std::string & type,
                       const Shader::Parameters &) const;
};

} // namespace Mercator

#endif // MERCATOR_SHADER_FACTORY_H
