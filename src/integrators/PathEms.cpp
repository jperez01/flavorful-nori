//
// Created by juperez01 on 4/23/23.
//

#include "PathEms.h"
#include <nori/scene.h>
#include <nori/bsdf.h>
#include <nori/warp.h>
#include <nori/emitter.h>

NORI_NAMESPACE_BEGIN
PathEmsIntegrator::PathEmsIntegrator(const PropertyList &props) {

}

Color3f PathEmsIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    Color3f Lo(0.0f), throughput(1.0f);
    float eta = 1.0f;
    Ray3f someRay(ray);

    int bounces = 0;
    Intersection its;
    while(scene->rayIntersect(someRay, its)) {
        if (its.mesh->isEmitter() && Frame::cosTheta(its.toLocal(-someRay.d)) > 0) {
            EmitterQueryRecord lRecE(someRay.o);
            lRecE.normal = its.shFrame.n;
            lRecE.pos = its.p;

            Lo += throughput * its.mesh->getEmitter()->sample(lRecE, sampler);
        }
        if (its.mesh->getBSDF()->isDiffuse()) {
            float light_pdf;
            Emitter *light = scene->sampleEmittedLight(sampler->next1D(), light_pdf);

            EmitterQueryRecord emitterQueryRecord(its.p);
            Color3f Le = light->sample(emitterQueryRecord, sampler);

            Ray3f otherRay(its.p, emitterQueryRecord.wi);
            Intersection it_shadow;
            if (!scene->rayIntersect(otherRay)) {
                BSDFQueryRecord record(its.toLocal(-someRay.d),
                                       its.toLocal(emitterQueryRecord.wi), ESolidAngle);

                Color3f brdf = its.mesh->getBSDF()->eval(record);
                float cosTheta = std::fmax(its.shFrame.n.dot(emitterQueryRecord.wi), 0.0f);

                Lo += cosTheta * brdf * Le / (light->pdf(emitterQueryRecord) * light_pdf);
            }
        }

        BSDFQueryRecord queryRecord(its.toLocal(-someRay.d));
        throughput *= its.mesh->getBSDF()->sample(queryRecord, sampler->next2D());
        eta *= queryRecord.eta;

        if (bounces > 3) {
            float probability = std::fmin(throughput.maxCoeff() * eta * eta, 0.99f);
            if (sampler->next1D() > probability)
                break;

            throughput /= probability;
        }
        someRay = Ray3f(its.p, its.toWorld(queryRecord.wo));
        bounces++;
    }

    return Lo;
}

std::string PathEmsIntegrator::toString() const {
    return std::string();
}
    NORI_REGISTER_CLASS(PathEmsIntegrator, "path_ems")
NORI_NAMESPACE_END