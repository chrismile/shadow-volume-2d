/*
 * MainApp.hpp
 *
 *  Created on: 22.04.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_MainApp_HPP_
#define LOGIC_MainApp_HPP_

#include <Utils/AppLogic.hpp>
#include <Utils/Random/Xorshift.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include <Math/Geometry/Point2.hpp>
#include <Graphics/Mesh/Mesh.hpp>
#include <Graphics/Scene/Camera.hpp>
#include "Logic/Cube.hpp"
#include "Logic/Primitive.hpp"
#include <vector>
#include <glm/glm.hpp>

#include "Logic/LightManagerMap.hpp"
#include "Logic/LightManagerVolume.hpp"

using namespace std;
using namespace sgl;

class Shape;
typedef boost::shared_ptr<Shape> ShapePtr;

class VolumeLightApp : public AppLogic
{
public:
	VolumeLightApp();
	~VolumeLightApp();
	void render();
	void renderScene(); // Renders lighted scene
	void renderEdges(); // Renders edge lines of scene that get extruded by the geometry of "edgeShader"
	void update(float dt);
	void resolutionChanged(EventPtr event);

private:
	// Lighting & rendering
	boost::shared_ptr<Camera> camera;
	boost::shared_ptr<LightManagerInterface> lightManager;
	int lightManagerType;
	vector<PrimitivePtr> primitives;
	ShaderProgramPtr plainShader;
	ShaderProgramPtr edgeShader;
	ShaderProgramPtr whiteSolidShader;

	// User interaction
	ShaderAttributesPtr grabPointRenderData;
	float grabPointRadius;
	VolumeLightPtr grabbedLight;
	XorshiftRandomGenerator random;

	// Benchmarking performance
	bool benchmark;
	float benchmarkTimer;
	bool benchmarkFinished;
	int maxLights;
	int numProbes;
	vector<vector<float>> benchmarkData;
	vector<int> fps;
};

#endif /* LOGIC_MainApp_HPP_ */
