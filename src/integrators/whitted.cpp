//
// Created by juperez01 on 4/21/23.
//

#include "whitted.h"
#include <nori/scene.h>
#include <nori/bsdf.h>
#include <nori/warp.h>
#include <nori/emitter.h>

NORI_NAMESPACE_BEGIN
WhittedIntegrator::WhittedIntegrator(const PropertyList &props) {

}

Color3f WhittedIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    Intersection its;

    if (!scene->rayIntersect(ray, its))
        return Color3f(0.0f);

    if (its.mesh->isEmitter()) {
        return its.mesh->getEmitter()->getRadiance();
    }

    const std::vector<Emitter *> &lights = scene->getLights();
    EmitterQueryRecord emitterQueryRecord(its.p);
    Color3f Lo(0.0f);
    for (auto &light : lights) {
        Color3f Le = light->sample(emitterQueryRecord, sampler->next2D());

        Ray3f otherRay(its.p, emitterQueryRecord.wi);
        Intersection it_shadow;
        if (scene->rayIntersect(otherRay, it_shadow)) {
            if (it_shadow.t < (emitterQueryRecord.dist - 1.e-5))
                continue;
        }

        BSDFQueryRecord record(its.toLocal(-ray.d),
                               its.toLocal(emitterQueryRecord.wi), ESolidAngle);

        Color3f brdf = its.mesh->getBSDF()->eval(record);
        float cosTheta = std::fmax(its.shFrame.n.dot(emitterQueryRecord.wi), 0.0f);

         Lo += cosTheta * brdf * Le / emitterQueryRecord.pdf;

        // Lo += Le * its.shFrame.n.dot(emitterQueryRecord.wi) * its.mesh->getBSDF()->eval(record);

    }
    return Lo;
}

std::string WhittedIntegrator::toString() const {
    return std::string();
}

NORI_REGISTER_CLASS(WhittedIntegrator, "whitted")
NORI_NAMESPACE_END
