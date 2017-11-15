/*
 * Cube.cpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#include "Cube.hpp"
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>

Cube::Cube(ShaderProgramPtr _plainShader, ShaderProgramPtr _edgeShader, const glm::vec2 &extent, const glm::mat4 &_specialTransform)
{
	specialTransform = _specialTransform;
	plainShader = _plainShader;
	edgeShader = _edgeShader;
	edges = { glm::vec2(-extent.x, -extent.y), glm::vec2(extent.x, -extent.y), glm::vec2(extent.x, extent.y), glm::vec2(-extent.x, extent.y) };
	vertices = { glm::vec2(extent.x, -extent.y), glm::vec2(extent.x, extent.y), glm::vec2(-extent.x, -extent.y), glm::vec2(-extent.x, extent.y) };

	cubeData = ShaderManager->createShaderAttributes(plainShader);
	GeometryBufferPtr geometryBuffer = Renderer->createGeometryBuffer(sizeof(glm::vec2)*vertices.size(), &vertices.front());
	cubeData->addGeometryBuffer(geometryBuffer, "position", ATTRIB_FLOAT, 2);
	cubeData->setVertexMode(VERTEX_MODE_TRIANGLE_STRIP);

	edgeData = ShaderManager->createShaderAttributes(edgeShader);
	geometryBuffer = Renderer->createGeometryBuffer(sizeof(glm::vec2)*edges.size(), &edges.front());
	edgeData->addGeometryBuffer(geometryBuffer, "position", ATTRIB_FLOAT, 2);
	edgeData->setVertexMode(VERTEX_MODE_LINE_LOOP);
}

void Cube::setEdgeShader(ShaderProgramPtr _edgeShader)
{
	edgeData = edgeData->copy(_edgeShader);
}

void Cube::render()
{
	Renderer->setModelMatrix(matrixTranslation(position)*specialTransform);
	plainShader->setUniform("color", Color(60, 60, 60));
	Renderer->render(cubeData);
}

void Cube::renderEdges()
{
	Renderer->setModelMatrix(matrixTranslation(position)*specialTransform);
	Renderer->render(edgeData);

}
