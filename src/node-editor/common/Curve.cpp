#include "Curve.h"
#include <cassert>
#include <cstring>
#include <cmath>
#include <memory>

using std::memcpy;
using std::fabs;
using std::sqrt;
using std::cos;
using std::sin;
using std::exp;
using std::strcmp;

#ifndef NULL
#define NULL 0
#endif

#ifndef MATH_PI
#define MATH_PI 3.14159265358979323846f
#endif

#ifndef MATH_PIOVER2 
#define MATH_PIOVER2 1.57079632679489661923f
#endif

#ifndef MATH_PIX2
#define MATH_PIX2 6.28318530717958647693f
#endif

// Object deletion macro
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
    if (x) \
    { \
        delete x; \
        x = NULL; \
    }
#endif

// Array deletion macro
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) \
    if (x) \
    { \
        delete[] x; \
        x = NULL; \
    }
#endif

static inline float bezier(float eq0, float eq1, float eq2, float eq3, float from, float out, float to, float in)
{
    return from * eq0 + out * eq1 + in * eq2 + to * eq3;
}

static inline float bspline(float eq0, float eq1, float eq2, float eq3, float c0, float c1, float c2, float c3)
{
    return c0 * eq0 + c1 * eq1 + c2 * eq2 + c3 * eq3;
}

static inline float hermite(float h00, float h01, float h10, float h11, float from, float out, float to, float in)
{
    return h00 * from + h01 * to + h10 * out + h11 * in;
}

static inline float hermiteFlat(float h00, float h01, float from, float to)
{
    return h00 * from + h01 * to;
}

static inline float hermiteSmooth(float h00, float h01, float h10, float h11, float from, float out, float to, float in)
{
    return h00 * from + h01 * to + h10 * out + h11 * in;
}

static inline float lerpInl(float s, float from, float to)
{
    return from + (to - from) * s;
}



Curve* Curve::create(unsigned int pointCount, unsigned int componentCount)
{
    return new Curve(pointCount, componentCount);
}

Curve::Curve(unsigned int pointCount, unsigned int componentCount)
    : _pointCount(pointCount), _componentCount(componentCount), _componentSize(sizeof(float)*componentCount), _quaternionOffset(NULL), _points(NULL)
{
    _points = new Point[_pointCount];
    for (unsigned int i = 0; i < _pointCount; i++)
    {
        _points[i].time = 0.0f;
        _points[i].value = new float[_componentCount];
        _points[i].inValue = new float[_componentCount];
        _points[i].outValue = new float[_componentCount];
        _points[i].type = LINEAR;
    }
    _points[_pointCount - 1].time = 1.0f;
}

Curve::~Curve()
{
    SAFE_DELETE_ARRAY(_points);
    SAFE_DELETE_ARRAY(_quaternionOffset);
}

Curve::Point::Point()
    : time(0.0f), value(NULL), inValue(NULL), outValue(NULL), type(LINEAR)
{
}

Curve::Point::~Point()
{
    SAFE_DELETE_ARRAY(value);
    SAFE_DELETE_ARRAY(inValue);
    SAFE_DELETE_ARRAY(outValue);
}

unsigned int Curve::getPointCount() const
{
    return _pointCount;
}

unsigned int Curve::getComponentCount() const
{
    return _componentCount;
}

float Curve::getStartTime() const
{
    return _points[0].time;
}

float Curve::getEndTime() const
{
    return _points[_pointCount-1].time;
}

float Curve::getPointTime(unsigned int index) const
{
    assert(index < _pointCount);
    return _points[index].time;
}


Curve::InterpolationType Curve::getPointInterpolation(unsigned int index) const
{
    assert(index < _pointCount);
    return _points[index].type;;
}

void Curve::getPointValues(unsigned int index, float* value, float* inValue, float* outValue) const
{
    assert(index < _pointCount);
    
    if (value)
        memcpy(value, _points[index].value, _componentSize);
    
    if (inValue)
        memcpy(inValue, _points[index].inValue, _componentSize);
    
    if (outValue)
        memcpy(outValue, _points[index].outValue, _componentSize);
}

void Curve::setPoint(unsigned int index, float time, float* value, InterpolationType type)
{
    setPoint(index, time, value, type, NULL, NULL);
}

void Curve::setPoint(unsigned int index, float time, float* value, InterpolationType type, float* inValue, float* outValue)
{
    //assert(index < _pointCount && time >= 0.0f && time <= 1.0f && !(_pointCount > 1 && index == 0 && time != 0.0f) && !(_pointCount != 1 && index == _pointCount - 1 && time != 1.0f));

    assert(index < _pointCount);
    assert(time >= 0.0f);
    //assert(time <= 1.0f);
    assert(!(_pointCount > 1 && index == 0 && time != 0.0f));
    //assert(!(_pointCount != 1 && index == _pointCount - 1 && time != 1.0f));


    _points[index].time = time;
    _points[index].type = type;

    if (value)
        memcpy(_points[index].value, value, _componentSize);

    if (inValue)
        memcpy(_points[index].inValue, inValue, _componentSize);

    if (outValue)
        memcpy(_points[index].outValue, outValue, _componentSize);
}

void Curve::setTangent(unsigned int index, InterpolationType type, float* inValue, float* outValue)
{
    assert(index < _pointCount);

    _points[index].type = type;

    if (inValue)
        memcpy(_points[index].inValue, inValue, _componentSize);

    if (outValue)
        memcpy(_points[index].outValue, outValue, _componentSize);
}

void Curve::evaluate(float time, float* dst) const
{
    assert(dst);

    evaluate(time, 0.0f, 1.0f, 0.0f, dst);
}

void Curve::evaluate(float time, float startTime, float endTime, float loopBlendTime, float* dst) const
{
    assert(dst && startTime >= 0.0f && startTime <= endTime && endTime <= 1.0f && loopBlendTime >= 0.0f);

    // If there's only one point on the curve, return its value.
    if (_pointCount == 1)
    {
        memcpy(dst, _points[0].value, _componentSize);
        return;
    }

    unsigned int min = 0;
    unsigned int max = _pointCount - 1;
    float localTime = time;
    if (startTime > 0.0f || endTime < 1.0f)
    {
        // Evaluating a sub section of the curve
        min = determineIndex(startTime, 0, max);
        max = determineIndex(endTime, min, max);

        // Convert time to fall within the subregion
        localTime = _points[min].time + (_points[max].time - _points[min].time) * time;
    }

    if (loopBlendTime == 0.0f)
    {
        // If no loop blend time is specified, clamp time to end points
        if (localTime < _points[min].time)
            localTime = _points[min].time;
        else if (localTime > _points[max].time)
            localTime = _points[max].time;
    }

    // If an exact endpoint was specified, skip interpolation and return the value directly
    if (localTime == _points[min].time)
    {
        memcpy(dst, _points[min].value, _componentSize);
        return;
    }
    if (localTime == _points[max].time)
    {
        memcpy(dst, _points[max].value, _componentSize);
        return;
    }

    Point* from;
    Point* to;
    float scale;
    float t;
    unsigned int index;

    if (localTime > _points[max].time)
    {
        // Looping forward
        index = max;
        from = &_points[max];
        to = &_points[min];

        // Calculate the fractional time between the two points.
        t = (localTime - from->time) / loopBlendTime;
    }
    else if (localTime < _points[min].time)
    {
        // Looping in reverse
        index = min;
        from = &_points[min];
        to = &_points[max];

        // Calculate the fractional time between the two points.
        t = (from->time - localTime) / loopBlendTime;
    }
    else
    {
        // Locate the points we are interpolating between using a binary search.
        index = determineIndex(localTime, min, max);
        from = &_points[index];
        to = &_points[index == max ? index : index+1];

        // Calculate the fractional time between the two points.
        scale = (to->time - from->time);
        t = (localTime - from->time) / scale;
    }

    // Calculate the value of the curve discretely if appropriate.
    switch (from->type)
    {
        case BEZIER:
        {
            interpolateBezier(t, from, to, dst);
            return;
        }
        case BSPLINE:
        {
            Point* c0;
            Point* c1;
            if (index == 0)
            {
                c0 = from;
            }
            else
            {
                c0 = (_points + index - 1);
            }
            
            if (index == _pointCount - 2)
            {
                c1 = to;
            }
            else
            {
                c1 = (_points + index + 2);
            }
            interpolateBSpline(t, c0, from, to, c1, dst);
            return;
        }
        case FLAT:
        {
            interpolateHermiteFlat(t, from, to, dst);
            return;
        }
        case HERMITE:
        {
            interpolateHermite(t, from, to, dst);
            return;
        }
        case LINEAR:
        {
            // Can just break here because linear formula follows switch
            break;
        }
        case SMOOTH:
        {
            interpolateHermiteSmooth(t, index, from, to, dst);
            return;
        }
        case STEP:
        {
            memcpy(dst, from->value, _componentSize);
            return;
        }
        case QUADRATIC_IN:
        {
            t *= t;
            break;
        }
        case QUADRATIC_OUT:
        {
            t *= -(t - 2.0f);
            break;
        }
        case QUADRATIC_IN_OUT:
        {
            float tx2 = t * 2.0f;

            if (tx2 < 1.0f)
                t = 0.5f * (tx2 * tx2);
            else
            {
                float temp = tx2 - 1.0f;
                t = 0.5f * (-( temp * (temp - 2.0f)) + 1.0f);
            }
            break;
        }
        case QUADRATIC_OUT_IN:
        {
            if (t < 0.5f)
            {
                t = 2.0f * t * (1.0f - t);
            }
            else
            {
                t = 1.0f + 2.0f * t * (t - 1.0f);
            }
            break;
        }
        case CUBIC_IN:
        {
            t *= t * t;
            break;
        }
        case CUBIC_OUT:
        {
            t--;
            t = t * t * t + 1;
            break;
        }
        case CUBIC_IN_OUT:
        {
            if ((t *= 2.0f) < 1.0f)
            {
                t = t * t * t * 0.5f;
            }
            else
            {
                t -= 2.0f;
                t = (t * t * t + 2.0f) * 0.5f;
            }
            break;
        }
        case CUBIC_OUT_IN:
        {
            t = (2.0f * t - 1.0f);
            t = (t * t * t + 1) * 0.5f;
            break;
        }
        case QUARTIC_IN:
        {
            t *= t * t * t;
            break;
        }
        case QUARTIC_OUT:
        {
            t--;
            t = -(t * t * t * t) + 1.0f;
            break;
        }
        case QUARTIC_IN_OUT:
        {
            t *= 2.0f;
            if (t < 1.0f)
            {
                t = 0.5f * t * t * t * t;
            }
            else
            {
                t -= 2.0f;
                t = -0.5f * (t * t * t * t - 2.0f);
            }
            break;
        }
        case QUARTIC_OUT_IN:
        {
            t = 2.0f * t - 1.0f;
            if (t < 0.0f)
            {
                t = 0.5f * (-(t * t) * t * t + 1.0f);
            }
            else
            {
                t = 0.5f * (t * t * t * t + 1.0f);
            }
            break;
        }
        case QUINTIC_IN:
        {
            t *= t * t * t * t;
            break;
        }
        case QUINTIC_OUT:
        {
            t--;
            t = t * t * t * t * t + 1.0f;
            break;
        }
        case QUINTIC_IN_OUT:
        {
            t *= 2.0f;
            if (t < 1.0f)
            {
                t = 0.5f * t * t * t * t * t;
            }
            else
            {
                t -= 2.0f;
                t = 0.5f * (t * t * t * t * t + 2.0f);
            }
            break;
        }
        case QUINTIC_OUT_IN:
        {
            t = 2.0f * t - 1.0f;
            t = 0.5f * (t * t * t * t * t + 1.0f);
            break;
        }
        case SINE_IN:
        {
            t = -(cos(t * MATH_PIOVER2) - 1.0f);
            break;
        }
        case SINE_OUT:
        {
            t = sin(t * MATH_PIOVER2);
            break;
        }
        case SINE_IN_OUT:
        {
            t = -0.5f * (cos(MATH_PI * t) - 1.0f);
            break;
        }
        case SINE_OUT_IN:
        {
            if (t < 0.5f)
            {
                t = 0.5f * sin(MATH_PI * t);
            }
            else
            {
                t = -0.5f * cos(MATH_PIOVER2 * (2.0f * t - 1.0f)) + 1.0f;
            }
            break;
        }
        case EXPONENTIAL_IN:
        {
            if (t != 0.0f)
            {
                t = exp(10.0f * (t - 1.0f));
            }
            break;
        }
        case EXPONENTIAL_OUT:
        {
            if (t != 1.0f)
            {
                t = -exp(-10.0f * t) + 1.0f;
            }
            break;
        }
        case EXPONENTIAL_IN_OUT:
        {
            if (t != 0.0f && t != 1.0f)
            {
                if (t < 0.5f)
                {
                    t = 0.5f * exp(10.0f * (2.0f * t - 1.0f));
                }
                else
                {
                    t = -0.5f * exp(10.0f * (-2.0f * t + 1.0f)) + 1.0f;
                }
            }
            break;
        }
        case EXPONENTIAL_OUT_IN:
        {
            if (t != 0.0f && t != 1.0f)
            {
                if (t < 0.5f)
                {
                    t = -0.5f * exp(-20.0f * t) + 0.5f;
                }
                else
                {
                    t = 0.5f * exp(20.0f * (t - 1.0f)) + 0.5f;
                }
            }
            break;
        }
        case CIRCULAR_IN:
        {
            t = -(sqrt(1.0f - t * t) - 1.0f);
            break;
        }
        case CIRCULAR_OUT:
        {
            t--;
            t = sqrt(1.0f - t * t);
            break;
        }
        case CIRCULAR_IN_OUT:
        {
            t *= 2.0f;
            if (t < 1.0f)
            {
                t = 0.5f * (-sqrt((1.0f - t * t)) + 1.0f);
            }
            else
            {
                t -= 2.0f;
                t = 0.5f * (sqrt((1.0f - t * t)) + 1.0f);
            }
            break;
        }
        case CIRCULAR_OUT_IN:
        {
            t = 2.0f * t - 1.0f;
            if (t < 0.0f)
            {
                t = 0.5f * sqrt(1.0f - t * t);
            }
            else
            {
                t = 0.5f * (2.0f - sqrt(1.0f - t * t));
            }
            break;
        }
        case ELASTIC_IN:
        {
            if (t != 0.0f && t != 1.0f)
            {
                t = t - 1.0f;
                t = -1.0f * ( exp(10.0f * t) * sin( (t - 0.075f) * MATH_PIX2 / 0.3f ) );
            }
            break;
        }
        case ELASTIC_OUT:
        {
            if (t != 0.0f && t != 1.0f)
            {
                t = exp(-10.0f * t) * sin((t - 0.075f) * MATH_PIX2 / 0.3f) + 1.0f;
            }
            break;
        }
        case ELASTIC_IN_OUT:
        {
            if (t != 0.0f && t != 1.0f)
            {
                t = 2.0f * t - 1.0f;
                if (t < 0.0f)
                {
                    t = -0.5f * (exp((10 * t)) * sin(((t - 0.1125f) * MATH_PIX2 / 0.45f)));
                }
                else
                {
                    t = 0.5f * exp((-10 * t)) * sin(((t - 0.1125f) * MATH_PIX2 / 0.45f)) + 1.0f;
                }
            }
            break;
        }
        case ELASTIC_OUT_IN:
        {
            if (t != 0.0f && t != 1.0f)
            {
                t *= 2.0f;
                if (t < 1.0f)
                {
                    t = 0.5f * (exp((-10 * t)) * sin(((t - 0.1125f) * (MATH_PIX2) / 0.45f))) + 0.5f;
                }
                else
                {
                    t = 0.5f * (exp((10 *(t - 2))) * sin(((t - 0.1125f) * (MATH_PIX2) / 0.45f))) + 0.5f;
                }
            }
            break;
        }
        case OVERSHOOT_IN:
        {
            t = t * t * (2.70158f * t - 1.70158f);
            break;
        }
        case OVERSHOOT_OUT:
        {
            t--;
            t = t * t * (2.70158f * t + 1.70158f) + 1;
            break;
        }
        case OVERSHOOT_IN_OUT:
        {
            t *= 2.0f;
            if (t < 1.0f)
            {
                t = 0.5f * t * t * (3.5949095f * t - 2.5949095f);
            }
            else
            {
                t -= 2.0f;
                t = 0.5f * (t * t * (3.5949095f * t + 2.5949095f) + 2.0f);
            }
            break;
        }
        case OVERSHOOT_OUT_IN:
        {
            t = 2.0f * t - 1.0f;
            if (t < 0.0f)
            {
                t = 0.5f * (t * t * (3.5949095f * t + 2.5949095f) + 1.0f);
            }
            else
            {
                t = 0.5f * (t * t * (3.5949095f * t - 2.5949095f) + 1.0f);
            }
            break;
        }
        case BOUNCE_IN:
        {
            t = 1.0f - t;

            if (t < 0.36363636363636365f)
            {
                t = 7.5625f * t * t;
            }
            else if (t < 0.7272727272727273f)
            {
                t -= 0.5454545454545454f;
                t = 7.5625f * t * t + 0.75f;
            }
            else if (t < 0.9090909090909091f)
            {
                t -= 0.8181818181818182f;
                t = 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= 0.9545454545454546f;
                t = 7.5625f * t * t + 0.984375f;
            }

            t = 1.0f - t;
            break;
        }
        case BOUNCE_OUT:
        {
            if (t < 0.36363636363636365f)
            {
                t = 7.5625f * t * t;
            }
            else if (t < 0.7272727272727273f)
            {
                t -= 0.5454545454545454f;
                t = 7.5625f * t * t + 0.75f;
            }
            else if (t < 0.9090909090909091f)
            {
                t -= 0.8181818181818182f;
                t = 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= 0.9545454545454546f;
                t = 7.5625f * t * t + 0.984375f;
            }
            break;
        }
        case BOUNCE_IN_OUT:
        {
            if (t < 0.5f)
            {
                t = 1.0f - t * 2.0f;

                if (t < 0.36363636363636365f)
                {
                    t = 7.5625f * t * t;
                }
                else if (t < 0.7272727272727273f)
                {
                    t -= 0.5454545454545454f;
                    t = 7.5625f * t * t + 0.75f;
                }
                else if (t < 0.9090909090909091f)
                {
                    t -= 0.8181818181818182f;
                    t = 7.5625f * t * t + 0.9375f;
                }
                else
                {
                    t -= 0.9545454545454546f;
                    t = 7.5625f * t * t + 0.984375f;
                }

                t = (1.0f - t) * 0.5f;
            }
            else
            {
                t = t * 2.0f - 1.0f;
                if (t < 0.36363636363636365f)
                {
                    t = 7.5625f * t * t;
                }
                else if (t < 0.7272727272727273f)
                {
                    t -= 0.5454545454545454f;
                    t = 7.5625f * t * t + 0.75f;
                }
                else if (t < 0.9090909090909091f)
                {
                    t -= 0.8181818181818182f;
                    t = 7.5625f * t * t + 0.9375f;
                }
                else
                {
                    t -= 0.9545454545454546f;
                    t = 7.5625f * t * t + 0.984375f;
                }

                t = 0.5f * t + 0.5f;
            }
            break;
        }
        case BOUNCE_OUT_IN:
        {
            if (t < 0.1818181818f)
            {
                t = 15.125f * t * t;
            }
            else if (t < 0.3636363636f)
            {
                t = 1.5f + (-8.250000001f + 15.125f * t) * t;
            }
            else if (t < 0.4545454546f)
            {
                t = 3.0f + (-12.375f + 15.125f * t) * t;
            }
            else if (t < 0.5f)
            {
                t = 3.9375f + (-14.4375f + 15.125f * t) * t;
            }
            else if (t <= 0.5454545455f)
            {
                t = -3.625000004f + (15.81250001f - 15.125f * t) * t;
            }
            else if (t <= 0.6363636365f)
            {
                t = -4.75f + (17.875f - 15.125f * t) * t;
            }
            else if (t <= 0.8181818180f)
            {
                t = -7.374999995f + (21.99999999f - 15.125f * t) * t;
            }
            else
            {
                t = -14.125f + (30.25f - 15.125f * t) * t;
            }
            break;
        }
    }

    interpolateLinear(t, from, to, dst);
}

float Curve::lerp(float t, float from, float to)
{
    return lerpInl(t, from, to);
}

void Curve::setQuaternionOffset(unsigned int offset)
{
    assert(offset <= (_componentCount - 4));

    if (!_quaternionOffset)
        _quaternionOffset = new unsigned int[1];
    
    *_quaternionOffset = offset;
}

void Curve::interpolateBezier(float s, Point* from, Point* to, float* dst) const
{
    float s_2 = s * s;
    float eq0 = 1 - s;
    float eq0_2 = eq0 * eq0;
    float eq1 = eq0_2 * eq0;
    float eq2 = 3 * s * eq0_2;
    float eq3 = 3 * s_2 * eq0;
    float eq4 = s_2 * s;

    float* fromValue = from->value;
    float* toValue = to->value;
    float* outValue = from->outValue;
    float* inValue = to->inValue;


    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = bezier(eq1, eq2, eq3, eq4, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = bezier(eq1, eq2, eq3, eq4, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }

        // Handle quaternion component.
        float interpTime = bezier(eq1, eq2, eq3, eq4, from->time, outValue[i], to->time, inValue[i]);
        interpolateQuaternion(interpTime, (fromValue + i), (toValue + i), (dst + i));
        
        // Handle remaining components (if any) as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = bezier(eq1, eq2, eq3, eq4, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }
    }
}

void Curve::interpolateBSpline(float s, Point* c0, Point* c1, Point* c2, Point* c3, float* dst) const
{   
    float s_2 = s * s;
    float s_3 = s_2 * s;
    float eq0 = (-s_3 + 3 * s_2 - 3 * s + 1) / 6.0f;
    float eq1 = (3 * s_3 - 6 * s_2 + 4) / 6.0f;
    float eq2 = (-3 * s_3 + 3 * s_2 + 3 * s + 1) / 6.0f;
    float eq3 = s_3 / 6.0f;

    float* c0Value = c0->value;
    float* c1Value = c1->value;
    float* c2Value = c2->value;
    float* c3Value = c3->value;

    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (c1Value[i] == c2Value[i])
                dst[i] = c1Value[i];
            else
                dst[i] = bspline(eq0, eq1, eq2, eq3, c0Value[i], c1Value[i], c2Value[i], c3Value[i]);
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {
            if (c1Value[i] == c2Value[i])
                dst[i] = c1Value[i];
            else
                dst[i] = bspline(eq0, eq1, eq2, eq3, c0Value[i], c1Value[i], c2Value[i], c3Value[i]);
        }

        // Handle quaternion component.
        float interpTime;
        if (c0->time == c1->time)
            interpTime = bspline(eq0, eq1, eq2, eq3, -c0->time, c1->time, c2->time, c3->time);
        else if (c2->time == c3->time)
            interpTime = bspline(eq0, eq1, eq2, eq3, c0->time, c1->time, c2->time, -c3->time); 
        else
            interpTime = bspline(eq0, eq1, eq2, eq3, c0->time, c1->time, c2->time, c3->time);
        interpolateQuaternion(s, (c1Value + i) , (c2Value + i), (dst + i));
            
        // Handle remaining components (if any) as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (c1Value[i] == c2Value[i])
                dst[i] = c1Value[i];
            else
                dst[i] = bspline(eq0, eq1, eq2, eq3, c0Value[i], c1Value[i], c2Value[i], c3Value[i]);
        }
    }
}

void Curve::interpolateHermite(float s, Point* from, Point* to, float* dst) const
{
    // Calculate the hermite basis functions.
    float s_2 = s * s;                   // t^2
    float s_3 = s_2 * s;                 // t^3
    float h00 = 2 * s_3 - 3 * s_2 + 1;   // basis function 0
    float h01 = -2 * s_3 + 3 * s_2;      // basis function 1
    float h10 = s_3 - 2 * s_2 + s;       // basis function 2
    float h11 = s_3 - s_2;               // basis function 3

    float* fromValue = from->value;
    float* toValue = to->value;
    float* outValue = from->outValue;
    float* inValue = to->inValue;

    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermite(h00, h01, h10, h11, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermite(h00, h01, h10, h11, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }

        // Handle quaternion component.
        float interpTime = hermite(h00, h01, h10, h11, from->time, outValue[i], to->time, inValue[i]);
        interpolateQuaternion(interpTime, (fromValue + i), (toValue + i), (dst + i));
        
        // Handle remaining components (if any) as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermite(h00, h01, h10, h11, fromValue[i], outValue[i], toValue[i], inValue[i]);
        }
    }
}

void Curve::interpolateHermiteFlat(float s, Point* from, Point* to, float* dst) const
{
    // Calculate the hermite basis functions.
    float s_2 = s * s;                   // t^2
    float s_3 = s_2 * s;                 // t^3
    float h00 = 2 * s_3 - 3 * s_2 + 1;   // basis function 0
    float h01 = -2 * s_3 + 3 * s_2;      // basis function 1

    float* fromValue = from->value;
    float* toValue = to->value;

    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermiteFlat(h00, h01, fromValue[i], toValue[i]);
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermiteFlat(h00, h01, fromValue[i], toValue[i]);
        }

        // Handle quaternion component.
        float interpTime = hermiteFlat(h00, h01, from->time, to->time);
        interpolateQuaternion(interpTime, (fromValue + i), (toValue + i), (dst + i));
        
        // Handle remaining components (if any) as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = hermiteFlat(h00, h01, fromValue[i], toValue[i]);
        }
    }
}

void Curve::interpolateHermiteSmooth(float s, unsigned int index, Point* from, Point* to, float* dst) const
{
    // Calculate the hermite basis functions.
    float s_2 = s * s;                   // t^2
    float s_3 = s_2 * s;                 // t^3
    float h00 = 2 * s_3 - 3 * s_2 + 1;   // basis function 0
    float h01 = -2 * s_3 + 3 * s_2;      // basis function 1
    float h10 = s_3 - 2 * s_2 + s;       // basis function 2
    float h11 = s_3 - s_2;               // basis function 3

    float inValue;
    float outValue;

    float* fromValue = from->value;
    float* toValue = to->value;

    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
            {
                dst[i] = fromValue[i];
            }
            else
            {
                if (index == 0)
                {
                    outValue = toValue[i] - fromValue[i];
                }
                else
                {
                    outValue = (toValue[i] - (from - 1)->value[i]) * ((from->time - (from - 1)->time) / (to->time - (from - 1)->time));
                }

                if (index == _pointCount - 2)
                {
                    inValue = toValue[i] - fromValue[i];
                }
                else
                {
                    inValue = ((to + 1)->value[i] - fromValue[i]) * ((to->time - from->time) / ((to + 1)->time - from->time));
                }

                dst[i] = hermiteSmooth(h00, h01, h10, h11, fromValue[i], outValue, toValue[i], inValue);
            }
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {   
            if (fromValue[i] == toValue[i])
            {
                dst[i] = fromValue[i];
            }
            else
            {    
                if (index == 0)
                {
                    outValue = toValue[i] - fromValue[i];
                }
                else
                {
                    outValue = (toValue[i] - (from - 1)->value[i]) * ((from->time - (from - 1)->time) / (to->time - (from - 1)->time));
                }

                if (index == _pointCount - 2)
                {
                    inValue = toValue[i] - fromValue[i];
                }
                else
                {
                    inValue = ((to + 1)->value[i] - fromValue[i]) * ((to->time - from->time) / ((to + 1)->time - from->time));
                }

                dst[i] = hermiteSmooth(h00, h01, h10, h11, fromValue[i], outValue, toValue[i], inValue);
            }
        }

        // Handle quaternion component.
        if (index == 0)
        {
            outValue = to->time - from->time;
        }
        else
        {
            outValue = (to->time - (from - 1)->time) * ((from->time - (from - 1)->time) / (to->time - (from - 1)->time));
        }

        if (index == _pointCount - 2)
        {
            inValue = to->time - from->time;
        }
        else
        {
            inValue = ((to + 1)->time - from->time) * ((to->time - from->time) / ((to + 1)->time - from->time));
        }

        float interpTime = hermiteSmooth(h00, h01, h10, h11, from->time, outValue, to->time, inValue);
        interpolateQuaternion(interpTime, (fromValue + i), (toValue + i), (dst + i));
        
        // Handle remaining components (if any) as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
            {
                dst[i] = fromValue[i];
            }
            else
            {
                // Interpolate as scalar.
                if (index == 0)
                {
                    outValue = toValue[i] - fromValue[i];
                }
                else
                {
                    outValue = (toValue[i] - (from - 1)->value[i]) * ((from->time - (from - 1)->time) / (to->time - (from - 1)->time));
                }

                if (index == _pointCount - 2)
                {
                    inValue = toValue[i] - fromValue[i];
                }
                else
                {
                    inValue = ((to + 1)->value[i] - fromValue[i]) * ((to->time - from->time) / ((to + 1)->time - from->time));
                }

                dst[i] = hermiteSmooth(h00, h01, h10, h11, fromValue[i], outValue, toValue[i], inValue);
            }
        }
    }
}

void Curve::interpolateLinear(float s, Point* from, Point* to, float* dst) const
{
    float* fromValue = from->value;
    float* toValue = to->value;

    if (!_quaternionOffset)
    {
        for (unsigned int i = 0; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = lerpInl(s, fromValue[i], toValue[i]);
        }
    }
    else
    {
        // Interpolate any values up to the quaternion offset as scalars.
        unsigned int quaternionOffset = *_quaternionOffset;
        unsigned int i = 0;
        for (i = 0; i < quaternionOffset; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = lerpInl(s, fromValue[i], toValue[i]);
        }

        // Handle quaternion component.
        interpolateQuaternion(s, (fromValue + i), (toValue + i), (dst + i));
        
        // handle any remaining components as scalars
        for (i += 4; i < _componentCount; i++)
        {
            if (fromValue[i] == toValue[i])
                dst[i] = fromValue[i];
            else
                dst[i] = lerpInl(s, fromValue[i], toValue[i]);
        }
    }
}


void slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw)
{
    // Fast slerp implementation by kwhatmough:
    // It contains no division operations, no trig, no inverse trig
    // and no sqrt. Not only does this code tolerate small constraint
    // errors in the input quaternions, it actually corrects for them.
    assert(dstx && dsty && dstz && dstw);
    assert(!(t < 0.0f || t > 1.0f));

    if (t == 0.0f)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }
    else if (t == 1.0f)
    {
        *dstx = q2x;
        *dsty = q2y;
        *dstz = q2z;
        *dstw = q2w;
        return;
    }

    if (q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }

    float halfY, alpha, beta;
    float u, f1, f2a, f2b;
    float ratio1, ratio2;
    float halfSecHalfTheta, versHalfTheta;
    float sqNotU, sqU;

    float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

    // As usual in all slerp implementations, we fold theta.
    alpha = cosTheta >= 0 ? 1.0f : -1.0f;
    halfY = 1.0f + alpha * cosTheta;

    // Here we bisect the interval, so we need to fold t as well.
    f2b = t - 0.5f;
    u = f2b >= 0 ? f2b : -f2b;
    f2a = u - f2b;
    f2b += u;
    u += u;
    f1 = 1.0f - u;

    // One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
    halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
    halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
    versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

    // Evaluate series expansions of the coefficients.
    sqNotU = f1 * f1;
    ratio2 = 0.0000440917108f * versHalfTheta;
    ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
    ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
    ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
    ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

    sqU = u * u;
    ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
    ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
    ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
    ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

    // Perform the bisection and resolve the folding done earlier.
    f1 *= ratio1 * halfSecHalfTheta;
    f2a *= ratio2;
    f2b *= ratio2;
    alpha *= f1 + f2a;
    beta = f1 + f2b;

    // Apply final coefficients to a and b as usual.
    float w = alpha * q1w + beta * q2w;
    float x = alpha * q1x + beta * q2x;
    float y = alpha * q1y + beta * q2y;
    float z = alpha * q1z + beta * q2z;

    // This final adjustment to the quaternion's length corrects for
    // any small constraint error in the inputs q1 and q2 But as you
    // can see, it comes at the cost of 9 additional multiplication
    // operations. If this error-correcting feature is not required,
    // the following code may be removed.
    f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
    *dstw = w * f1;
    *dstx = x * f1;
    *dsty = y * f1;
    *dstz = z * f1;
}

void Curve::interpolateQuaternion(float s, float* from, float* to, float* dst) const
{
    // Evaluate.
    if (s >= 0)
        slerp(from[0], from[1], from[2], from[3], to[0], to[1], to[2], to[3], s, dst, dst + 1, dst + 2, dst + 3);
    else
        slerp(to[0], to[1], to[2], to[3], from[0], from[1], from[2], from[3], s, dst, dst + 1, dst + 2, dst + 3);
}

int Curve::determineIndex(float time, unsigned int min, unsigned int max) const
{
    unsigned int mid;

    // Do a binary search to determine the index.
    do 
    {
        mid = (min + max) >> 1;

        if (time >= _points[mid].time && time < _points[mid + 1].time)
            return mid;
        else if (time < _points[mid].time)
            max = mid - 1;
        else
            min = mid + 1;
    } while (min <= max);
    
    return max;
}

int Curve::getInterpolationType(const char* curveId)
{
    if (strcmp(curveId, "BEZIER") == 0)
    {
        return Curve::BEZIER;
    }
    else if (strcmp(curveId, "BSPLINE") == 0)
    {
        return Curve::BSPLINE;
    }
    else if (strcmp(curveId, "FLAT") == 0)
    {
        return Curve::FLAT;
    }
    else if (strcmp(curveId, "HERMITE") == 0)
    {
        return Curve::HERMITE;
    }
    else if (strcmp(curveId, "LINEAR") == 0)
    {
        return Curve::LINEAR;
    }
    else if (strcmp(curveId, "SMOOTH") == 0)
    {
        return Curve::SMOOTH;
    }
    else if (strcmp(curveId, "STEP") == 0)
    {
        return Curve::STEP;
    }
    else if (strcmp(curveId, "QUADRATIC_IN") == 0)
    {
        return Curve::QUADRATIC_IN;
    }
    else if (strcmp(curveId, "QUADRATIC_OUT") == 0)
    {
        return Curve::QUADRATIC_OUT;
    }
    else if (strcmp(curveId, "QUADRATIC_IN_OUT") == 0)
    {
        return Curve::QUADRATIC_IN_OUT;
    }
    else if (strcmp(curveId, "QUADRATIC_OUT_IN") == 0)
    {
        return Curve::QUADRATIC_OUT_IN;
    }
    else if (strcmp(curveId, "CUBIC_IN") == 0)
    {
        return Curve::CUBIC_IN;
    }
    else if (strcmp(curveId, "CUBIC_OUT") == 0)
    {
        return Curve::CUBIC_OUT;
    }
    else if (strcmp(curveId, "CUBIC_IN_OUT") == 0)
    {
        return Curve::CUBIC_IN_OUT;
    }
    else if (strcmp(curveId, "CUBIC_OUT_IN") == 0)
    {
        return Curve::CUBIC_OUT_IN;
    }
    else if (strcmp(curveId, "QUARTIC_IN") == 0)
    {
        return Curve::QUARTIC_IN;
    }
    else if (strcmp(curveId, "QUARTIC_OUT") == 0)
    {
        return Curve::QUARTIC_OUT;
    }
    else if (strcmp(curveId, "QUARTIC_IN_OUT") == 0)
    {
        return Curve::QUARTIC_IN_OUT;
    }
    else if (strcmp(curveId, "QUARTIC_OUT_IN") == 0)
    {
        return Curve::QUARTIC_OUT_IN;
    }
    else if (strcmp(curveId, "QUINTIC_IN") == 0)
    {
        return Curve::QUINTIC_IN;
    }
    else if (strcmp(curveId, "QUINTIC_OUT") == 0)
    {
        return Curve::QUINTIC_OUT;
    }
    else if (strcmp(curveId, "QUINTIC_IN_OUT") == 0)
    {
        return Curve::QUINTIC_IN_OUT;
    }
    else if (strcmp(curveId, "QUINTIC_OUT_IN") == 0)
    {
        return Curve::QUINTIC_OUT_IN;
    }
    else if (strcmp(curveId, "SINE_IN") == 0)
    {
        return Curve::SINE_IN;
    }
    else if (strcmp(curveId, "SINE_OUT") == 0)
    {
        return Curve::SINE_OUT;
    }
    else if (strcmp(curveId, "SINE_IN_OUT") == 0)
    {
        return Curve::SINE_IN_OUT;
    }
    else if (strcmp(curveId, "SINE_OUT_IN") == 0)
    {
        return Curve::SINE_OUT_IN;
    }
    else if (strcmp(curveId, "EXPONENTIAL_IN") == 0)
    {
        return Curve::EXPONENTIAL_IN;
    }
    else if (strcmp(curveId, "EXPONENTIAL_OUT") == 0)
    {
        return Curve::EXPONENTIAL_OUT;
    }
    else if (strcmp(curveId, "EXPONENTIAL_IN_OUT") == 0)
    {
        return Curve::EXPONENTIAL_IN_OUT;
    }
    else if (strcmp(curveId, "EXPONENTIAL_OUT_IN") == 0)
    {
        return Curve::EXPONENTIAL_OUT_IN;
    }
    else if (strcmp(curveId, "CIRCULAR_IN") == 0)
    {
        return Curve::CIRCULAR_IN;
    }
    else if (strcmp(curveId, "CIRCULAR_OUT") == 0)
    {
        return Curve::CIRCULAR_OUT;
    }
    else if (strcmp(curveId, "CIRCULAR_IN_OUT") == 0)
    {
        return Curve::CIRCULAR_IN_OUT;
    }
    else if (strcmp(curveId, "CIRCULAR_OUT_IN") == 0)
    {
        return Curve::CIRCULAR_OUT_IN;
    }
    else if (strcmp(curveId, "ELASTIC_IN") == 0)
    {
        return Curve::ELASTIC_IN;
    }
    else if (strcmp(curveId, "ELASTIC_OUT") == 0)
    {
        return Curve::ELASTIC_OUT;
    }
    else if (strcmp(curveId, "ELASTIC_IN_OUT") == 0)
    {
        return Curve::ELASTIC_IN_OUT;
    }
    else if (strcmp(curveId, "ELASTIC_OUT_IN") == 0)
    {
        return Curve::ELASTIC_OUT_IN;
    }
    else if (strcmp(curveId, "OVERSHOOT_IN") == 0)
    {
        return Curve::OVERSHOOT_IN;
    }
    else if (strcmp(curveId, "OVERSHOOT_OUT") == 0)
    {
        return Curve::OVERSHOOT_OUT;
    }
    else if (strcmp(curveId, "OVERSHOOT_IN_OUT") == 0)
    {
        return Curve::OVERSHOOT_IN_OUT;
    }
    else if (strcmp(curveId, "OVERSHOOT_OUT_IN") == 0)
    {
        return Curve::OVERSHOOT_OUT_IN;
    }
    else if (strcmp(curveId, "BOUNCE_IN") == 0)
    {
        return Curve::BOUNCE_IN;
    }
    else if (strcmp(curveId, "BOUNCE_OUT") == 0)
    {
        return Curve::BOUNCE_OUT;
    }
    else if (strcmp(curveId, "BOUNCE_IN_OUT") == 0)
    {
        return Curve::BOUNCE_IN_OUT;
    }
    else if (strcmp(curveId, "BOUNCE_OUT_IN") == 0)
    {
        return Curve::BOUNCE_OUT_IN;
    }

    return -1;
}
