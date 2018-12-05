#ifndef PATH_H
#define PATH_H

#include <vector>
#include <Urho3D/Core/Spline.h>
#include "Curve.h"

enum PathType
{
    EPT_CONSTANT,
    EPT_LINEAR,
    EPT_CUBIC,
    EPT_AKIMA,
    EPT_BESSEL,
    EPT_PCHIP,
    EPT_QUINTIC,
};

/*
class Spline
{
public:

    Spline(PathType type)
    {
        Urho3D::InterpolationMode LOOKUP_INTERPOLATION_MODE[] =
        {
            Urho3D::LINEAR_CURVE,
            Urho3D::LINEAR_CURVE,
            Urho3D::BEZIER_CURVE,
            Urho3D::LINEAR_CURVE,
            Urho3D::CATMULL_ROM_CURVE,
            Urho3D::CATMULL_ROM_FULL_CURVE
        };

        Urho3D::InterpolationMode mode = LOOKUP_INTERPOLATION_MODE[EPT_LINEAR];
        _spline.SetInterpolationMode(Urho3D::LINEAR_CURVE);
    }

    double eval(double d)
    {
        return _spline.GetPoint(d).GetDouble();
    }

    void clear()
    {
        _spline.Clear();
    }

    void build(double* times, double* values, size_t count)
    {
        Urho3D::Vector<Urho3D::Variant> knots;
        for (int i=0; i < count; ++i)
            knots.Push(values[i]);

        _spline.Clear();
        _spline.SetKnots(knots);
    }

private:
    Urho3D::Spline _spline;
};*/


class Spline
{
public:

    Spline(PathType type) :
        _spline(nullptr),
        _type(EPT_LINEAR)
    {

    }

    ~Spline()
    {
        if(_spline)
            delete _spline;
    }

    double eval(double d)
    {
        assert(_spline);

        float dst;
        _spline->evaluate(d, &dst);
        return dst;
    }

    void clear()
    {
    }

    void build(double* times, double* values, size_t count)
    {
        if(_spline)
            delete _spline;
        _spline = new Curve(count, 1);

        for (size_t i=0; i < count; ++i)
        {
            float keyValues = values[i];
            _spline->setPoint(i, times[i], &keyValues, Curve::InterpolationType::LINEAR);
        }
    }

private:
    Curve* _spline;
    PathType _type;
};



enum PathLoopMode
{
    ELM_LOOP,
    ELM_ZERO,
    ELM_LAST,
};

struct PathKey
{
    double time;
    double value;
};

class Path
{
public:
    Path(PathType splineType = EPT_LINEAR, unsigned int keyCount = 0);
    ~Path();

    void build();
    double evaluate(double time) const;
    void addKey(double keyTime, double keyValue);
    void setAtIndex(unsigned index, double time, double value);
    void setLoopMode(PathLoopMode loopMode);
    void clear();

    unsigned int getKeyCount() const;
    const PathKey getKeyByIndex(unsigned int index) const;
    double getStartTime() const;
    double getEndTime() const;

private:
    void _insertKey(double keyTime, double keyValue);

private:
    std::vector<double> _times;
    std::vector<double> _values;
    Spline* _spline;
    PathLoopMode _loopMode;
};



class Path4
{
public:
    struct Float4
    {
        double x;
        double y;
        double z;
        double w;
    };

    Float4 evaluate(double time) const;
    const Path4& getSubPath(unsigned int index) const;
    Path4& getSubPath(unsigned int index);

private:
    Path _subPaths[4];
};

class PathSampler
{

};

class Path4Sampler
{

};

#endif // PATH_H
