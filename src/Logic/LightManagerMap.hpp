/*
 * LightManager2.hpp
 *
 *  Created on: 23.04.2017
 *      Author: christoph
 */

#ifndef LOGIC_VOLUMELIGHT_LIGHTMANAGER2_HPP_
#define LOGIC_VOLUMELIGHT_LIGHTMANAGER2_HPP_

#include "LightManagerInterface.hpp"

using namespace std;
using namespace sgl;

class LightManagerMap : public LightManagerInterface
{
public:
	LightManagerMap(CameraPtr _camera);
	void beginRenderScene();
	void endRenderScene();
	void beginRenderLightmap();
	void renderLightmap(function<void()> renderfun);
	void endRenderLightmap();
	void blitMixSceneAndLights();

	VolumeLightPtr addLight(const glm::vec2 &pos, float rad = 10.0f, const Color &col = Color(255, 255, 255));
	vector<VolumeLightPtr> &getLights() { return lights; }

	void onResolutionChanged();
	ShaderProgramPtr getEdgeShader() { return shadowmapShader; }

private:
	CameraPtr camera;
	vector<VolumeLightPtr> lights;
	ShaderProgramPtr plainShader;
	ShaderProgramPtr shadowmapShader;
	ShaderProgramPtr shadowMapRenderShader;
	ShaderProgramPtr lightCombineShader;
	bool multisampling;

	ShaderAttributesPtr shadowmapRenderAttributes;
	glm::mat4 lightcamProj[3];
	glm::mat4 lightcamView[3];

	int shadowMapWidth;

	RenderTargetPtr sceneTarget;
	FramebufferObjectPtr sceneFBO;
	TexturePtr sceneTex;
	TexturePtr sceneRenderTex; // Equal to sceneTex if no MSAA is used
	RenderTargetPtr lightTarget;
	FramebufferObjectPtr lightFBO;
	TexturePtr lightTex;
	RenderTargetPtr shadowmapTarget;
	FramebufferObjectPtr shadowmapFBO;
	TexturePtr shadowmap;
};



#endif /* LOGIC_VOLUMELIGHT_LIGHTMANAGER2_HPP_ */
