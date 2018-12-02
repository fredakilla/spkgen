#ifndef GRIDGEOMETRY_H
#define GRIDGEOMETRY_H

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/CustomGeometry.h>


class GridGeometry
{
public:
    GridGeometry();    
    void HideGrid();
    void ShowGrid();
    void CreateGrid(Urho3D::Scene *scene);
    void UpdateGrid(bool updateGridGeometry = true);

private:
    Urho3D::SharedPtr<Urho3D::Node> _gridNode;
    Urho3D::SharedPtr<Urho3D::CustomGeometry> _gridgeometry;
    bool _showGrid;
    bool _grid2DMode;
    Urho3D::Color _gridColor;
    Urho3D::Color _gridSubdivisionColor;
    Urho3D::Color _gridXColor;
    Urho3D::Color _gridYColor;
    Urho3D::Color _gridZColor;
};

#endif // GRIDGEOMETRY_H
