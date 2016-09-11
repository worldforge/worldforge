// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Mercator/ShaderFactory.h>

#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>

#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

using Mercator::GrassShader;
using Mercator::DepthShader;
using Mercator::FillShader;
using Mercator::HighShader;
using Mercator::LowShader;
using Mercator::BandShader;

int main()
{
    Mercator::ShaderFactories & factories = Mercator::ShaderFactories::instance();

    Mercator::Shader::Parameters params;
    Mercator::Shader * shader;

    //////////////////////// Grass Shader //////////////////////////////

    shader = factories.newShader("grass", params);
    GrassShader * grass_shader = dynamic_cast<GrassShader *>(shader);
    assert(grass_shader != 0);
    assert(grass_shader->lowThreshold() == GrassShader::default_lowThreshold);
    assert(grass_shader->highThreshold() == GrassShader::default_highThreshold);
    assert(grass_shader->cutoff() == GrassShader::default_cutoff);
    assert(grass_shader->intercept() == GrassShader::default_intercept);

    params[GrassShader::key_lowThreshold] = 2.f;
    params[GrassShader::key_highThreshold] = 40.f;
    params[GrassShader::key_cutoff] = 2.f;
    params[GrassShader::key_intercept] = 3.f;
    shader = factories.newShader("grass", params);
    grass_shader = dynamic_cast<GrassShader *>(shader);
    assert(grass_shader != 0);
    assert(grass_shader->lowThreshold() == 2.f);
    assert(grass_shader->highThreshold() == 40.f);
    assert(grass_shader->cutoff() == 2.f);
    assert(grass_shader->intercept() == 3.f);

    params.clear();

    //////////////////////// Depth Shader //////////////////////////////

    shader = factories.newShader("depth", params);
    DepthShader * depth_shader = dynamic_cast<DepthShader *>(shader);
    assert(depth_shader != 0);
    assert(depth_shader->waterLevel() == DepthShader::default_waterLevel);
    assert(depth_shader->murkyDepth() == DepthShader::default_murkyDepth);

    params[DepthShader::key_waterLevel] = 1.f;
    params[DepthShader::key_murkyDepth] = -40.f;
    shader = factories.newShader("depth", params);
    depth_shader = dynamic_cast<DepthShader *>(shader);
    assert(depth_shader != 0);
    assert(depth_shader->waterLevel() == 1.f);
    assert(depth_shader->murkyDepth() == -40.f);

    params.clear();

    //////////////////////// Fill Shader //////////////////////////////

    shader = factories.newShader("fill", params);
    assert(dynamic_cast<FillShader *>(shader) != 0);

    params.clear();

    //////////////////////// High Shader //////////////////////////////

    shader = factories.newShader("high", params);
    HighShader * high_shader = dynamic_cast<HighShader *>(shader);
    assert(high_shader != 0);
    assert(high_shader->threshold() == HighShader::default_threshold);

    params[HighShader::key_threshold] = 2.f;
    shader = factories.newShader("high", params);
    high_shader = dynamic_cast<HighShader *>(shader);
    assert(high_shader != 0);
    assert(high_shader->threshold() == 2.f);

    params.clear();

    //////////////////////// Low Shader //////////////////////////////

    shader = factories.newShader("low", params);
    LowShader * low_shader = dynamic_cast<LowShader *>(shader);
    assert(low_shader != 0);
    assert(low_shader->threshold() == LowShader::default_threshold);

    params[LowShader::key_threshold] = -2.f;
    shader = factories.newShader("low", params);
    low_shader = dynamic_cast<LowShader *>(shader);
    assert(low_shader != 0);
    assert(low_shader->threshold() == -2.f);

    params.clear();

    //////////////////////// Band Shader //////////////////////////////

    shader = factories.newShader("band", params);
    BandShader * band_shader = dynamic_cast<BandShader *>(shader);
    assert(band_shader != 0);
    assert(band_shader->lowThreshold() == BandShader::default_lowThreshold);
    assert(band_shader->highThreshold() == BandShader::default_highThreshold);

    params[BandShader::key_lowThreshold] = -2.f;
    params[BandShader::key_highThreshold] = 2.f;
    shader = factories.newShader("band", params);
    band_shader = dynamic_cast<BandShader *>(shader);
    assert(band_shader != 0);
    assert(band_shader->lowThreshold() == -2.f);
    assert(band_shader->highThreshold() == 2.f);

    params.clear();

    //////////////////////// No Shader //////////////////////////////

    shader = factories.newShader("", params);
    assert(shader == 0);

    params.clear();

    shader = factories.newShader("unknown_shader", params);
    assert(shader == 0);

    return 0;
}
