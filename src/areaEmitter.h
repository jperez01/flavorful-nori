//
// Created by juperez01 on 4/21/23.
//

#ifndef NORI_AREAEMITTER_H
#define NORI_AREAEMITTER_H

#include <nori/emitter.h>
#include <nori/proplist.h>
#include <nori/scene.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter {
public:
    AreaLight(const PropertyList &props);

    Color3f sample(EmitterQueryRecord &record, const Point2f &sample) const;
    Color3f sample(EmitterQueryRecord &record, Sampler *sampler);

    bool rayIntersect(const Scene* scene, Ray3f &shadowRay, Intersection &its) const {
        if(!scene->rayIntersect(shadowRay, its)) {
            return false;
        }
        if(its.mesh->getEmitter() == this)
            return false;

        return true;
    }
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay) const {
        Intersection its;
        return rayIntersect(scene, shadowRay, its);
    }

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
