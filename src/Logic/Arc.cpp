/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2017-2020, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Arc.hpp"
#include <Math/Math.hpp>

// See http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes for a more precise description of the implementation
void getPointsOnSvgEllipticalArc(std::vector<glm::vec2> &points, const SvgEllipticalArcDataIn &in) {
    float x1 = in.x1; // Starting point of the arc
    float y1 = in.y1;
    float x2 = in.x2; // End point of the arc
    float y2 = in.y2;
    float rx = sgl::abs(in.rx); // x radius of the ellipse
    float ry = sgl::abs(in.ry); // y radius of the ellipse
    float rotx = in.deg / 180.0f * sgl::PI; // Rotation angle relative to the x-axis
    int fa = sgl::floatEquals(in.fa, 0.0f) ? 0 : 1; // "Large arc flag"
    int fs = sgl::floatEquals(in.fs, 0.0f) ? 0 : 1; // "Sweep flag"

    float dx = x1 - x2;
    float dy = y1 - y2;
    float dist = sgl::sqrt(dx*dx + dy*dy);
    if (dist < 0.0001f) {
        // Starting point and end point identical!
        return;
    } else if (rx < 0.0001f || ry < 0.0001f) {
        // The arc equals a line - abort
        points.push_back(glm::vec2(x2, y2));
        return;
    }

    // Compute cosine and sine of the angle
    // -> See http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes
    float sinx = sgl::sin(rotx);
    float cosx = sgl::cos(rotx);

    // Convert to "center parameterization".
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

    // Step 1: Compute x1' and y1'
    float x1p = cosx * dx / 2.0f + sinx * dy / 2.0f;
    float y1p = -sinx * dx / 2.0f + cosx * dy / 2.0f;
    dist = sgl::sqr(x1p)/sgl::sqr(rx) + sgl::sqr(y1p)/sgl::sqr(ry);
    if (dist > 1) {
        dist = sgl::sqrt(dist);
        rx *= dist;
        ry *= dist;
    }

    // Step 2: Compute cx' and cy'
    float factor = 0.0f;
    float radNum = sgl::sqr(rx)*sgl::sqr(ry) - sgl::sqr(rx)*sgl::sqr(y1p) - sgl::sqr(ry)*sgl::sqr(x1p);
    float radDenom = sgl::sqr(rx)*sgl::sqr(y1p) + sgl::sqr(ry)*sgl::sqr(x1p);
    if (radNum < 0.0f)
        radNum = 0.0f;
    if (radDenom > 0.0f)
        factor = sgl::sqrt(radNum / radDenom);
    if (fa == fs)
        factor = -factor;
    float cxp = factor * rx * y1p / ry;
    float cyp = factor * -ry * x1p / rx;

    // Step 3: Compute cx and cy from cx' and cy'
    float cx = cosx*cxp - sinx*cyp + (x1 + x2)/2.0f;
    float cy = sinx*cxp + cosx*cyp + (y1 + y2)/2.0f;

    // Step 4: Compute the start angle and the length of the arc angle
    float ux = (x1p - cxp) / rx;
    float uy = (y1p - cyp) / ry;
    float vx = (-x1p - cxp) / rx;
    float vy = (-y1p - cyp) / ry;
    float startAngle = sgl::vectorAngle(glm::vec2(1.0f, 0.0f), glm::vec2(ux, uy));
    float arcAngle = sgl::vectorAngle(glm::vec2(ux, uy), glm::vec2(vx, vy)); // Length of the arc in radians

    if (fa == 1) {
        // Choose the longer arc
        if (arcAngle > 0.0f) {
            arcAngle = arcAngle - 2*sgl::PI;
        } else {
            arcAngle = 2*sgl::PI + arcAngle;
        }
    }

    // Compute the fineness of the arc (the number of segments)
    int ellipseSegments = getNumCircleSegments((rx + ry) / 2.0f);//16;
    /*if (Settings::get()->getValue("circle-refinement-factor").length() > 0) {
        ellipseSegments = ellipseSegments * sgl::sqr(Settings::get()->getFloatValue("circle-refinement-factor"));
    }*/
    int numSegments = sgl::max(2, int(sgl::abs(arcAngle)/(sgl::TWO_PI)*ellipseSegments));

    std::vector<glm::vec2> arcPoints;
    getPointsOnEllipseArc(arcPoints, glm::vec2(cx, cy), rx, ry, startAngle, arcAngle, numSegments);
    points.insert(points.end(), arcPoints.begin(), arcPoints.end());
}

// direction: +1 -> CCW, -1 -> CW
void getPointsOnCircleArc(
        std::vector<glm::vec2> &points, const glm::vec2 &center, float radius,
        const glm::vec2 &start, const glm::vec2 &end, int direction) {
    // Compute the start and end angles with the arc tangent of the point vectors (-> angles on the unit circle)
    glm::vec2 normalStart = glm::normalize(start-center);
    glm::vec2 normalEnd = glm::normalize(end-center);
    float startAngle = sgl::atan2(normalStart.y, normalStart.x);
    float endAngle = sgl::atan2(normalEnd.y, normalEnd.x);
    float arcAngle = 0;

    // Is the rotation Ist die Drehung im counterclockwise (i.e.: do start and end angles need to be swapped?)
    if (direction == -1) {
        float temp = endAngle;
        endAngle = startAngle;
        startAngle = temp;
    }

    // Reduce the end angle by the period 2*Pi as long as it is smaller than the start angle.
    while (endAngle < startAngle) {
        endAngle += sgl::TWO_PI;
    }
    arcAngle = endAngle - startAngle; // Compute the total angle

    // Compute the fineness of the arc (the number of segments)
    int circleSegments = getNumCircleSegments(radius);//16;
    /*if (Settings::get()->getValue("circle-refinement-factor").length() > 0) {
        circleSegments = circleSegments * sgl::sqr(Settings::get()->getFloatValue("circle-refinement-factor"));
    }*/
    int numSegments = int(sgl::abs(arcAngle)/(sgl::TWO_PI)*circleSegments);

    // Does the arc equal a line?
    if (numSegments < 2) {
        points.push_back(end);
    } else {
        getPointsOnCircleArc(points, center, radius, startAngle, arcAngle, numSegments);
    }
}

void getPointsOnCircle(std::vector<glm::vec2> &points, const glm::vec2 &center, float radius, int numSegments) {
    float theta = 2 * 3.1415926f / float(numSegments);
    float tangetialFactor = tan(theta);
    float radialFactor = cos(theta);
    float x = radius; // Starting at an angle of 0
    float y = 0;

    for(int i = 0; i < numSegments; i++) {
        points.push_back(glm::vec2(x + center.x, y + center.y));

        // Compute the tangent vector and correct it using the radial factor.
        float tx = -y;
        float ty = x;
        x += tx * tangetialFactor;
        y += ty * tangetialFactor;
        x *= radialFactor;
        y *= radialFactor;
    }
}

void getPointsOnCircleArc(
        std::vector<glm::vec2> &points, const glm::vec2 &center, float radius, float startAngle, float arcAngle,
        int numSegments) {
    float theta = arcAngle / float(numSegments - 1); // The arc is open => numSegments - 1
    float tangetialFactor = tan(theta);
    float radialFactor = cos(theta);
    float x = radius * cos(startAngle);
    float y = radius * sin(startAngle);

    for(int i = 0; i < numSegments; i++) {
        points.push_back(glm::vec2(x + center.x, y + center.y));

        // Compute the tangent vector and correct it using the radial factor.
        float tx = -y;
        float ty = x;
        x += tx * tangetialFactor;
        y += ty * tangetialFactor;
        x *= radialFactor;
        y *= radialFactor;
    }
}

void getPointsOnEllipse(
        std::vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy, int numSegments) {
    float theta = 2 * 3.1415926f / float(numSegments);
    float tangetialFactor = tan(theta);
    float radialFactor = cos(theta);
    float x = 1.0f; // Starting at an angle of 0
    float y = 0;

    for(int i = 0; i < numSegments; i++) {
        points.push_back(glm::vec2(x*radiusx + center.x, y*radiusy + center.y));

        // Compute the tangent vector and correct it using the radial factor.
        float tx = -y;
        float ty = x;
        x += tx * tangetialFactor;
        y += ty * tangetialFactor;
        x *= radialFactor;
        y *= radialFactor;
    }
}

void getPointsOnEllipseArc(
        std::vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy,
        float startAngle, float arcAngle, int numSegments) {
    float theta = arcAngle / float(numSegments - 1); // The arc is open => numSegments - 1
    float tangetialFactor = tan(theta);
    float radialFactor = cos(theta);
    float x = cos(startAngle);
    float y = sin(startAngle);

    for(int i = 0; i < numSegments; i++) {
        points.push_back(glm::vec2(x*radiusx + center.x, y*radiusy + center.y));

        // Compute the tangent vector and correct it using the radial factor.
        float tx = -y;
        float ty = x;
        x += tx * tangetialFactor;
        y += ty * tangetialFactor;
        x *= radialFactor;
        y *= radialFactor;
    }
}


int getNumCircleSegments(float radius) {
    /* We want to find the smallest number of segments needed so that
     * the delta between the circle approximation arc line center and
     * the real arc is smaller than or equal to the delta requested.
     * Equatation: delta <= r * (1 - cos(pi/n))
     */
    float delta = 3.0f;
    float val = sgl::ceil(sgl::PI / sgl::acos(1 - delta/radius));
    //float val = factor * sgl::sqrt(radius);
    return sgl::clamp((int)val, 16, 128);
}

