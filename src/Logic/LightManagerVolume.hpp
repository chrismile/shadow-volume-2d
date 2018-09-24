/*
 * LightManager2.hpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_
#define LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_

#include "LightManagerInterface.hpp"

using namespace std;
using namespace sgl;

class LightManagerVolume : public LightManagerInterface
{
public:
	LightManagerVolume(CameraPtr _camera);
	void beginRenderScene();
	void endRenderScene();
	void beginRenderLightmap();
	void renderLightmap(function<void()> renderfun);
	void endRenderLightmap();
	void blitMixSceneAndLights();
	void renderGUI();

	VolumeLightPtr addLight(const glm::vec2 &pos, float rad = 10.0f, const Color &col = Color(255, 255, 255));
	vector<VolumeLightPtr> &getLights() { return lights; }

	void onResolutionChanged();
	ShaderProgramPtr getEdgeShader() { return edgeShader; }

private:
	CameraPtr camera;
	vector<VolumeLightPtr> lights;
	ShaderProgramPtr plainShader;
	ShaderProgramPtr edgeShader;
	ShaderProgramPtr lightCombineShader;

	RenderTargetPtr sceneTarget;
	RenderTargetPtr lightTarget;
	RenderTargetPtr lightTempTarget;
	FramebufferObjectPtr sceneFBO;
	TexturePtr sceneRenderTex; // Equal to sceneTex if no MSAA is used
	TexturePtr sceneTex;
	FramebufferObjectPtr lightFBO;
	TexturePtr lightRenderTex; // Equal to lightTex if no MSAA is used
	TexturePtr lightTex;
	FramebufferObjectPtr lightTempFBO;
	TexturePtr lightTempTex;
};



#endif /* LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_ */
