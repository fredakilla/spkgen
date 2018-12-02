#include "Color.h"

eColor::eColor() : r(0), g(0), b(0), a(255)
{
}

eColor::eColor(eU8 nr, eU8 ng, eU8 nb, eU8 na) : r(nr), g(ng), b(nb), a(na)
{
}

eColor::eColor(eColorConst cc)
{
    static const eColor colors[] =
    {
        eColor(255,   0,   0), // red
        eColor(  0, 255,   0), // green
        eColor(  0,   0, 255), // blue
        eColor(215, 108,   0), // orange
        eColor(255, 255,   0), // yellow
        eColor(255,   0, 255), // purple
        eColor(  0, 255, 255), // cyan
        eColor(255,   0, 128), // pink
        eColor(255, 255, 255), // white
        eColor(  0,   0,   0), // black
        eColor(128, 128, 128), // gray
        eColor( 64,  64,  64), // dark gray
        eColor(204, 204, 204), // light gray
    };

    *this = colors[cc];
}

void eColor::set(eU8 nr, eU8 ng, eU8 nb)
{
    r = nr;
    g = ng;
    b = nb;
}

void eColor::set(eU8 nr, eU8 ng, eU8 nb, eU8 na)
{
    set(nr, ng, nb);
    a = na;
}

eU8 eColor::grayScale() const
{
    return (r*11+g*16+b*5)/32;
}

void eColor::toGrayScale()
{
    const eU8 gray = grayScale();
    r = gray;
    g = gray;
    b = gray;
}

void eColor::scale (const eColor &c)
{
    r = r*c.r/255;
    g = g*c.g/255;
    b = b*c.b/255;
    a = a*c.a/255;
}

eColor eColor::operator + (const eColor &c) const
{
    return eColor(eMin((eInt)(r+c.r), 255),
                    eMin((eInt)(g+c.g), 255),
                    eMin((eInt)(b+c.b), 255),
                    eMin((eInt)(a+c.a), 255));
}

eColor eColor::operator - (const eColor &c) const
{
    return eColor(eMax(0, (eInt)(r-c.r)),
                    eMax(0, (eInt)(g-c.g)),
                    eMax(0, (eInt)(b-c.b)),
                    eMax(0, (eInt)(a-c.a)));
}

eColor eColor::operator * (eF32 s) const
{
    eASSERT(s >= 0.0f);

    return eColor(eMin(eFtoL((eF32)r*s), 255),
                    eMin(eFtoL((eF32)g*s), 255),
                    eMin(eFtoL((eF32)b*s), 255),
                    eMin(eFtoL((eF32)a*s), 255));
}

eColor & eColor::operator += (const eColor &c)
{
    *this = *this+c;
    return *this;
}

eColor & eColor::operator -= (const eColor &c)
{
    *this = *this-c;
    return *this;
}

eColor & eColor::operator *= (eF32 s)
{
    *this = *this*s;
    return *this;
}

void eColor::minComponents(const eColor &c)
{
    r = eMin(r, c.r);
    g = eMin(g, c.g);
    b = eMin(b, c.b);
    a = eMin(a, c.a);
}

void eColor::maxComponents(const eColor &c)
{
    r = eMax(r, c.r);
    g = eMax(g, c.g);
    b = eMax(b, c.b);
    a = eMax(a, c.a);
}

void eColor::average(const eColor &c)
{
    r = (r+c.r)/2;
    g = (g+c.g)/2;
    b = (b+c.b)/2;
    a = (a+c.a)/2;
}

void eColor::difference(const eColor &c)
{
    r = eAbs(r-c.r);
    g = eAbs(g-c.g);
    b = eAbs(b-c.b);
    a = eAbs(a-c.a);
}

// linear interpolation (0 <= t <= 1)
eColor eColor::lerp(const eColor &to, eF32 t) const
{
    eASSERT(t >= 0.0f && t <= 1.0f);

    return eColor(eFtoL(eLerp((eF32)r, (eF32)to.r, t)),
                    eFtoL(eLerp((eF32)g, (eF32)to.g, t)),
                    eFtoL(eLerp((eF32)b, (eF32)to.b, t)),
                    eFtoL(eLerp((eF32)a, (eF32)to.a, t)));
}

eU32 eColor::toArgb() const // a MSB, b LSB
{
    // swaps R und B channels
    eU32 argb = abgr&0xff00ff00;
    argb |= ((abgr&0x00ff0000)>>16);
    argb |= ((abgr&0x000000ff)<<16);
    return argb;
}

const eU8 & eColor::operator [] (eInt index) const
{
    eASSERT(index < 4);
    return ((eU8 *)this)[index];
}

eU8 & eColor::operator [] (eInt index)
{
    eASSERT(index < 4);
    return ((eU8 *)this)[index];
}

eBool eColor::operator == (const eColor &c) const
{
    return (abgr == c.abgr);
}

eBool eColor::operator != (const eColor &c) const
{
    return !(*this == c);
}

// converts from RGB to HSV color space. the hue
// defines the color. its range is 0..359 if the
// color is chromatic and -1 if the color is
// achromatic. the saturation and value both vary
// between 0 and 255.
void eColor::toHsv(eInt &h, eInt &s, eInt &v)
{
    // find maximum channel
    eInt max = r;
    eInt whatMax = 0; // r => 0, g => 1, b => 2

    if (g > max)
    {
        max = g;
        whatMax = 1;
    }

    if (b > max)
    {
        max = b;
        whatMax = 2;
    }

    // find minimum channel
    const eInt min = eMin(r, eMin(g, b));
    const eInt delta = max-min;

    // calculate HSV values
    v = max;
    s = max ? (510*delta+max)/(2*max) : 0;

    if (s == 0)
        h = -1; // hue is undefined
    else
    {
        switch (whatMax)
        {
        case 0: // red is max component
            if (g >= b)
                h = (120*(g-b)+delta)/(2*delta);
            else
                h = (120*(g-b+delta)+delta)/(2*delta)+300;
            break;

        case 1: // green is max component
            if (b > r)
                h = 120+(120*(b-r)+delta)/(2*delta);
            else
                h = 60+(120*(b-r+delta)+delta)/(2*delta);
            break;

        case 2: // blue is max component
            if (r > g)
                h = 240+(120*(r-g)+delta)/(2*delta);
            else
                h = 180+(120*(r-g+delta)+delta)/(2*delta);
            break;
        }
    }
}

void eColor::fromHsv(eInt h, eInt s, eInt v)
{
    if (h < -1 || (eU32)s > 255 || (eU32)v > 255)
        return;

    if (s == 0 || h == -1)
        set(v, v, v); // ignore achromatic case
    else
    {
        // much more complicated chromatic case
        if ((eU32)h >= 360)
            h %= 360;

        const eU32 f = h%60;
        h /= 60;
        const eU32 p = (eU32)(2*v*(255-s)+255)/510;

        if (h&1)
        {
            const eU32 q = (eU32)(2*v*(15300-s*f)+15300)/30600;

            switch (h)
            {
            case 1:
                set(q, v, p);
                break;

            case 3:
                set(p, q, v);
                break;

            case 5:
                set(v, p, q);
                break;
            }
        }
        else
        {
            const eU32 t = (eU32)(2*v*(15300-(s*(60-f)))+15300)/30600;

            switch (h)
            {
            case 0:
                set(v, t, p);
                break;

            case 2:
                set(p, v, t);
                break;

            case 4:
                set(t, p, v);
                break;
            }
        }
    }
}
