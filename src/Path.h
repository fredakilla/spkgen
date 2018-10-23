#ifndef PATH_H
#define PATH_H

#include "thirdparty/splines/Splines.hh"
using namespace SplinesLoad;

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
    Path(PathType splineType, unsigned int keyCount = 0);
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
    Splines::Spline* _spline;
    PathLoopMode _loopMode;
};

class Path4
{

};

class PathSampler
{

};

class Path4Sampler
{

};

#endif // PATH_H
