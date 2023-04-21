//
// Created by juperez01 on 4/21/23.
//

#ifndef NORI_WHITTED_H
#define NORI_WHITTED_H

#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN

class WhittedIntegrator : public Integrator {
public:
    WhittedIntegrator(const PropertyList &props);

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

    std::string toString() const;
};

NORI_NAMESPACE_END


#endif //NORI_WHITTED_H
