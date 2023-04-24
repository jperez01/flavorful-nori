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

    if (its.mesh->getBSDF()->isDiffuse()) {
        float light_pdf;
        Emitter *light = scene->sampleEmittedLight(sampler->next1D(), light_pdf);

        EmitterQueryRecord emitterQueryRecord(its.p);
        Color3f Le = light->sample(emitterQueryRecord, sampler);

        Ray3f otherRay(its.p, emitterQueryRecord.wi);
        Intersection it_shadow;
        if (scene->rayIntersect(otherRay, it_shadow)) {
                return Color3f(0.0f);
        }

        BSDFQueryRecord record(its.toLocal(-ray.d),
                               its.toLocal(emitterQueryRecord.wi), ESolidAngle);

        Color3f brdf = its.mesh->getBSDF()->eval(record);
        float cosTheta = std::fmax(its.shFrame.n.dot(emitterQueryRecord.wi), 0.0f);

        return cosTheta * brdf * Le / light->pdf(emitterQueryRecord) / light_pdf;
    } else {
        if (sampler->next1D() < 0.95f) {
            BSDFQueryRecord bsdfQueryRecord(its.toLocal(-ray.d));
            Color3f brdf = its.mesh->getBSDF()->sample(bsdfQueryRecord, sampler->next2D());

            Ray3f otherRay(its.p, its.toWorld(bsdfQueryRecord.wo));
            return (1.0f / 0.95f) * brdf * Li(scene, sampler, otherRay);
        } else {
            return Color3f(0.0f);
        }
    }
}

std::string WhittedIntegrator::toString() const {
    return std::string();
}

NORI_REGISTER_CLASS(WhittedIntegrator, "whitted")
NORI_NAMESPACE_END
