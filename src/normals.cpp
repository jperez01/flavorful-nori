//
// Created by juperez01 on 4/19/23.
//

#include "normals.h"
#include <nori/scene.h>

NORI_NAMESPACE_BEGIN

NormalIntegrator::NormalIntegrator(const PropertyList &props) {
    std::cout << "No parameters" << "\n";
}

Color3f
NormalIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    Intersection its;
    if (!scene->rayIntersect(ray, its))
        return Color3f(0.0f);

    Normal3f n = its.shFrame.n.cwiseAbs();

    return Color3f(n.x(), n.y(), n.z());
}

std::string NormalIntegrator::toString() const {
    return "NormalIntegrator[]";
}

NORI_REGISTER_CLASS(NormalIntegrator, "normals");
NORI_NAMESPACE_END