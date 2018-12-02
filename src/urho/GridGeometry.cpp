#include "GridGeometry.h"
#include "../UrhoDevice.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Material.h>

using namespace Urho3D;

GridGeometry::GridGeometry() :
    _gridgeometry(nullptr),
    _gridNode(nullptr)
{
    _gridColor = Color(0.5f, 0.5f, 0.5f);
    _gridSubdivisionColor = Color(0.2f, 0.2f, 0.2f);
    _gridXColor = Color(1.0f, 0.1f, 0.1f);
    _gridYColor = Color(0.1f, 1.0f, 0.1f);
    _gridZColor = Color(0.1f, 0.5f, 1.0f);
    _showGrid = true;
    _grid2DMode = false;
}

void GridGeometry::HideGrid()
{
    if (_gridgeometry != NULL)
        _gridNode->SetEnabled(false);
}

void GridGeometry::ShowGrid()
{
    if (_gridgeometry != NULL)
        _gridNode->SetEnabled(true);
}

void GridGeometry::CreateGrid(Scene* scene)
{
    ResourceCache* cache = UrhoDevice::getInstance()->GetSubsystem<ResourceCache>();
    if (!_gridNode)
    {
        _gridNode = new Node(UrhoDevice::gUrhoContext);
        _gridgeometry = _gridNode->CreateComponent<CustomGeometry>();
        _gridgeometry->SetNumGeometries(1);
        _gridgeometry->SetMaterial(cache->GetResource<Material>("Materials/VColUnlit.xml"));
        _gridgeometry->SetViewMask(0x80000000); // Editor raycasts use viewmask 0x7fffffff
        _gridgeometry->SetOccludee(false);

        if (scene->GetComponent<Octree>() != NULL)
            scene->GetComponent<Octree>()->AddManualDrawable(_gridgeometry);
    }
    UpdateGrid();
}

void GridGeometry::UpdateGrid(bool updateGridGeometry)
{
    _showGrid ? ShowGrid() : HideGrid();
    _gridNode->SetScale(Vector3(8.0f, 8.0f, 8.0f));

    if (!updateGridGeometry)
        return;

    unsigned int size = (unsigned int)(floor(8.0f / 2.0f) * 2.0f);
    float halfSizeScaled = size / 2.0f;
    float scale = 1.0f;
    unsigned int subdivisionSize = (unsigned int)(pow(2.0f, 3.0f));

    if (subdivisionSize > 0)
    {
        size *= subdivisionSize;
        scale /= subdivisionSize;
    }

    unsigned int halfSize = size / 2;

    _gridgeometry->BeginGeometry(0, LINE_LIST);
    float lineOffset = -halfSizeScaled;
    for (unsigned int i = 0; i <= size; ++i)
    {
        bool lineCenter = i == halfSize;
        bool lineSubdiv = !Equals(float(i% subdivisionSize), 0.0f);

        if (!_grid2DMode)
        {
            _gridgeometry->DefineVertex(Vector3(lineOffset, 0.0, halfSizeScaled));
            _gridgeometry->DefineColor(lineCenter ? _gridZColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
            _gridgeometry->DefineVertex(Vector3(lineOffset, 0.0, -halfSizeScaled));
            _gridgeometry->DefineColor(lineCenter ? _gridZColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));

            _gridgeometry->DefineVertex(Vector3(-halfSizeScaled, 0.0, lineOffset));
            _gridgeometry->DefineColor(lineCenter ? _gridXColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
            _gridgeometry->DefineVertex(Vector3(halfSizeScaled, 0.0, lineOffset));
            _gridgeometry->DefineColor(lineCenter ? _gridXColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
        }
        else
        {
            _gridgeometry->DefineVertex(Vector3(lineOffset, halfSizeScaled, 0.0));
            _gridgeometry->DefineColor(lineCenter ? _gridYColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
            _gridgeometry->DefineVertex(Vector3(lineOffset, -halfSizeScaled, 0.0));
            _gridgeometry->DefineColor(lineCenter ? _gridYColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));

            _gridgeometry->DefineVertex(Vector3(-halfSizeScaled, lineOffset, 0.0));
            _gridgeometry->DefineColor(lineCenter ? _gridXColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
            _gridgeometry->DefineVertex(Vector3(halfSizeScaled, lineOffset, 0.0));
            _gridgeometry->DefineColor(lineCenter ? _gridXColor : (lineSubdiv ? _gridSubdivisionColor : _gridColor));
        }

        lineOffset += scale;
    }
    _gridgeometry->Commit();
}
