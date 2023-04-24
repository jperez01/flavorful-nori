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
        Color3f lightColor = SamplingLight(scene, sampler, ray), bsdfColor = SamplingBSDF(scene, sampler, ray);
        Color3f someColor = lightColor + bsdfColor;
        return someColor;
    }

    Color3f PathMisIntegrator::SamplingLight(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return 0.f;
        }

        if(!its.mesh->getBSDF()->isDiffuse()) {
            if(drand48() > 0.95)
                return Color3f(0.f);
            else {
                BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));

                Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
                return 1.057 * albedo * SamplingLight(scene, sampler, Ray3f(its.p, its.toWorld(bsdfQ.wo)));
            }
        }

        //preprocessing for caculation
        float light_pdf;
        Emitter *emit = scene->sampleEmittedLight(sampler->next1D(), light_pdf);
        EmitterQueryRecord eqr = EmitterQueryRecord(its.p);
        Color3f Le = emit->sample(eqr, sampler);
        Vector3f distanceVec = eqr.wi.normalized();
        Color3f directColor = Color3f(0.f);

        //for calculatin G(x<->y)
        float objectNormal = abs(its.shFrame.n.dot(distanceVec));

        //check current its.p is emitter() then distnace -> infinite
        if(its.mesh->isEmitter()) {
            if(depth == 0)
                return its.mesh->getEmitter()->Le(eqr);
            else
                return Color3f(0.f);
        }

        //BRDF of given its.p material
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(distanceVec), ESolidAngle);
        Color3f bsdf = its.mesh->getBSDF()->eval(bsdfQ);

        //Pdf value for light (diffuse area light)
        float lightPdf = light_pdf * emit->pdf(eqr);

        //Weighting
        float modifiedLightPdf = lightPdf;

        float bsdfPdf = its.mesh->getBSDF()->pdf(bsdfQ);
        float weight = modifiedLightPdf/(modifiedLightPdf+ bsdfPdf);

        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));

        Ray3f shadowRay(eqr.ref, eqr.wi);
        //MC integral
        if(!emit->rayIntersect(scene, shadowRay)) {
            directColor = objectNormal * Le * bsdf * 1.f/light_pdf;
        }

        if(drand48() < 0.95f && depth < 10)
            return weight * 1.f/0.95 * (directColor + albedo * SamplingLight(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))), depth + 1));
        else
            return Color3f(0.f);
    }

    Color3f PathMisIntegrator::SamplingBSDF(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return 0.f;
        }

        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        Color3f light = Color3f(0.f);
        if(its.mesh->isEmitter()) {
            EmitterQueryRecord erq = EmitterQueryRecord(its.p);
            erq.normal = its.shFrame.n;
            erq.wi = -its.toWorld(bsdfQ.wo);
            light = its.mesh->getEmitter()->Le(erq);
        }

        //Weighting
        float lightPdf = 0.f;
        Intersection lightInsect;
        if(scene->rayIntersect(Ray3f(its.p, its.toWorld((bsdfQ.wo))), lightInsect)) {
            if(lightInsect.mesh->isEmitter()) {
                EmitterQueryRecord eqr = EmitterQueryRecord(lightInsect.p);


                    lightPdf = 1.0f * lightInsect.mesh->getEmitter()->pdf(eqr);
                    Vector3f distanceVec = lightInsect.p - its.p;
                    float distance = distanceVec.dot(distanceVec);
                    lightPdf *= distance;

            }
        }

        float bsdfPdf = its.mesh->getBSDF()->pdf(bsdfQ);
        float weight = bsdfPdf/(bsdfPdf + lightPdf);
        if(bsdfPdf == 0 && lightPdf == 0) {
            weight = 1;
        }

        //russian Roulette
        if(drand48() < 0.95f && depth < 10)
            return weight * 1.f/0.95 * (light + albedo * SamplingBSDF(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))), depth + 1));
        else
            return Color3f(0.f);
    }

    std::string PathMisIntegrator::toString() const {
        return std::string();
    }
    NORI_REGISTER_CLASS(PathMisIntegrator, "path_mis")
NORI_NAMESPACE_END