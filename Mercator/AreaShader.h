#ifndef MERCATOR_AREASHADER_H
#define MERCATOR_AREASHADER_H

#include <Mercator/Shader.h>

namespace Mercator
{

class Area;

class AreaShader : public Shader
{
public:
    AreaShader(int layer);
    
    virtual void shade(Surface &s) const;
    
    virtual bool checkIntersect(Surface &) const;
private:
    /// helper to shader a single area into the surface
    void shadeArea(Surface& s, const Area* const s) const;
    
    int m_layer;
};

}

#endif // of MERCATOR_AREASHADER_H
