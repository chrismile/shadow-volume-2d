/*
 * LightManagerInterface.hpp
 *
 *  Created on: 26.10.2017
 *      Author: christoph
 */

#ifndef LOGIC_LIGHTMANAGERINTERFACE_HPP_
#define LOGIC_LIGHTMANAGERINTERFACE_HPP_

#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Scene/RenderTarget.hpp>
#include <Graphics/Scene/Camera.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include "VolumeLight.hpp"

using namespace std;
using namespace sgl;

class LightManagerInterface
{
public:
	LightManagerInterface() {}
	virtual ~LightManagerInterface() {}
	virtual void beginRenderScene()=0;
	virtual void endRenderScene()=0;
	virtual void beginRenderLightmap()=0;
	virtual void renderLightmap(function<void()> renderfun)=0;
	virtual void endRenderLightmap()=0;
	virtual void blitMixSceneAndLights()=0;
	virtual void renderGUI()=0;

	virtual VolumeLightPtr addLight(const glm::vec2 &pos, float rad = 10.0f, const Color &col = Color(255, 255, 255))=0;
	virtual vector<VolumeLightPtr> &getLights()=0;

	virtual void onResolutionChanged()=0;
	virtual ShaderProgramPtr getEdgeShader()=0;
};



#endif /* LOGIC_LIGHTMANAGERINTERFACE_HPP_ */
