#include "Tema1.h"

#include <vector>
#include <iostream>
#include <stb/stb_image.h>
#include <Core/Engine.h>
#include <math.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Tema1::Tema1()
{
	global_time = 0;
	sphere_radius = 100;
	sky = SkySphere("sky", 20, 60);

	lightPosition = glm::vec3(0, 20, 0);
	lightDirection = glm::vec3(0, 0, 0);
	materialShininess = 30;
	materialKd = 0.6f;
	materialKs = 0.6f;

	start_river_level = -40;
	end_river_level = 0;

	no_of_generated_points = 20;	//number of points on a Bezier curve
	max_translate = 8.0f;			//for the translation surface, it's the distance between the first and the last curve
	max_rotate = glm::radians(360.0f);	//for the rotation surface, it's the angle between the first and the last curve
	show_support_points = 0;
	z_increment = 4;
	river_wigglyness = 8;
	idx_segment = 0;

	width_river = 1.4;
	brat_width[0] = 0.45;
	brat_width[1] = 0.5;
	brat_width[2] = 0.45;

	nr_brate = 3;
	brat_angle[0] = 45;
	brat_angle[1] = 90;
	brat_angle[2] = 135;
	num_points_brate = 8;

	map = Map(40, 80, -20, -40);
	boat = Boat("BoatyMcBoatFace");

	river_resolution = 100;
	boat_speed = 1.f;
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	resolution = window->GetResolution();

	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 1, -40), glm::quat(glm::vec3(-30 * TO_RADIANS, 180 * TO_RADIANS, 0)));
	camera->Update();

	projInfo = camera->GetProjectionInfo();
	fov_start = projInfo.zNear + (projInfo.zFar - projInfo.zNear) * 0.0005;
	fov_end = projInfo.zFar - (projInfo.zFar - projInfo.zNear) * 0.85;
	std::cout << fov_start << ' ' << fov_end << '\n';

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	std::string shaderPath = "Source/Tema1/Shaders/";
	{
		Shader* shader = new Shader("SphereMap");
		shader->AddShader(shaderPath + "Spheremap_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Spheremap_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for rendering to texture

	{
		Shader* shader = new Shader("Reflect");
		shader->AddShader(shaderPath + "Reflect_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Reflect_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("River");
		shader->AddShader(shaderPath + "River_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "River_GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "River_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Mountain");
		shader->AddShader(shaderPath + "Mountain_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Mountain_GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "Mountain_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Mountain_lat");
		shader->AddShader(shaderPath + "Mountain_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Mountain_lat_GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "Mountain_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Boat");
		shader->AddShader(shaderPath + "Boat_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Boat_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Particle");
		shader->AddShader(shaderPath + "Particle_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Particle_GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "Particle_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Snow");
		shader->AddShader(shaderPath + "Snow_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Snow_GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "Snow_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	std::string path = "Source/Tema1/sky dome/";

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "sky_dome1.jpg").c_str(), GL_REPEAT);
		mapTextures["sky1"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "sky_dome2.jpg").c_str(), GL_CLAMP_TO_EDGE);
		mapTextures["sky2"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "sky_dome3.jpg").c_str(), GL_REPEAT);
		mapTextures["sky3"] = texture;
	}

	path = "Source/Tema1/Textures/";

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "box.jpg").c_str(), GL_REPEAT);
		mapTextures["box_texture"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "Pool_N.jpg").c_str(), GL_REPEAT);
		mapTextures["pool_N"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "Pool_S.jpg").c_str(), GL_REPEAT);
		mapTextures["pool"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "water-normal.jpg").c_str(), GL_REPEAT);
		mapTextures["water_normal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "SeaWavesB_S.jpg").c_str(), GL_REPEAT);
		mapTextures["seaB"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "mountain.jpg").c_str(), GL_REPEAT);
		mapTextures["mountain"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "water_particle.png").c_str(), GL_REPEAT);
		mapTextures["water_particle"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((path + "snow_particle.png").c_str(), GL_REPEAT);
		mapTextures["snow_particle"] = texture;
	}

	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-4.0, -2.5,  1.0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(-4.0, 5.5,  1.0), glm::vec3(0, 1, 0))
		};

		vector<unsigned short> indices =
		{
			0, 1
		};

		meshes["surface"] = new Mesh("generated initial surface points");
		meshes["surface"]->InitFromData(vertices, indices);
		meshes["surface"]->SetDrawMode(GL_LINES);
	}
	{
	get_river_control_points();
	populateRiverPoints();
	crt_position = river_points[0][0];
	next_position = river_points[0][1];
	river_index = 0;
	brat_index = 0;
	returning = 1;
	transition = 0;
	}

	//particles
	{
		num_particles = 50000;
		particleEffect = new ParticleEffect<Particle>();
		particleEffect->Generate(num_particles, true);
		particleSSBO = particleEffect->GetParticleBuffer();
		particle_data = const_cast<Particle*>(particleSSBO->GetBuffer());
	}

	glm::vec3 dir = glm::normalize(next_position - crt_position);
	glm::vec4 np = glm::vec4(crt_position - dir * 0.4f, 1);
	for (unsigned int i = 0; i < num_particles; i++)
	{
		glm::vec4 speed(0);
		speed.x = (float)(rand() % 20000) / 5000.f - 2.f;
		speed.z = (float)(rand() % 10000) / 5000.f;
		speed.y = (float)(rand() % 2000) / 1000.f;

		particle_data[i].SetInitial(np, speed);
	}
	particleSSBO->SetBufferData(particle_data);

	{
		Shader* shader = new Shader("Depth");
		shader->AddShader(shaderPath + "Depth_VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Depth_FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;

		Shader* shader2 = new Shader("Depth_1");
		shader2->AddShader(shaderPath + "Depth_VS.glsl", GL_VERTEX_SHADER);
		shader2->AddShader(shaderPath + "Depth2_FS.glsl", GL_FRAGMENT_SHADER);
		shader2->CreateAndLink();
		shaders[shader2->GetName()] = shader2;

	}

	frameBuffer = new FrameBuffer();
	frameBuffer->Clean();
	frameBuffer->Clear();
	frameBuffer->Generate(resolution.x, resolution.y, 1, true);

	//snow

	{
		{
			num_snow_particles = 100000;
			snowEffect = new ParticleEffect<Particle>();
			snowEffect->Generate(num_snow_particles, true);
			snowSSBO = snowEffect->GetParticleBuffer();
			snow_data = const_cast<Particle*>(snowSSBO->GetBuffer());
		}

		

		for (unsigned int i = 0; i < num_snow_particles; i++)
		{
			glm::vec4 position(0);
			glm::vec4 speed(0);

			position.x = (float)(rand() % 6000) / 100.f - 30.f;
			position.y = (float)(rand() % 5000) / 1000.f;
			position.z = (float)(rand() % 14000) / 200.f - 40;

			speed.x = (float)(rand() % 10000) / 1000.f ;
			speed.z = (float)(rand() % 1000) / 1000.f;
			speed.y = 0;
			snow_data[i].SetInitial(position, speed);
		}
		snowSSBO->SetBufferData(snow_data);
	}

	//enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


double get_rand_0_1()
{
	return ((double)rand()) / (double)(RAND_MAX);
}

void check_border(glm::vec2 border_angle, glm::vec3 &poz, float border_x_left, float border_x_right, float init_z)
{
	float coordinate;
	if (border_angle.x <= 90 && border_angle.x >= -90)
	{
		coordinate = (poz.z - init_z) * tan(border_angle.x * TO_RADIANS) + border_x_left;
		if (poz.x < coordinate)
		{
			poz.x = coordinate;
		}
	}
	if (border_angle.y <= 90 && border_angle.y >= -90)
	{
		coordinate = (poz.z - init_z) * tan(border_angle.y * TO_RADIANS) + border_x_right;
		if (poz.x > coordinate)
		{
			poz.x = coordinate;
		}
	}
}

void Tema1::add_brat(int idx, glm::vec3 direction, glm::vec3 split_point, glm::vec2 border_angle)
{
	int j = 1;
	control_p.push_back(split_point);
	brat_start_idx[idx] = control_p.size() - 1;
	glm::vec3 crt = split_point;
	float border_left_x = split_point.x + brat_width[idx];
	float border_right_x = split_point.x - brat_width[idx];
	int nr = 0;
	for(int i = 0, j = 1; i < num_points_brate; i++, j++)
	{
		crt = crt + direction * float(z_increment);
		
		if (j % 2)
			crt += glm::vec3(get_rand_0_1() * river_wigglyness, 0, 0);
		else
			crt -= glm::vec3(get_rand_0_1() * river_wigglyness, 0, 0);
		
		check_border(border_angle, crt, border_left_x, border_right_x, split_point.z);

		if (j == 3)
		{
			control_p.push_back((crt + control_p.back()) / 2.f);
			j = 1;
		}
		control_p.push_back(crt);
	}	
}

void Tema1::get_river_control_points()
{
	int i, j, nr;
	glm::vec3 p;
	for (j = 0,i = start_river_level; i <= end_river_level; i+=z_increment, j++)
	{
		if(j%2)
			p = glm::vec3(get_rand_0_1()* river_wigglyness, 0, i);
		else
			p = glm::vec3(-get_rand_0_1() * river_wigglyness, 0, i);

		if (j == 3)
		{
			control_p.push_back((p + control_p.back()) / 2.f);
			j = 1;
		}
		control_p.push_back(p);
	}

	split_point_idx = control_p.size() - 1;
	while (split_point_idx % 3)
	{
		control_p.pop_back();
		split_point_idx = control_p.size() - 1;
	}

	glm::vec3 split_point = control_p.back();
	glm::vec2 border_angle;

	glm::vec3 direction = glm::normalize(glm::vec3(1, 0, 1));
	brat_angle[0] = glm::radians(45.f);
	border_angle = glm::vec2(30, 100);
	add_brat(0, direction, split_point + glm::vec3(width_river - brat_width[0],0,0), border_angle);

	direction = glm::normalize(glm::vec3(0, 0, 1));
	brat_angle[1] = glm::radians(90.f);
	border_angle = glm::vec2(-30, 30);
	add_brat(1, direction, split_point + glm::vec3(width_river - 2*brat_width[0] - brat_width[1], 0, 0), border_angle);

	direction = glm::normalize(glm::vec3(-1, 0, 1));
	brat_angle[2] = glm::radians(135.f);
	border_angle = glm::vec2(100, -30);
	add_brat(2, direction, split_point + glm::vec3(-width_river + brat_width[2], 0, 0), border_angle);
}

glm::vec3 bezier(glm::vec3 p[4], float t)
{
	return p[0] * pow((1 - t), 3) + p[1] * 3.f * t * pow((1 - t), 2) +
				p[2] * 3.f * pow(t, 2) * (1 - t) + p[3] * pow(t, 3);
}

void Tema1::populateRiverPoints()
{
	int i, j;
	double increment;
	increment = 1.f / river_resolution;

	glm::vec3 p;
	for (i = 0; i + 3 <= split_point_idx; i += 3)
	{
		for (j = 0; j <= river_resolution; j++)
		{
			p = bezier(&control_p[i], j * increment);
			river_points[0].push_back(p);
		}
		river_points[0].pop_back();
	}
	river_points[0].push_back(p);

	for (int idx = 0; idx < nr_brate - 1; idx++)
	{
		for (i = brat_start_idx[idx]; i + 3 < brat_start_idx[idx + 1]; i += 3)
		{
			for (j = 0; j <= river_resolution; j++)
			{
				p = bezier(&control_p[i], j * increment);
				river_points[idx + 1].push_back(p);
			}
			river_points[idx+1].pop_back();
		}
	river_points[idx+1].push_back(p);
	}

	for (int i = brat_start_idx[nr_brate - 1]; i + 3 < control_p.size(); i += 3)
	{
		for (j = 0; j <= river_resolution; j++)
		{
			p = bezier(&control_p[i], j * increment);
			river_points[nr_brate].push_back(p);
		}
		river_points[nr_brate].pop_back();
	}
	river_points[nr_brate].push_back(p);

}

void Tema1::FrameStart()
{
}

void Tema1::Update(float deltaTimeSeconds)
{
	auto camera = GetSceneCamera();
	if (first_person)
	{
		camera->SetPosition(crt_position + glm::vec3(0,0.5f,0));
	}

	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	global_time += deltaTimeSeconds;
	ClearScreen();
	DrawSkySphere();
	DrawRiver();
	DrawMountains(shaders["Mountain"], false);
	DrawMountains(shaders["Mountain_lat"], true);
	DrawBoat(deltaTimeSeconds);
	if (start)
		DrawParticles(deltaTimeSeconds);
	DrawSnow(deltaTimeSeconds);

	frameBuffer->BindDefault();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glm::vec3 camPosition = camera->transform->GetWorldPosition();
	glm::quat camRotation = camera->transform->GetWorldRotation();

	ClearScreen();
	{
		auto shader = shaders["Depth_1"];
		shader->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffer->GetTexture(0)->GetTextureID());
		glUniform1i(shader->GetUniformLocation("textColor"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,frameBuffer->GetDepthTexture()->GetTextureID());
		glUniform1i(shader->GetUniformLocation("textDepth"), 1);

		glUniform2i(shader->GetUniformLocation("screenSize"), resolution.x, resolution.y);
		glUniform1f(shader->GetUniformLocation("fov_start"), fov_start);
		glUniform1f(shader->GetUniformLocation("fov_end"), fov_end);
		glUniform1f(shader->GetUniformLocation("near"), projInfo.zNear);
		glUniform1f(shader->GetUniformLocation("far"), projInfo.zFar);

		float aspect = (float)resolution.x / (float)resolution.y;

		glm::mat4 proj_mat;
		glm::mat4 modelMatrix(1);
		if (resolution.x >= resolution.y)
		{
			modelMatrix = glm::scale(modelMatrix, glm::vec3(aspect, 1, 1));
			proj_mat = glm::ortho(-1.f * aspect, aspect, -1.f, 1.f, -1.f, 20.f);
		}
		else
		{
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1, aspect, 1));
			proj_mat = glm::ortho(-1.f, 1.f, aspect, 1.f * aspect, 1.f, 20.f);
		}
		glm::mat4 view_mat = glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0,1,0));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_mat)); //and pass them to the shader
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(proj_mat));
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		meshes["quad"]->Render();
	}


}


void Tema1::DrawParticles(float dt)
{
	//glDisable(GL_DEPTH_TEST);

	auto shader = shaders["Particle"];

	if (shader->GetProgramID())
	{
		shader->Use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mapTextures["water_particle"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("water_particle");
		glUniform1i(loc_texture, 1);
		particleEffect->Render(GetSceneCamera(), shader);

		int loc_uniform = shader->GetUniformLocation("position");
		glUniform3f(loc_uniform, crt_position.x, crt_position.y, crt_position.z);

		loc_uniform = shader->GetUniformLocation("target");
		glUniform3f(loc_uniform, next_position.x, next_position.y, next_position.z);

		loc_uniform = shader->GetUniformLocation("dt");
		glUniform1f(loc_uniform, dt);

	}
	//glEnable(GL_DEPTH_TEST);
}


void Tema1::DrawSnow(float dt)
{
	//glDisable(GL_DEPTH_TEST);

	auto shader = shaders["Snow"];

	if (shader->GetProgramID())
	{
		shader->Use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mapTextures["snow_particle"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("snow_particle");
		glUniform1i(loc_texture, 1);
		snowEffect->Render(GetSceneCamera(), shader);

		glUniform1f(shader->GetUniformLocation("global_time"), global_time);
		glUniform1f(shader->GetUniformLocation("dt"), dt);

	}
	//glEnable(GL_DEPTH_TEST);
}


void Tema1::DrawBoat(float deltaTime)
{
	Camera* camera = GetSceneCamera();

	Shader* shader = shaders["Boat"];
	shader->Use();

	{
		int light_position = glGetUniformLocation(shader->program, "light_position");
		glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

		int light_direction = glGetUniformLocation(shader->program, "light_direction");
		glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

		int loc_eyepos = glGetUniformLocation(shader->program, "eye_position");
		glUniform3fv(loc_eyepos, 1, glm::value_ptr(camera->transform->GetWorldPosition()));

		int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
		glUniform1i(material_shininess, materialShininess);

		int material_kd = glGetUniformLocation(shader->program, "material_kd");
		glUniform1f(material_kd, materialKd);

		int material_ks = glGetUniformLocation(shader->program, "material_ks");
		glUniform1f(material_ks, materialKs);
	}
	
	glm::vec3 position = getBoatPosition(deltaTime);
	glm::mat4 boat_rotation = getBoatRotation();
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2,0.2,0.2));
	modelMatrix =  modelMatrix * boat_rotation;

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	boat.get_mesh()->Render();
}

void Tema1::DrawMountains(Shader* shader, bool lat)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	shader->Use();

	{
		Camera* camera = GetSceneCamera();

		int light_position = glGetUniformLocation(shader->program, "light_position");
		glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

		int light_direction = glGetUniformLocation(shader->program, "light_direction");
		glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

		int loc_eyepos = glGetUniformLocation(shader->program, "eye_position");
		glUniform3fv(loc_eyepos, 1, glm::value_ptr(camera->transform->GetWorldPosition()));

		int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
		glUniform1i(material_shininess, materialShininess);

		int material_kd = glGetUniformLocation(shader->program, "material_kd");
		glUniform1f(material_kd, materialKd);

		int material_ks = glGetUniformLocation(shader->program, "material_ks");
		glUniform1f(material_ks, materialKs);

	}
	
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mapTextures["mountain"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("mountain_texture");
		glUniform1i(loc_texture, 1);
	}

	//trimitei la shadere numarul de puncte care aproximeaza o curba (no_of_generated_points)
	//si caracteristici pentru crearea suprafetelor de translatie/rotatie (max_translate, max_rotate)
	glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
	glUniform1i(glGetUniformLocation(shader->program, "show_support_points"), show_support_points);

	int i,segment;
	if (!lat)
	{
		for (i = 0; i < nr_brate - 1; i++)
			for (int j = brat_start_idx[i], segment = 0; j + 3 < brat_start_idx[i + 1]; j += 3, segment++)
				DrawMountainSegment(i, i + 1, j - brat_start_idx[i], segment, shader);
	}
	else
	{
		for (int j = 0; j + 3 <= split_point_idx; j += 3)
		{
			DrawMountainSegment(0, -2, j, 0, shader, true);
			DrawMountainSegment(0, -1, j, 0, shader, true);
		}

		for (int j = brat_start_idx[0], segment = 0; j + 3 < brat_start_idx[1]; j += 3, segment++)
			DrawMountainSegment(0, -2, j - brat_start_idx[0], segment, shader);

		for (int j = brat_start_idx[nr_brate - 1], segment = 0; j + 3 < control_p.size(); j += 3, segment++)
			DrawMountainSegment(nr_brate-1, -1, j - brat_start_idx[nr_brate - 1], segment, shader);

	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Tema1::DrawMountainSegment(int idx1, int idx2, int s_idx, int num_segment, Shader* shader, bool main_river)
{
	int start1 = brat_start_idx[idx1];
	int start2;
	if( idx2 >= 0)
		 start2 = brat_start_idx[idx2];

	if(!main_river)
		glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 4, glm::value_ptr(control_p[start1 + s_idx]));
	else 
		glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 4, glm::value_ptr(control_p[s_idx]));

	if(idx2>=0)
		glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 4, glm::value_ptr(control_p[start2 + s_idx]));
	
	if( !main_river)
		glUniform1f(glGetUniformLocation(shader->program, "width1"), brat_width[idx1]);
	else
		glUniform1f(glGetUniformLocation(shader->program, "width1"), width_river);

	if( idx2>=0)
		glUniform1f(glGetUniformLocation(shader->program, "width2"), brat_width[idx2]);
	else
	{
		if( idx2 == -2)
			glUniform1f(glGetUniformLocation(shader->program, "width2"), 1);
		else 
			glUniform1f(glGetUniformLocation(shader->program, "width2"), -1);
	}
	glUniform1i(glGetUniformLocation(shader->program, "num_segment"), num_segment);
	
	Mesh* mesh = meshes["surface"];

	for (int i = 0; i < no_of_generated_points; i += 5)
	{
		glUniform1i(glGetUniformLocation(shader->program, "start_point"), i);
		glUniform1i(glGetUniformLocation(shader->program, "end_point"), i + 5);
		RenderMeshInstanced(mesh, shader, glm::mat4(1), 3);
	}
}

void Tema1::DrawCube()
{
	auto camera = GetSceneCamera();

	Shader* shader = shaders["Reflect"];
	shader->Use();

	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1), glm::vec3(1.f)), glm::vec3(5, 0, 0));

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	auto cameraPosition = camera->transform->GetWorldPosition();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
	int loc_texture = shader->GetUniformLocation("texture_cubemap");
	glUniform1i(loc_texture, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mapTextures["sky2"]->GetTextureID());
	loc_texture = shader->GetUniformLocation("sky_texture");
	glUniform1i(loc_texture, 2);

	int loc_camera = shader->GetUniformLocation("camera_position");
	glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	meshes["cube"]->Render();
}

void Tema1::DrawSkySphere()
{
	//glLineWidth(1);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto camera = GetSceneCamera();

	Shader* shader = shaders["SphereMap"];
	shader->Use();

	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(sphere_radius));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mapTextures["sky2"]->GetTextureID());
	int loc_texture = shader->GetUniformLocation("sky_texture");
	glUniform1i(loc_texture, 2);

	sky.getMesh()->Render();

	//meshes["cube"]->Render();
}

void Tema1::DrawRiver()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Shader* shader = shaders["River"];
	shader->Use();

	{
		Camera* camera = GetSceneCamera();

		int light_position = glGetUniformLocation(shader->program, "light_position");
		glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

		int light_direction = glGetUniformLocation(shader->program, "light_direction");
		glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

		int loc_eyepos = glGetUniformLocation(shader->program, "eye_position");
		glUniform3fv(loc_eyepos, 1, glm::value_ptr(camera->transform->GetWorldPosition()));

		int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
		glUniform1i(material_shininess, materialShininess);

		int material_kd = glGetUniformLocation(shader->program, "material_kd");
		glUniform1f(material_kd, materialKd);

		int material_ks = glGetUniformLocation(shader->program, "material_ks");
		glUniform1f(material_ks, materialKs);

	}

	int loc_time = glGetUniformLocation(shader->program, "time");
	glUniform1f(loc_time, global_time);


	//uniform sampler2D sky_texture;
	//uniform sampler2D water_normals;
	//uniform sampler2D water_texture;

	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mapTextures["water_normal"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("water_normals");
		glUniform1i(loc_texture, 1);
	}

	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mapTextures["sky2"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("sky_texture");
		glUniform1i(loc_texture, 2);
	}

	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mapTextures["seaB"]->GetTextureID());
		int loc_texture = shader->GetUniformLocation("water_texture");
		glUniform1i(loc_texture, 3);
	}

	//trimitei la shadere numarul de puncte care aproximeaza o curba (no_of_generated_points)
	//si caracteristici pentru crearea suprafetelor de translatie/rotatie (max_translate, max_rotate)
	glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
	glUniform1i(glGetUniformLocation(shader->program, "show_support_points"), show_support_points);

	int i;
	for (i = 0; i + 3 <= split_point_idx; i += 3)
		DrawRiverSegment(i, width_river, 0, shader);
	for (int idx = 0; idx < nr_brate - 1; idx++)
		for (int i = brat_start_idx[idx]; i + 3 < brat_start_idx[idx + 1]; i += 3)
			DrawRiverSegment(i, brat_width[idx], 0, shader);
	for (int i = brat_start_idx[nr_brate - 1]; i + 3 < control_p.size(); i += 3)
		DrawRiverSegment(i, brat_width[nr_brate - 1], 0, shader);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Tema1::DrawRiverSegment(int start_of_segment_idx, float width, float rotation, Shader* shader)
{
	//send uniforms to shaders
	glUniform3fv(glGetUniformLocation(shader->program, "control_p"), 4, glm::value_ptr(control_p[start_of_segment_idx]));
	glUniform1f(glGetUniformLocation(shader->program, "width"), width);
	glUniform1f(glGetUniformLocation(shader->program, "rotation"), rotation);

	Mesh* mesh = meshes["surface"];
	//draw the object instanced

	for (int i = 0; i < no_of_generated_points; i += 5)
	{
		glUniform1i(glGetUniformLocation(shader->program, "start_point"), i);
		glUniform1i(glGetUniformLocation(shader->program, "end_point"), i+5);
		RenderMeshInstanced(mesh, shader, glm::mat4(1), 3);
	}
}

void Tema1::RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader 
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object instanced
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, instances);

}

void Tema1::FrameEnd()
{
//	DrawCoordinatSystem();
}

void Tema1::chooseNewBrat(bool force_same)
{
	int new_brat = rand() % 4;
	if (force_same)
		new_brat = river_index;

	if (new_brat == river_index)
	{
		if (river_index == 0)
		{
			returning = 0;
			brat_index = river_points[river_index].size()-1;
			crt_position = river_points[river_index][brat_index];
			next_position = river_points[river_index][brat_index - 1];
		}
		else
		{
			returning = 0;
			brat_index = 0;
			crt_position = river_points[river_index][brat_index];
			next_position = river_points[river_index][brat_index + 1];
		}
	}
	else
	{
		transition = 1;
		boat_speed = 10;

		if (river_index == 0)
			crt_position = river_points[river_index].back();
		else crt_position = river_points[river_index].front();
		
		if (new_brat == 0)
			next_position = river_points[new_brat].back();
		else next_position = river_points[new_brat].front();

		if (crt_position == next_position)
		{
			transition = 0;
			boat_speed = 1;
			if (new_brat == 0)
			{
				brat_index = river_points[0].size() - 1;
				next_position = river_points[0][brat_index - 1];
			}
			else
			{
				brat_index = 0;
				next_position = river_points[new_brat][brat_index + 1];
			}
		}
	}
	river_index = new_brat;
}

void Tema1::getNextPoint()
{
	if (transition)
	{
		transition = 0;
		boat_speed = 1;
		chooseNewBrat(true);
		return;
	}

	if (river_index == 0)
	{
		if (returning)
		{
			if (brat_index < river_points[river_index].size() - 2)
			{
				brat_index++;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index + 1];
			}
			else
				chooseNewBrat();
		}
		else
		{
			if (brat_index > 1)
			{
				brat_index--;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index - 1];
			}
			else
			{
				returning = 1;
				brat_index = 0;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index + 1];
			}
		}
	}
	else
	{
		if (returning)
		{
			if (brat_index > 1)
			{
				brat_index--;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index- 1];
			}
			else
				chooseNewBrat();
		}
		else
		{
			if (brat_index < river_points[river_index].size() - 2)
			{
				brat_index++;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index + 1];
			}
			else
			{
				returning = 1;
				brat_index = river_points[river_index].size()-1;
				crt_position = river_points[river_index][brat_index];
				next_position = river_points[river_index][brat_index - 1];
			}
		}
	}
}

glm::mat4 Tema1::getBoatRotation()
{
	glm::vec3 dir = glm::normalize(next_position - crt_position);
	return glm::mat4(glm::vec4(dir.z, 0, -dir.x,0), glm::vec4(0, 1, 0,0), glm::vec4(dir.x, 0, dir.z,0), glm::vec4(0,0,0,1));
}

glm::vec3 Tema1::getBoatPosition(float deltaTime)
{
	if (!start)
		return crt_position;

	glm::vec3 dir = glm::normalize(next_position - crt_position);
	if ( glm::distance2(next_position, crt_position) < 0.1)
	{
		getNextPoint();
		dir = glm::normalize(next_position - crt_position);
	}

	crt_position = crt_position + boat_speed * deltaTime * dir;
	return crt_position;
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_EQUAL)
		idx_segment++;
	if (key == GLFW_KEY_MINUS)
		idx_segment--;
	if (key == GLFW_KEY_SPACE)
		start = 1;
	if (key == GLFW_KEY_LEFT_SHIFT)
		boat_speed = 10;
	if (key == GLFW_KEY_F)
		first_person = 1 - first_person;

	if (key == GLFW_KEY_P)
		no_of_generated_points++;
	if (key == GLFW_KEY_O)
		no_of_generated_points--;
	if (key == GLFW_KEY_T)
		show_support_points = 1 - show_support_points;
};

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
	if (key == GLFW_KEY_LEFT_SHIFT)
		boat_speed = 1;

};

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Tema1::OnWindowResize(int width, int height)
{
	// treat window resize event
	frameBuffer->Generate(width, height, 2, true);
}


unsigned int Tema1::UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// TODO - create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);


	// TODO - bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO - load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}
