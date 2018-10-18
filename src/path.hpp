/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   This file is part of
 *       ______        _                             __ __
 *      / ____/____   (_)____ _ ____ ___   ____ _   / // /
 *     / __/  / __ \ / // __ `// __ `__ \ / __ `/  / // /_
 *    / /___ / / / // // /_/ // / / / / // /_/ /  /__  __/
 *   /_____//_/ /_//_/ \__, //_/ /_/ /_/ \__,_/     /_/.   
 *                    /____/                              
 *
 *   Copyright © 2003-2012 Brain Control, all rights reserved.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PATH_HPP
#define PATH_HPP

#include "Types.h"
#include <vector>
#include <math/Vector4.h>

using namespace gplay;

#define eArray std::vector

enum ePathKeyInterpol
{
    ePI_CUBIC,
    ePI_LINEAR,
    ePI_STEP
};

enum ePathLoopMode
{
    ePLM_LOOP,
    ePLM_ZERO,
    ePLM_LAST,
};

struct ePathKey
{
    ePathKeyInterpol        interpol;
    eF32                    time;
    eF32                    val;


    bool operator==(const ePathKey& other) const
    {
        return (this->time == other.time &&
                this->val == other.val &&
                this->interpol == other.interpol);
    }

    /*bool operator==(const ePathKey& p1, const ePathKey& p2)
    {
        return (p1.time == p2.time &&
                p1.val == p2.val &&
                p1.interpol == p2.interpol);
    }*/

#ifdef eEDITOR
    class ePath *           ownerPath;
    eBool                   selected;
#endif
};

class ePath
{
public:
    ePath(eU32 keyCount = 0);

    eF32                    evaluate(eF32 time) const;
    void                    addKey(eF32 time, eF32 val, ePathKeyInterpol interpol);
    void                    removeKey(eU32 index);
    void                    clear();
    void                    setLoopMode(ePathLoopMode pcm);

    ePathLoopMode           getLoopMode() const;
    const ePathKey &        getKeyByIndex(eU32 index) const;
    ePathKey &              getKeyByIndex(eU32 index);
    const ePathKey *        getKeyByTime(eF32 time) const;
    eU32                    getKeyCount() const;
    eF32                    getDuration() const;
    eF32                    getStartTime() const;
    eF32                    getEndTime() const;

    eBool                   operator == (const ePath &p) const;
    eBool                   operator != (const ePath &p) const;

private:
    void                    _insertKey(const ePathKey &key);
    eF32                    _catmullRom(eF32 t, eF32 v0, eF32 v1, eF32 v2, eF32 v3) const;

private:
    eArray<ePathKey>        m_keys;
    ePathLoopMode           m_loopMode;
};

class ePath4
{
public:
    Vector4                evaluate(eF32 time) const;
    Vector4                evaluateUnitTime(eF32 time) const;

    eBool                   isEmpty() const;
    const ePath &           getSubPath(eU32 index) const;
    ePath &                 getSubPath(eU32 index);
    eArray<ePathKey *>      getAllKeys();
    eF32                    getDuration() const;
    eF32                    getStartTime() const;
    eF32                    getEndTime() const;

    eBool                   operator == (const ePath4 &p) const;
    eBool                   operator != (const ePath4 &p) const;

private:
    ePath                   m_subPaths[4];
};

struct ePath4Sample
{
    eF32                    time;
    Vector4                values;
};

class ePath4Sampler
{
public:
    void                    sample(const ePath4 &path, eU32 samplesPerSec=25);
    const Vector4 &        evaluate(eF32 time) const;

    eU32                    getSampleCount() const;
    const ePath4Sample &    getSample(eU32 index) const;

private:
    eArray<ePath4Sample>    m_samples;
    eF32                    m_stepInv;
};

#endif // PATH_HPP
