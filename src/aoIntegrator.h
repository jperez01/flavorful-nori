//
// Created by juperez01 on 4/20/23.
//

#ifndef NORI_AOINTEGRATOR_H
#define NORI_AOINTEGRATOR_H

#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN

class AoIntegrator : public Integrator {
public:
    AoIntegrator(const PropertyList &props);

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

    std::string toString() const;
};

NORI_NAMESPACE_END

#endif //NORI_AOINTEGRATOR_H
