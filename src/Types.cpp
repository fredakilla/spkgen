#include "Types.h"
#include <math.h>

eF32 eAbs(eF32 x)
{
    return fabsf(x);
}

eU32 eAbs(eInt x)
{
    return ((x^(x>>31))-(x>>31));
}

eChar * eIntToStr(eInt val)
{
    // remember if integer is negative and
    // if it is, make it positive
    const eBool negative = (val < 0);

    if (negative)
        val = -val;

    // 12 spaces are enough for 32-bit decimal
    // (10 digits + 1 null terminator byte +
    // eventually a sign character)
    static eChar str[12];

    eChar *cp = str+sizeof(str)-1;
    *cp = '\0';

    do
    {
        *(--cp) = val%10+'0';
        val /= 10;
    }
    while (val > 0);

    // prepend negative sign character
    if (negative)
        *(--cp) = '-';

    return cp;
}

eU32 eSignBit(eF32 x)
{
    return (eU32 &)x&0x80000000;
}

eF32 eSign(eF32 x)
{
    // test exponent and mantissa bits: is input zero?
    if (((eInt &)x&0x7fffffff) == 0)
        return 0.0f;

    // mask sign bit in x, set it in r if necessary
    eF32 r = 1.0f;
    (eInt &)r |= eSignBit(x);
    return r;
}

eInt eSign(eInt x)
{
    return (x != 0)|(x>>(sizeof(eInt)*8-1));
}

eBool eIsFloatZero(eF32 x)
{
    return (eAbs(x) < eALMOST_ZERO);
}

eBool eAreFloatsEqual(eF32 x, eF32 y)
{
    return eIsFloatZero(x-y);
}

eBool eMemEqual(eConstPtr mem0, eConstPtr mem1, size_t count)
{
    return !memcmp(mem0, mem1, count);
}

// faster float to long conversion than c-lib's
// default version. must be called explicitly.
#include <xmmintrin.h>
eInt eFtoL(eF32 x)
{
    return _mm_cvt_ss2si(_mm_load_ss(&x));
    //return (eInt)(x+0.001) / 1L;
    //return (eInt)x;
}
