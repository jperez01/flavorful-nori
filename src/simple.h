//
// Created by juperez01 on 4/20/23.
//

#ifndef NORI_SIMPLE_H
#define NORI_SIMPLE_H

#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN

class SimpleIntegrator : public Integrator {
public:
    SimpleIntegrator(const PropertyList &props);

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;
    std::string toString() const;

private:
    Point3f m_position;
    Color3f m_energy;
    float denom;
};
NORI_NAMESPACE_END

#endif //NORI_SIMPLE_H
