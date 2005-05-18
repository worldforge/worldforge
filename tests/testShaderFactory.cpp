// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Mercator/ShaderFactory.h>

#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>

int main()
{
    Mercator::ShaderFactories & factories = Mercator::ShaderFactories::instance();

    Mercator::Shader::Parameters params;
    Mercator::Shader * shader;

    shader = factories.newShader("grass", params);
    assert(dynamic_cast<Mercator::GrassShader *>(shader) != 0);

    shader = factories.newShader("depth", params);
    assert(dynamic_cast<Mercator::DepthShader *>(shader) != 0);

    shader = factories.newShader("fill", params);
    assert(dynamic_cast<Mercator::FillShader *>(shader) != 0);

    shader = factories.newShader("high", params);
    assert(dynamic_cast<Mercator::HighShader *>(shader) != 0);

    shader = factories.newShader("low", params);
    assert(dynamic_cast<Mercator::LowShader *>(shader) != 0);

    shader = factories.newShader("band", params);
    assert(dynamic_cast<Mercator::BandShader *>(shader) != 0);

    shader = factories.newShader("", params);
    assert(shader == 0);

    shader = factories.newShader("unknown_shader", params);
    assert(shader == 0);

    return 0;
}
