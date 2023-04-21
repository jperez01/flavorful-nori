//
// Created by juperez01 on 4/20/23.
//

#include "aoIntegrator.h"
#include <nori/scene.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

AoIntegrator::AoIntegrator(const nori::PropertyList &props) {
    std::cout << "No Props" << "\n";
}

Color3f AoIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    Intersection its;

    if (!scene->rayIntersect(ray, its))
        return Color3f(0.0f);

    Vector3f wi = Warp::squareToCosineHemisphere(sampler->next2D());
    Vector3f direction = its.shFrame.toWorld(wi).normalized();

    float pdf = Warp::squareToCosineHemispherePdf(wi);
    float li = direction.dot(its.shFrame.n.normalized()) * INV_PI / pdf;

    Ray3f shadowRay(its.p, direction);
    if (scene->rayIntersect(shadowRay))
        return Color3f(0.0f);

    return Color3f(clamp(li, 0.0f, 1.0f));
}

std::string AoIntegrator::toString() const {
    return tfm::format("AOIntegrator[]");
}

NORI_REGISTER_CLASS(AoIntegrator, "ao")
NORI_NAMESPACE_END
