/*
 * Circle.hpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VOLUMELIGHT_CIRCLE_HPP_
#define LOGIC_VOLUMELIGHT_CIRCLE_HPP_

#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include "Primitive.hpp"
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace sgl;

// TODO: Instance IDs

class CirclePrimitive : public Primitive {
public:
    CirclePrimitive(ShaderProgramPtr _plainShader, ShaderProgramPtr _edgeShader, const glm::mat4 &_specialTransform = matrixIdentity());
    inline void setPosition(const glm::vec2 &pos) { position = pos; }
    void render();
    void renderEdges();
    void setEdgeShader(ShaderProgramPtr _edgeShader);

private:
    ShaderProgramPtr plainShader;
    ShaderProgramPtr edgeShader;
    ShaderAttributesPtr circleData;
    ShaderAttributesPtr edgeData;
    vector<glm::vec2> vertices;
    vector<glm::vec2> edges;
    glm::vec2 position;
    glm::mat4 specialTransform;
};


#endif /* LOGIC_VOLUMELIGHT_CIRCLE_HPP_ */
