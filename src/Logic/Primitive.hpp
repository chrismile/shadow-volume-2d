/*
 * Primitive.hpp
 *
 *  Created on: 14.05.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VOLUMELIGHT_PRIMITIVE_HPP_
#define LOGIC_VOLUMELIGHT_PRIMITIVE_HPP_

#include <boost/shared_ptr.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>

class Primitive;
typedef boost::shared_ptr<Primitive> PrimitivePtr;

class Primitive {
public:
    virtual ~Primitive() {}
    virtual void render()=0;
    virtual void renderEdges()=0;
    virtual void setEdgeShader(sgl::ShaderProgramPtr _edgeShader)=0;
};

#endif /* LOGIC_VOLUMELIGHT_PRIMITIVE_HPP_ */
