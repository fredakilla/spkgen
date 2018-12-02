#ifndef TYPES_H
#define TYPES_H

#include <assert.h>

typedef unsigned char   eU8;
typedef signed char     eS8;
typedef unsigned short  eU16;
typedef short           eS16;
typedef unsigned int    eU32;
typedef int             eS32;
typedef float           eF32;
typedef double          eF64;
typedef int             eInt;
typedef char            eChar;
typedef signed char     eBool;
typedef void *          ePtr;
typedef const void *    eConstPtr;
typedef eU32            eID;

#define eU32_MAX        (0xffffffffU)
#define eS32_MIN        (-2147483647-1)
#define eS32_MAX        (2147483647)
#define eU16_MAX        (0xffffU)
#define eS16_MIN        (-32768)
#define eS16_MAX        (32767)
#define eU8_MAX         (0xffU)
#define eS8_MIN         (-128)
#define eS8_MAX         (127)
#define eF32_MAX        (3.402823466e+38F)
#define eF32_MIN        (-eF32_MAX)
#define eF32_INF        (1e30f)
#define eTRUE           (eBool)(!0)
#define eFALSE          0
#define eNOID           0

#define eSQRT2          1.41421356237f
#define ePI             3.1415926535897932384626433832795f
#define eTWOPI          (ePI*2.0f)
#define eHALFPI         (ePI*0.5f)
#define eSQRPI          (ePI*ePI)
#define eINVHALFPI      (1.0f/eHALFPI)
#define eEXPONE         2.718281828459f
#define eALMOST_ZERO    0.00001f
#define eMAX_RAND       2147483647
#define eMAX_NAME_LEN   64
#define eMAX_PATH_LEN   256
#define e360            360.0f


#define eASSERT(X) assert(X)


// fake type during code integration; to delete after
typedef void* eIOperator;
#include <string>
#include <qstring.h>
typedef QString eString;


eF32 eAbs(eF32 x);
eU32 eAbs(eInt x);
eInt eFtoL(eF32 x);
eChar* eIntToStr(eInt val);
eU32 eSignBit(eF32 x);
eF32 eSign(eF32 x);
eInt eSign(eInt x);
eBool eAreFloatsEqual(eF32 x, eF32 y);
eBool eIsFloatZero(eF32 x);
eBool eMemEqual(eConstPtr mem0,eConstPtr mem1, size_t count);



template<class T> T eMin(const T &a, const T &b)
{
    return (a < b) ? a : b;
}

template<class T> T eMax(const T &a, const T &b)
{
    return (a > b) ? a : b;
}

template<class T> T eClamp(const T &min, const T &x, const T &max)
{
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
}

template <typename T> inline T zLerp(const T &a, const T &b, T t)
{
    return (1-t)*a + t*b;
}

template<class T> T eLerp(const T &a, const T &b, eF32 t)
{
    return a+(b-a)*t;
}

template<class T> void eSetBit(T &t, eU32 index)
{
    eASSERT(index < sizeof(T)*8);
    t |= (1<<index);
}

template<class T> void eSetBit(T &t, eU32 index, eBool set)
{
    eASSERT(set == 0 || set == 1);

    eASSERT(index <sizeof(T)*8);
    t |= (set<<index);
}

template<class T> void eClearBit(T &t, eU32 index)
{
    eASSERT(index < sizeof(T)*8);
    t &= ~(1<<index);
}

template<class T> eBool eGetBit(T t, eU32 index)
{
    eASSERT(index < sizeof(T)*8);
    return ((t&(1<<index)) != 0);
}

template<class T> void eToggleBit(T &t, eU32 index)
{
    eASSERT(index < sizeof(T)*8);
    t ^= (1<<index);
}






template <typename T>
class eXY
{
public:
    struct
    {
        T    x;
        T    y;
    };

    eXY() {}
    eXY(T x, T y) : x(x), y(y) {}
};


template <typename T>
class eXYZ
{
public:
    struct
    {
        T    x;
        T    y;
        T    z;
    };

    eXYZ() {}
    eXYZ(T x, T y, T z) : x(x), y(y), z(z) {}
};


template <typename T>
class eXYZW
{
public:
    struct
    {
        T    x;
        T    y;
        T    z;
        T    w;
    };

    eXYZW() {}
    eXYZW(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
};

typedef eXY<float>      eFXY;
typedef eXYZ<float>     eFXYZ;
typedef eXYZW<float>    eFXYZW;
typedef eXY<int>        eIXY;
typedef eXYZ<int>       eIXYZ;
typedef eXYZW<int>      eIXYZW;


#endif // TYPES_H
