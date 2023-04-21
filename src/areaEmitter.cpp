//
// Created by juperez01 on 4/21/23.
//

#include "areaEmitter.h"

NORI_NAMESPACE_BEGIN

AreaLight::AreaLight(const PropertyList &props) {
    m_radiance = props.getColor("radiance", Color3f(0.0f));
}

Color3f AreaLight::sample(EmitterQueryRecord &record, const Point2f &sample) const {
    m_mesh->samplePosition(sample, record.p, record.n, record.uv);

    record.dist = (record.p - record.ref).norm();
    record.wi = (record.p - record.ref) / record.dist;
    record.pdf = pdf(record);

    return eval(record);
}

float AreaLight::pdf(const EmitterQueryRecord &record) const {
    float prob_s = m_mesh->pdf(record.p);
    float denom = std::abs(record.n.dot(record.wi));
    float norm = record.dist;

    if (denom < 0.0001f)
        denom = 0.0001f;

    float pdf_value = prob_s * norm * norm / denom;

    if (isnan(pdf_value))
        pdf_value = 0.0f;

    return pdf_value;
}

Color3f AreaLight::eval(const EmitterQueryRecord &record) const {
    float dot = record.n.dot(record.wi);
    if (dot <= 0.0f) {
        return m_radiance;
    }

    return Color3f(0.0f);
}

std::string AreaLight::toString() const {
    return tfm::format(
            "AreaLight[]"
            );
}

void AreaLight::setParent(NoriObject *parent) {
    auto type = parent->getClassType();
    if (type == EMesh)
        m_mesh = static_cast<Mesh*>(parent);
}

    NORI_REGISTER_CLASS(AreaLight, "area")
NORI_NAMESPACE_END