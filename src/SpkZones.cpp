#include "SpkZones.h"

//--------------------------------------------------------------------------------------------
// base spark zone
//--------------------------------------------------------------------------------------------

void NodeSparkZoneBase::createBaseZoneParams(eString name, bool shared)
{
    // create base SPKObject parameters
    createBaseObjectParams(name,shared);

    // create base Zone parameters
    PARAM_FXYZ("Position", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 0.0f);
}

void NodeSparkZoneBase::setBaseZoneParams(SPK::Ref<SPK::Zone> zone)
{
    if(zone)
    {
        // get parameters
        SPK::Vector3D position = ToSpkVector3D(getParameter("Position")->getValueAsFXYZ());

        // set zone parameters
        zone->setPosition(position);

        // set base SPKObject parameters
        setBaseObjectParams(zone);
    }
}

void NodeSparkZoneBase::setResult(SPK::Ref<SPK::Zone> zone)
{
    _zone.reset();
    _zone = zone;
}


//------------------------------------------------------------------------------------------------------------------------------
// zone point node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkZonePoint::NodeSparkZonePoint()
{
    OUT_PORT(EPT_ZONE, "zone");

    createBaseZoneParams("PointZone");
}

void NodeSparkZonePoint::process()
{
    SPK::Ref<SPK::Point> zonePoint = SPK::Point::create();
    setBaseZoneParams(zonePoint);
    setResult(zonePoint);
}


//------------------------------------------------------------------------------------------------------------------------------
// plane zone node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkZonePlane::NodeSparkZonePlane()
{
    OUT_PORT(EPT_ZONE, "zone");

    createBaseZoneParams("PlaneZone");
    PARAM_FXYZ("Normal", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
}

void NodeSparkZonePlane::process()
{
    SPK::Vector3D normal = ToSpkVector3D(getParameter("Normal")->getValueAsFXYZ());

    SPK::Ref<SPK::Plane> zonePlane = SPK::Plane::create();
    zonePlane->setNormal(normal);
    setBaseZoneParams(zonePlane);
    setResult(zonePlane);
}


//------------------------------------------------------------------------------------------------------------------------------
// sphere zone node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkZoneSphere::NodeSparkZoneSphere()
{
    OUT_PORT(EPT_ZONE, "zone");

    createBaseZoneParams("SphereZone");
    PARAM_FLOAT("Radius", 0.0f, eF32_MAX, 1.0f);
}

void NodeSparkZoneSphere::process()
{
    float radius = getParameter("Radius")->getValueAsFloat();

    SPK::Ref<SPK::Sphere> zoneSphere = SPK::Sphere::create();
    zoneSphere->setRadius(radius);
    setBaseZoneParams(zoneSphere);
    setResult(zoneSphere);
}


//------------------------------------------------------------------------------------------------------------------------------
// box zone node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkZoneBox::NodeSparkZoneBox()
{
    OUT_PORT(EPT_ZONE, "zone");

    createBaseZoneParams("BoxZone");
    PARAM_FXYZ("Dimensions", eF32_MIN, eF32_MAX, 1.0f, 1.0f, 1.0f);
    PARAM_FXYZ("Front", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 1.0f);
    PARAM_FXYZ("Up", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
}

void NodeSparkZoneBox::process()
{
    SPK::Vector3D dimensions = ToSpkVector3D(getParameter("Dimensions")->getValueAsFXYZ());
    SPK::Vector3D front = ToSpkVector3D(getParameter("Front")->getValueAsFXYZ());
    SPK::Vector3D up = ToSpkVector3D(getParameter("Up")->getValueAsFXYZ());

    SPK::Ref<SPK::Box> zoneBox = SPK::Box::create();
    zoneBox->setDimensions(dimensions);
    zoneBox->setAxis(front, up);
    setBaseZoneParams(zoneBox);
    setResult(zoneBox);
}

