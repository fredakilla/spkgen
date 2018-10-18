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

//#include "../system/system.hpp"
//#include "../math/math.hpp"
#include "path.hpp"
#include <math.h>

ePath::ePath(eU32 keyCount) :
    m_loopMode(ePLM_LOOP)
{
	m_keys.resize(keyCount);
}

eF32 ePath::evaluate(eF32 time) const
{
    if (!m_keys.size())
        return 0.0f;

    const ePathKey &keyMin = m_keys.front();
    const ePathKey &keyMax = m_keys.back();

    if (m_loopMode == ePLM_LOOP)
        time = fmodf(time-keyMin.time, keyMax.time-keyMin.time);

    if (time <= keyMin.time || m_keys.size() == 1) // time before first key?
        return (m_loopMode == ePLM_ZERO ? 0.0f : keyMin.val);
    else if (time >= keyMax.time) //  time after last key?
        return (m_loopMode == ePLM_ZERO ? 0.0f : keyMax.val);
    else // time between first and last key?
    {
        eInt maxKey = (eInt)m_keys.size()-1;
        for (eInt i=0; i<maxKey; i++)
        {
            const ePathKey &keyFrom = m_keys[i];
            const ePathKey &keyTo = m_keys[i+1];

            if (keyFrom.time <= time && keyTo.time > time)
            {
                const ePathKey &wpPrev = (i > 0 ? m_keys[i-1] : keyFrom);
                const ePathKey &wpNext = (i < (eInt)m_keys.size()-2 ? m_keys[i+2] : keyTo);
                const eF32 t = (time-keyFrom.time)/(keyTo.time-keyFrom.time);

                if (keyFrom.interpol == ePI_LINEAR)
                    return eLerp(keyFrom.val, keyTo.val, t);
                else if (keyFrom.interpol == ePI_STEP)
                    return keyFrom.val;
                else
                    return _catmullRom(t, wpPrev.val, keyFrom.val, keyTo.val, wpNext.val);
            }
        }
    }

    eASSERT(eFALSE);
    return 0.0f;
}

void ePath::addKey(eF32 time, eF32 val, ePathKeyInterpol interpol)
{
    eASSERT(time >= 0.0f);

    ePathKey key;
    key.interpol = interpol;
    key.time = time;
    key.val = val;
#ifdef eEDITOR
    key.ownerPath = this;
    key.selected = eFALSE;
#endif
    _insertKey(key);
}

void ePath::removeKey(eU32 index)
{
    eASSERT(index < m_keys.size());
    //@@m_keys.removeAt(index);
    m_keys.erase(m_keys.begin() + index);
}

void ePath::clear()
{
    m_keys.clear();
}

void ePath::setLoopMode(ePathLoopMode pcm)
{
    m_loopMode = pcm;
}

ePathLoopMode ePath::getLoopMode() const
{
    return m_loopMode;
}

const ePathKey & ePath::getKeyByIndex(eU32 index) const
{
    return m_keys[index];
}

ePathKey & ePath::getKeyByIndex(eU32 index)
{
    return m_keys[index];
}

const ePathKey * ePath::getKeyByTime(eF32 time) const
{
    const ePathKey *key = nullptr;

    for (eU32 i=0; i<m_keys.size(); i++)
    {
        if (m_keys[i].time > time)
            break;

        key = &m_keys[i];
    }

    return key;
}

eU32 ePath::getKeyCount() const
{
    return m_keys.size();
}

eF32 ePath::getDuration() const
{
    return getEndTime()-getStartTime();
}

eF32 ePath::getStartTime() const
{
    if (m_keys.size() == 0)
        return 0.0f;

    return m_keys[0].time;
}

eF32 ePath::getEndTime() const
{
    if (m_keys.size() == 0)
        return 0.0f;

    return m_keys[m_keys.size()-1].time;
}

eBool ePath::operator == (const ePath &p) const
{
    return (m_keys == p.m_keys);
}

eBool ePath::operator != (const ePath &p) const
{
    return !(*this == p);
}

void ePath::_insertKey(const ePathKey &key)
{
    for (eU32 i=0; i<m_keys.size(); i++)
    {
        if (m_keys[i].time > key.time)
        {
            //@@m_keys.insert(i, key);
                std::vector<ePathKey>::iterator it; it;
                it = m_keys.begin();
                m_keys.insert(it+i, key);

            return;
        }
    }

    // no key found => append
    m_keys.push_back(key);
}

eF32 ePath::_catmullRom(eF32 t, eF32 v0, eF32 v1, eF32 v2, eF32 v3) const
{
    const eF32 tt = t*t;
    return 0.5f*((2.0f*v1)+(-v0+v2)*t+(2.0f*v0-5.0f*v1+4.0f*v2-v3)*tt+(-v0+3.0f*(v1-v2)+v3)*tt*t);
}

// implementation of 4-dimensional path

Vector4 ePath4::evaluate(eF32 time) const
{
    return Vector4(m_subPaths[0].evaluate(time),
                    m_subPaths[1].evaluate(time),
                    m_subPaths[2].evaluate(time),
                    m_subPaths[3].evaluate(time));
}

Vector4 ePath4::evaluateUnitTime(eF32 time) const
{
	return evaluate(getStartTime()+time*(getEndTime()-getStartTime()));
}

eBool ePath4::isEmpty() const
{
    for (eU32 i=0; i<4; i++)
        if (m_subPaths[i].getKeyCount() > 0)
            return eFALSE;

    return eTRUE;
}

const ePath & ePath4::getSubPath(eU32 index) const
{
    eASSERT(index < 4);
    return m_subPaths[index];
}

ePath & ePath4::getSubPath(eU32 index)
{
    eASSERT(index < 4);
    return m_subPaths[index];
}

eArray<ePathKey *> ePath4::getAllKeys()
{
    eArray<ePathKey *> keys;
    for (eU32 i=0; i<4; i++)
        for (eU32 j=0; j<m_subPaths[i].getKeyCount(); j++)
            keys.push_back(&m_subPaths[i].getKeyByIndex(j));
    
    return keys;
}

eF32 ePath4::getDuration() const
{
    return getEndTime()-getStartTime();
}

eF32 ePath4::getStartTime() const
{
    const eF32 s0 = eMin(m_subPaths[0].getStartTime(), m_subPaths[1].getStartTime());
    const eF32 s1 = eMin(m_subPaths[2].getStartTime(), m_subPaths[3].getStartTime());
    return eMin(s0, s1);
}

eF32 ePath4::getEndTime() const
{
    const eF32 e0 = eMax(m_subPaths[0].getEndTime(), m_subPaths[1].getEndTime());
    const eF32 e1 = eMax(m_subPaths[2].getEndTime(), m_subPaths[3].getEndTime());
    return eMax(e0, e1);
}

eBool ePath4::operator == (const ePath4 &p) const
{
    return (m_subPaths[0] == p.m_subPaths[0] &&
            m_subPaths[1] == p.m_subPaths[1] &&
            m_subPaths[2] == p.m_subPaths[2] &&
            m_subPaths[3] == p.m_subPaths[3]);
}

eBool ePath4::operator != (const ePath4 &p) const
{
    return !(*this == p);
}

// implementation of path sampler

void ePath4Sampler::sample(const ePath4 &path, eU32 samplesPerSec)
{
    if (samplesPerSec < 1)
        return;

    const eU32 sampleCount = eMax(1, eFtoL(path.getEndTime()*samplesPerSec));
    const eF32 step = 1.0f/(eF32)samplesPerSec;

    m_samples.resize(sampleCount+1);
    m_stepInv = 1.0f/step;

    for (eU32 i=0; i<m_samples.size(); i++)
    {
        const eF32 t = (eF32)i*step;
        m_samples[i].time = t;
        m_samples[i].values = path.evaluate(t);
    }
}

const Vector4 & ePath4Sampler::evaluate(eF32 time) const
{
    const eU32 index = eClamp(0, (int)(time*m_stepInv), (eInt)m_samples.size()-1);
    //const eU32 index = eClamp(0, static_cast<int>(time*m_stepInv), (eInt)m_samples.size()-1);
    //const eU32 index = eClamp(0, eFtoL(time*m_stepInv), (eInt)m_samples.size()-1);
    return m_samples[index].values;
}

eU32 ePath4Sampler::getSampleCount() const
{
    return m_samples.size();
}

const ePath4Sample & ePath4Sampler::getSample(eU32 index) const
{
    eASSERT(index < m_samples.size());
    return m_samples[index];
}
