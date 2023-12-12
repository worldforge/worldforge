// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_SHADER_FACTORY_H
#define MERCATOR_SHADER_FACTORY_H

#include "Shader.h"
#include <memory>

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
	virtual std::unique_ptr<Shader> newShader(const Shader::Parameters&) const = 0;
};

/// \brief Factory template used to create ordinary shader objects.
template<class T>
class ShaderFactory : public ShaderKit {
public:
	ShaderFactory();

	~ShaderFactory() override;

	std::unique_ptr<Shader> newShader(const Shader::Parameters&) const override;
};

/// \brief Class which manages all the shader factories available.
///
/// Requests for new shaders of a given type are passed here, and
/// shader objects are returned.
class ShaderFactories {
private:

	/// \brief Map of shader factory pointers keyed on type string.
	std::map<std::string, std::unique_ptr<ShaderKit>> m_factories;

public:
	ShaderFactories();

	~ShaderFactories();

	std::unique_ptr<Shader> newShader(const std::string& type,
									  const Shader::Parameters&) const;
};

} // namespace Mercator

#endif // MERCATOR_SHADER_FACTORY_H
