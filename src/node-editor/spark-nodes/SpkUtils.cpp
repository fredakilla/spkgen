#include "SpkUtils.h"


SPK::Vector3D ToSpkVector3D(const eFXYZ& v)
{
    return SPK::Vector3D(v.x, v.y, v.z);
}

Urho3D::Vector3 ToUrhoVector3(const SPK::Vector3D& v)
{
    return Urho3D::Vector3(v.x, v.y, v.z);
}

SPK::Color ToSpkColor(const eColor& c)
{
    return SPK::Color(c.r, c.g, c.b, c.a);
}
