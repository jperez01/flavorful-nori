/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include <nori/object.h>
#include <nori/mesh.h>

NORI_NAMESPACE_BEGIN

enum EmitterType {
    AREA_LIGHT
};

struct EmitterQueryRecord {
    const Emitter* emitter;

    Point3f ref;
    Point3f p;
    Normal3f n;
    Point2f uv;

    float pdf;
    Vector3f wi;
    float dist;

    EmitterQueryRecord() : emitter(nullptr) {}
    EmitterQueryRecord(const Point3f &ref) : ref(ref) {}
    EmitterQueryRecord(const Emitter* emitter,
                       const Point3f& ref, const Point3f& p,
                       const Normal3f& n, const Point2f& uv)
                       : emitter(emitter), ref(ref), p(p), n(n), uv(uv){
        wi = p - ref;
        dist = wi.norm();
        wi /= dist;
    }
};

/**
 * \brief Superclass of all emitters
 */
class Emitter : public NoriObject {
public:
    virtual ~Emitter() {}

    virtual Color3f sample(EmitterQueryRecord &record, const Point2f &sample) const = 0;

    virtual float pdf(const EmitterQueryRecord &record) const = 0;

    virtual Color3f eval(const EmitterQueryRecord &record) const = 0;

    virtual Color3f getRadiance() const = 0;

    void setMesh(Mesh *mesh) { m_mesh = mesh; }
    EmitterType getType() const { return m_type; }

    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
    EClassType getClassType() const { return EEmitter; }

protected:
    Mesh *m_mesh;
    EmitterType m_type;
};

NORI_NAMESPACE_END
