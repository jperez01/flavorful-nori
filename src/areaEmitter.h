//
// Created by juperez01 on 4/21/23.
//

#ifndef NORI_AREAEMITTER_H
#define NORI_AREAEMITTER_H

#include <nori/emitter.h>
#include <nori/proplist.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter {
public:
    AreaLight(const PropertyList &props);

    Color3f sample(EmitterQueryRecord &record, const Point2f &sample) const;

    float pdf(const EmitterQueryRecord &record) const;

    Color3f eval(const EmitterQueryRecord &record) const;

    void setParent(NoriObject *parent);

    std::string toString() const;
    Color3f getRadiance() const { return m_radiance; }

private:
    Color3f m_radiance;
};

NORI_NAMESPACE_END

#endif //NORI_AREAEMITTER_H
