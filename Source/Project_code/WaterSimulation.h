#pragma once
#include "WaterBody.h"

#include <Component/SimpleScene.h>
#include <vector>

#define g 9.81

class WaterSimulation : public SimpleScene
{
public:
	WaterSimulation();
	~WaterSimulation();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void DrawScene();

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	unsigned int UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz);
	void DrawCubemap();
	void DrawCube();

	//define cubemap sizes
	float cubemap_side_length;

	WaterBody waterBody;
	float global_time;

	int cubeMapTextureID;

	//declare the variables for the collision waves
	int num_circular_waves;
	std::vector<float> initial_times;
	std::vector<glm::vec3> collision_center;

	//declare the variables for the gernster waves
	int num_gerstner_base_waves;
	std::vector<float> Q;
	std::vector<float> L;
	std::vector<float> A;
	std::vector<glm::vec3> D;
	std::vector<float> S;
	std::vector<float> steepness;
	std::vector<float> w;
	std::vector<float> phi;

	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

	std::unordered_map<std::string, Texture2D*> mapTextures;

	//cubes 
	std::vector<glm::vec3> cube_positions;
	std::vector<glm::vec3> cube_velocity;
	int animate_cube;
	int colission_detected;
};
