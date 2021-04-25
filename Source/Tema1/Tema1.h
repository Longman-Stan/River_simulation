#pragma once
#pragma once
#pragma comment(lib, "Winmm.lib")

#include <Windows.h>
#include <Component/SimpleScene.h>
#include <vector>
#include "SkySphere.h"
#include "Map.h"
#include "Boat.h"
#include <glm/gtx/norm.hpp>

#define g 9.81

struct Particle
{
	glm::vec4 position;
	glm::vec4 speed;
	glm::vec4 initialPos;
	glm::vec4 initialSpeed;

	Particle() {};

	Particle(const glm::vec4& pos, const glm::vec4& speed)
	{
		SetInitial(pos, speed);
	}

	void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
	{
		position = pos;
		initialPos = pos;

		this->speed = speed;
		initialSpeed = speed;
	}
};


class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void DrawSkySphere();
	void DrawRiver();
	void DrawBoat(float deltaTime);
	void DrawMountains(Shader* shader, bool lat);
	void DrawRiverSegment(int start_of_segment_idx, float width, float rotation, Shader* shader);
	void DrawMountainSegment(int idx1, int idx2, int s_idx, int num_segment, Shader* shader, bool main_river = false);
	void RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color = glm::vec3(1));
	void get_river_control_points();
	void add_brat(int idx, glm::vec3 direction, glm::vec3 split_point, glm::vec2 border_angle);

	unsigned int UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz);

	int cubeMapTextureID;

	void Tema1::DrawCube();

	float sphere_radius;
	float global_time;

	std::unordered_map<std::string, Texture2D*> mapTextures;
	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

	SkySphere sky;

	Map map;

	//River
	int start_river_level, end_river_level;
	int z_increment;
	std::vector<glm::vec3> control_p;
	unsigned int no_of_generated_points;
	float max_translate, max_rotate;
	unsigned int show_support_points;
	int idx_segment;
	int river_wigglyness;
	int split_point_idx;
	float width_river;
	int nr_brate;
	int num_points_brate;
	float brat_width[10];
	int brat_start_idx[10];
	float brat_angle[10];

	//Mesh lat1, lat2, interb1, interb2;
	Boat boat;

	float start_time;
	glm::vec3 crt_position;
	glm::vec3 next_position;
	int river_index;
	int brat_index;

	glm::vec3 getBoatPosition(float deltaTime);
	glm::mat4 getBoatRotation();
	void getNextPoint();
	void chooseNewBrat(bool force_same = false);
	
	void populateRiverPoints();
	int river_resolution;
	std::vector<glm::vec3> river_points[4];
	int start;
	float boat_speed;
	int returning;
	int transition;
	glm::vec3 old_dir;

	ParticleEffect<Particle>* particleEffect;
	Particle* particle_data;
	SSBO<Particle>* particleSSBO;
	unsigned int num_particles;
	void DrawParticles(float dt);

	FrameBuffer* frameBuffer;
	glm::ivec2 resolution;

	ProjectionInfo projInfo;
	float fov_start;
	float fov_end;

	int first_person;

	ParticleEffect<Particle>* snowEffect;
	Particle* snow_data;
	SSBO<Particle>* snowSSBO;
	unsigned int num_snow_particles;
	void DrawSnow(float dt);
};
