//
// Created by juperez01 on 4/20/23.
//

#include "simple.h"
#include <nori/scene.h>

NORI_NAMESPACE_BEGIN

SimpleIntegrator::SimpleIntegrator(const PropertyList &props) {
    m_position = props.getPoint("position", Point3f(0.2f));
    m_energy = props.getColor("energy", Color3f(0.5f));
    denom = 0.25f * INV_PI * INV_PI;
}

Color3f SimpleIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    Intersection its;
    if (!scene->rayIntersect(ray, its))
        return Color3f(0.0f);

    Normal3f normal = its.shFrame.n.normalized();
    Vector3f direction = (m_position - its.p);
    float d = direction.norm();
    direction.normalize();

    float cosTheta = normal.dot(direction);
    Ray3f shadowRay(its.p, direction);
    if (scene->rayIntersect(shadowRay))
        return Color3f(0.0f);

    Color3f li = m_energy * std::fmax(0.0f, cosTheta) * denom / (d * d);
    return li.clamp();

}

std::string SimpleIntegrator::toString() const {
    return tfm::format("SimpleIntegrator[\n"
    "position: \"%s\"\n"
    "energy: \"%s\"\n", m_position, m_energy);
}

NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END
