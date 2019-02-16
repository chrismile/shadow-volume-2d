/*
 * Cube.hpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VOLUMELIGHT_CUBE_HPP_
#define LOGIC_VOLUMELIGHT_CUBE_HPP_

#include <vector>
#include <Math/Geometry/MatrixUtil.hpp>
#include <glm/glm.hpp>
#include "Primitive.hpp"

using namespace std;
using namespace sgl;

// TODO: Instance IDs

class Cube : public Primitive {
public:
    Cube(ShaderProgramPtr _plainShader, ShaderProgramPtr _edgeShader, const glm::vec2 &extent, const glm::mat4 &_specialTransform = matrixIdentity());
    inline void setPosition(const glm::vec2 &pos) { position = pos; }
    void render();
    void renderEdges();
    void setEdgeShader(ShaderProgramPtr _edgeShader);

private:
    ShaderProgramPtr plainShader;
    ShaderProgramPtr edgeShader;
    ShaderAttributesPtr cubeData;
    ShaderAttributesPtr edgeData;
    vector<glm::vec2> vertices;
    vector<glm::vec2> edges;
    glm::vec2 position;
    glm::mat4 specialTransform;
};


#endif /* LOGIC_VOLUMELIGHT_CUBE_HPP_ */
