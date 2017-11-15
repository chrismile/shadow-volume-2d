/*
 * Arc.hpp
 *
 *  Created on: 03.12.2014
 *      Author: Christoph Neuhauser
 */

#ifndef RENDERABLES_FUNCTIONS_ARC_HPP_
#define RENDERABLES_FUNCTIONS_ARC_HPP_

#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct SvgEllipticalArcDataIn {
	SvgEllipticalArcDataIn() : x1(0), y1(0), x2(0), y2(0), rx(0), ry(0), deg(0), fa(0), fs(0) {}
	SvgEllipticalArcDataIn(float _x1, float _y1, float _x2, float _y2, float _rx, float _ry, float _deg, int _fa, int _fs)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), deg(_deg), fa(_fa), fs(_fs) {}
	float x1, y1, x2, y2, rx, ry, deg, fa, fs;
};

void getPointsOnSvgEllipticalArc(vector<glm::vec2> &points, const SvgEllipticalArcDataIn &in);
void getPointsOnCircleArc(vector<glm::vec2> &points, const glm::vec2 &center, float radius, const glm::vec2 &start, const glm::vec2 &end, int direction);
void getPointsOnCircle(vector<glm::vec2> &points, const glm::vec2 &center, float radius, int numSegments);
void getPointsOnCircleArc(vector<glm::vec2> &points, const glm::vec2 &center, float radius, float startAngle, float arcAngle, int numSegments);
void getPointsOnEllipse(vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy, int numSegments);
void getPointsOnEllipseArc(vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy, float startAngle, float arcAngle, int numSegments);

// Get appropriate subdivision segment number
int getNumCircleSegments(float radius);

#endif /* RENDERABLES_FUNCTIONS_ARC_HPP_ */
