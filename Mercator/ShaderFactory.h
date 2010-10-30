// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_SHADER_FACTORY_H
#define MERCATOR_SHADER_FACTORY_H

#include <Mercator/Shader.h>

namespace Mercator {

/// \brief Interface for shader factory objects.
class ShaderKit {
  protected:
    explicit ShaderKit();
  public:
    virtual ~ShaderKit();

    /// \brief Create a new Shader instance
    ///
    /// @return a pointer to the new Shader.
    virtual Shader * newShader(const Shader::Parameters &) const = 0;
};

/// \brief Factory template used to create ordinary shader objects.
template <class T>
class ShaderFactory : public ShaderKit {
  public:
    ShaderFactory();
    ~ShaderFactory();

    virtual Shader * newShader(const Shader::Parameters &) const;
};

/// \brief Class which manages all the shader factories available.
///
/// Requests for new shaders of a given type are passed here, and
/// shader objects are returned.
class ShaderFactories {
  private:
    /// \brief STL map of shader factory pointers.
    typedef std::map<std::string, ShaderKit *> FactoryMap;

    /// \brief Map of shader factory pointers keyed on type string.
    FactoryMap m_factories;

    /// \brief Pointer to the singleton instance of ShaderFactories.
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
