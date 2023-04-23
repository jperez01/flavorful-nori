/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class Microfacet : public BSDF {
public:
    Microfacet(const PropertyList &propList) {
        /* RMS surface roughness */
        m_alpha = propList.getFloat("alpha", 0.1f);

        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);

        /* Albedo of the diffuse base material (a.k.a "kd") */
        m_kd = propList.getColor("kd", Color3f(0.5f));

        /* To ensure energy conservation, we must scale the 
           specular component by 1-kd. 

           While that is not a particularly realistic model of what 
           happens in reality, this will greatly simplify the 
           implementation. Please see the course staff if you're 
           interested in implementing a more realistic version 
           of this BRDF. */
        m_ks = 1 - m_kd.maxCoeff();
    }

    /// Evaluate the BRDF for the given pair of directions
    Color3f eval(const BSDFQueryRecord &bRec) const {
        if (Frame::cosTheta(bRec.wi) <= 0.0f || Frame::cosTheta(bRec.wo) <= 0.0f)
            return Color3f(0.0f);

    	Color3f diffusePart = m_kd / M_PI;

        Vector3f sumVector = bRec.wi + bRec.wo;
        Vector3f wh = sumVector.normalized();

        float fCoefficient = fresnel(wh.dot(bRec.wi), m_extIOR, m_intIOR);

        Vector3f z_axis(0, 0, 1);
        float cosThetai = bRec.wi.dot(z_axis), cosThetao = bRec.wo.dot(z_axis),
            cosThetah = wh.dot(z_axis);

        float Gih = GShadowTerm(bRec.wi, wh), Goh = GShadowTerm(bRec.wo, wh);

        float d = Warp::squareToBeckmannPdf(wh, m_alpha);

        Color3f specularPart = m_ks * (d * fCoefficient * Gih * Goh) / (4.0f * cosThetah * cosThetai * cosThetao);

        return diffusePart + specularPart;
    }

    float GShadowTerm(Vector3f wv, Vector3f wh) const {
        float cosThetav = Frame::cosTheta(wv);
        float c = wv.dot(wh) / cosThetav;
        float x = c > 0 ? 1 : 0;

        float b = 1.0f / (m_alpha * tan(acos(cosThetav)));
        float complexArg = 1.0f;
        if (b < 1.6) {
            float b2 = b * b;
            complexArg = (3.535f * b + 2.181f * b2) / (1 + 2.276 * b + 2.577 * b2);
        }

        return x * complexArg;
    }

    /// Evaluate the sampling density of \ref sample() wrt. solid angles
    float pdf(const BSDFQueryRecord &bRec) const {
        if (Frame::cosTheta(bRec.wi) <= 0.0f || Frame::cosTheta(bRec.wo) <= 0.0f)
            return 0.0f;

    	Vector3f wh = (bRec.wi + bRec.wo).normalized();

        float D = Warp::squareToBeckmannPdf(wh, m_alpha);
        float J = 1 / (4.0f * wh.dot(bRec.wo));

        float cosThetao = Frame::cosTheta(bRec.wo);

        return m_ks * D * J + (1 - m_ks) * cosThetao * INV_PI;
    }

    /// Sample the BRDF
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &_sample) const {
        if (Frame::cosTheta(bRec.wi) <= 0.0f)
            return Color3f(0.0f);
        bRec.measure = ESolidAngle;

        if (_sample.x() < m_ks) {
            Point2f sampleReuse(_sample.x() / m_ks, _sample.y());
            Normal3f n = Warp::squareToBeckmann(sampleReuse, m_alpha);

            bRec.wo = 2.0f * bRec.wi.dot(n) * n - bRec.wi;
            bRec.wo.normalize();
        } else {
            Point2f sampleReuse((_sample.x() - m_ks) / (1.0f - m_ks), _sample.y());
            bRec.wo = Warp::squareToCosineHemisphere(sampleReuse);
        }
        bRec.eta = m_extIOR / m_intIOR;

        if (Frame::cosTheta(bRec.wo) < 0.0f)
            return Color3f(0.0f);

        return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);

        // Note: Once you have implemented the part that computes the scattered
        // direction, the last part of this function should simply return the
        // BRDF value divided by the solid angle density and multiplied by the
        // cosine factor from the reflection equation, i.e.
        // return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
    }

    bool isDiffuse() const {
        /* While microfacet BRDFs are not perfectly diffuse, they can be
           handled by sampling techniques for diffuse/non-specular materials,
           hence we return true here */
        return true;
    }

    std::string toString() const {
        return tfm::format(
            "Microfacet[\n"
            "  alpha = %f,\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  kd = %s,\n"
            "  ks = %f\n"
            "]",
            m_alpha,
            m_intIOR,
            m_extIOR,
            m_kd.toString(),
            m_ks
        );
    }
private:
    float m_alpha;
    float m_intIOR, m_extIOR;
    float m_ks;
    Color3f m_kd;
};

NORI_REGISTER_CLASS(Microfacet, "microfacet");
NORI_NAMESPACE_END
