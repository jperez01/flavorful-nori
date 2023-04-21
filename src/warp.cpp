/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

float Warp::tentPdfInverse(float value) {
    return value < 0.5 ? sqrt(value * 2) - 1 : 1 - sqrt(2 - 2 * value);
}

Point2f Warp::squareToTent(const Point2f &sample) {
    return Point2f(tentPdfInverse(sample.x()), tentPdfInverse(sample.y()));
}

float Warp::squareToTentPdf(const Point2f &p) {
    return (1 - std::abs(p.x())) * (1 - std::abs(p.y()));
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    float r = sqrt(sample.x()), theta = 2 * M_PI * sample.y();

    return Point2f(r * cos(theta), r * sin(theta));
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    float x2 = p.x() * p.x(), y2 = p.y() * p.y();
    return (std::sqrt(x2 + y2) < 1) ? INV_PI : 0.0f;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    float z = 1 - 2 * sample.x();
    float r = sqrt(std::max(0.0f, 1 - z * z));
    float phi = 2 * M_PI * sample.y();

    return Vector3f(r * cos(phi), r * sin(phi), z);
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    return 0.25 * INV_PI;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    float theta = acos(sample.x()), phi = 2 * M_PI * sample.y();
    float sinTheta = sin(theta);
    return Vector3f(
            sinTheta * cos(phi),
            sinTheta * sin(phi),
            cos(theta)
    );
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    return (v.x() * v.x() + v.y() * v.y() + v.z() * v.z() <= 1.0f && v.z() >= 0)
           ? 0.5 * INV_PI
           : 0.0f;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    float phi = asin(sqrt(sample.x())), theta = 2 * M_PI * sample.y();

    return Vector3f(
    sin(phi) * cos(theta),
    sin(phi) * sin(theta),
    cos(phi)
    );
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    return (v.x() * v.x() + v.y() * v.y() + v.z() * v.z() <= 1.0f && v.z() >= 0)
           ? v.z() * INV_PI
           : 0.0f;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    float x, y, z, phi, logSample, cos_theta, sin_theta, tan2theta;

    phi = 2 * M_PI * sample[0];
    logSample = std::log(std::max((float)0, 1-sample[1]));
    // logSample can be a number from -inf to 0.
    tan2theta = -alpha * alpha * logSample;
    // tan2theta can be a number from inf to 0.
    cos_theta = (float)1 / std::sqrt(1 + tan2theta);
    // cos_theta can be a number from 0 to 1.
    sin_theta = std::sqrt(std::max((float)0, (float)1. - cos_theta * cos_theta));

    // Now from polar to cartesian
    x = sin_theta * std::cos(phi);
    y = sin_theta * std::sin(phi);
    z = cos_theta;

    return Vector3f(x,y,z);
}

float Warp::squareToBeckmannPdf(const Vector3f &v, float alpha) {
    float tan2theta, cos_theta, alpha2, prob, denom;
    alpha2 = alpha * alpha;
    // From z we have cos theta and tan
    cos_theta = v[2];

    if (cos_theta == 0) {
        prob = 0;
    }else {
        tan2theta = ((float)1. / (cos_theta * cos_theta))-1;
        denom = M_PI * alpha2 * cos_theta * cos_theta * cos_theta;
        prob = std::exp(-tan2theta / alpha2) / denom;
    }

    return (v[2] >= 0 && (std::abs(std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) - 1) < 0.0001)) ? prob : 0.0f;
}

    Point2f Warp::squareToUniformTriangle(const Point2f &sample) {
        float spx = std::sqrt(sample.x());
        float py = sample.y();

        return Point2f(1 - spx, py * spx);
    }

    float Warp::squareToUniformTrianglePdf(const Point2f &p) {
        return ((p.array() >= 0).all() && (p.array() <= 1).all() && p[0]+p[1] < 1) ? 2.0f : 0.0f;
    }

NORI_NAMESPACE_END
