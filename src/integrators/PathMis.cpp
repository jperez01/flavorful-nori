//
// Created by juperez01 on 4/23/23.
//

#include "PathMis.h"
#include <nori/scene.h>
#include <nori/bsdf.h>
#include <nori/warp.h>
#include <nori/emitter.h>

NORI_NAMESPACE_BEGIN
    PathMisIntegrator::PathMisIntegrator(const PropertyList &props) {

    }

    Color3f PathMisIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Color3f Lo(0.0f), throughput(1.0f);
        float eta = 1.0f;
        Ray3f someRay(ray);

        bool countLight = true;
        int bounces = 0;
        Intersection its;
        while(scene->rayIntersect(someRay, its)) {
            if (its.mesh->isEmitter() && Frame::cosTheta(its.toLocal(-someRay.d)) > 0) {
                EmitterQueryRecord lRecE(someRay.o);
                lRecE.n = its.shFrame.n;
                lRecE.p = its.p;

                Color3f Le = its.mesh->getEmitter()->sample(lRecE, sampler->next2D());

                BSDFQueryRecord record(its.toLocal(-someRay.d),
                                       its.toLocal(lRecE.wi), ESolidAngle);
                float brdfPdf = its.mesh->getBSDF()->pdf(record);
                float solidAnglePdf = clamp(lRecE.pdf * lRecE.dist * lRecE.dist
                                            / abs(-lRecE.wi.dot(lRecE.n)), 0.0f, 10000.0f);
                float weightedPdf = lRecE.pdf / (lRecE.pdf + brdfPdf);

                Lo += weightedPdf * throughput * Le;
            }

            BSDFQueryRecord queryRecord(its.toLocal(-someRay.d));
            Color3f bsdfColor = its.mesh->getBSDF()->sample(queryRecord, sampler->next2D());
            throughput *= bsdfColor;
            eta *= queryRecord.eta;

            float brdfPdf = its.mesh->getBSDF()->pdf(queryRecord);

            if (its.mesh->getBSDF()->isDiffuse()) {
                float light_pdf;
                const Emitter *light = scene->sampleEmittedLight(sampler->next1D(), light_pdf);

                EmitterQueryRecord emitterQueryRecord(its.p);
                Color3f Le = light->sample(emitterQueryRecord, sampler->next2D());

                Ray3f otherRay(its.p, emitterQueryRecord.wi);
                Intersection it_shadow;
                if (!scene->rayIntersect(otherRay)) {
                    BSDFQueryRecord record(its.toLocal(-someRay.d),
                                           its.toLocal(emitterQueryRecord.wi), ESolidAngle);

                    float cosTheta = std::fmax(its.shFrame.n.dot(emitterQueryRecord.wi), 0.0f);

                    float solidAnglePdf = clamp(emitterQueryRecord.pdf * emitterQueryRecord.dist * emitterQueryRecord.dist
                            / std::abs(-emitterQueryRecord.wi.dot(emitterQueryRecord.n)), 0.0f, 10000.0f);
                    float weightedPdf = (emitterQueryRecord.pdf + brdfPdf > 0.0f) ? brdfPdf / (solidAnglePdf + brdfPdf) : brdfPdf;

                    Lo += weightedPdf * bsdfColor * Le * cosTheta;
                }
            }

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

    std::string PathMisIntegrator::toString() const {
        return std::string();
    }
    NORI_REGISTER_CLASS(PathMisIntegrator, "path_mis")
NORI_NAMESPACE_END