//
// Created by juperez01 on 4/23/23.
//

#ifndef NORI_PATHEMS_H
#define NORI_PATHEMS_H

#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN

    class PathEmsIntegrator : public Integrator {
    public:
        PathEmsIntegrator(const PropertyList &props);

        Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

        std::string toString() const;
    };

NORI_NAMESPACE_END


#endif //NORI_PATHEMS_H
