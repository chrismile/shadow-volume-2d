/*
 * Circle.cpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#include "Circle.hpp"
#include "Arc.hpp"
#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>

CirclePrimitive::CirclePrimitive(ShaderProgramPtr _plainShader, ShaderProgramPtr _edgeShader, const glm::mat4 &_specialTransform)
{
    specialTransform = _specialTransform;
    const float rad = 0.2f;
    const int numSegments = 64;

    getPointsOnCircle(edges, glm::vec2(0.0f, 0.0f), rad, numSegments);
    vertices = edges;

    plainShader = _plainShader;
    edgeShader = _edgeShader;

    circleData = ShaderManager->createShaderAttributes(plainShader);
    GeometryBufferPtr geometryBuffer = Renderer->createGeometryBuffer(sizeof(glm::vec2)*vertices.size(), &vertices.front());
    circleData->addGeometryBuffer(geometryBuffer, "position", ATTRIB_FLOAT, 2);
    circleData->setVertexMode(VERTEX_MODE_TRIANGLE_FAN);

    edgeData = ShaderManager->createShaderAttributes(edgeShader);
    geometryBuffer = Renderer->createGeometryBuffer(sizeof(glm::vec2)*edges.size(), &edges.front());
    edgeData->addGeometryBuffer(geometryBuffer, "position", ATTRIB_FLOAT, 2);
    edgeData->setVertexMode(VERTEX_MODE_LINE_LOOP);
}

void CirclePrimitive::setEdgeShader(ShaderProgramPtr _edgeShader)
{
    edgeData = edgeData->copy(_edgeShader);
}

void CirclePrimitive::render()
{
    Renderer->setModelMatrix(matrixTranslation(position)*specialTransform);
    plainShader->setUniform("color", Color(60, 60, 60));
    Renderer->render(circleData);
}

void CirclePrimitive::renderEdges()
{
    Renderer->setModelMatrix(matrixTranslation(position)*specialTransform);
    Renderer->render(edgeData);

}
