//
// Created by juperez01 on 4/19/23.
//

#ifndef NORI_NORMALS_H
#define NORI_NORMALS_H

#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN

class NormalIntegrator : public Integrator {
public:
    NormalIntegrator(const PropertyList &props);

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

    std::string toString() const;

protected:
    std::string m_myProperty;
};

NORI_NAMESPACE_END

#endif //NORI_NORMALS_H
