#include "stdafx.h"

#include "Factories.h"
//forwards : 
#include "Utils.h"
#include "imgui_extension.h"


//addings : 
//template<>
//void ResourceFactory<Material>::add(const FileHandler::CompletePath& path, unsigned int hashKey)
//{
//	Material* newResource = getProgramFactory()(path);
//	newResource->init(path);
//
//	m_resources[path] = newResource;
//	m_resourceMapping[path] = hashKey;
//	m_resourcesFromHashKey[hashKey] = newResource;
//}

// Creation : 
//template<>
//Material* ResourceFactory<Material>::createNewResource(const FileHandler::CompletePath& path, void* data)
//{
//	//TODO 01
//
//	std::string* materialModelName = static_cast<std::string>(data);
//
//	Material* newMaterial = MaterialModelsFactory::instance().getInstance(materialModelName);
//	
//	return newMaterial;
//}



//Shader Programes
/*
template<>
void ResourceFactory<ShaderProgram>::initDefaults()
{
	//////////////////// CPU PARTICLE shaders ////////////////////////
	addDefault("defaultParticlesCPU", new ShaderProgram(FileHandler::CompletePath("particleCPU.vert"), FileHandler::CompletePath("particleCPU.frag")));

	//////////////////// PARTICLE shaders ////////////////////////
	addDefault("defaultParticles", new ShaderProgram(FileHandler::CompletePath("particle.vert"), FileHandler::CompletePath("particle.frag"), FileHandler::CompletePath("particle.geom")));

	//////////////////// PARTICLE SIMULATION shaders ////////////////////////
	addDefault("particleSimulation", new ShaderProgram(FileHandler::CompletePath("particleSimulation.vert"), FileHandler::CompletePath(), FileHandler::CompletePath("particleSimulation.geom")));

	//////////////////// BILLBOARD shaders ////////////////////////
	addDefault("defaultBillboard", new ShaderProgram(FileHandler::CompletePath("billboard.vert"), FileHandler::CompletePath("billboard.frag")));

	//////////////////// SKYBOX shaders ////////////////////////
	addDefault("defaultSkybox", new ShaderProgram(FileHandler::CompletePath("skybox.vert"), FileHandler::CompletePath("skybox.frag")));

	//////////////////// 3D Gpass shaders ////////////////////////
	addDefault("defaultLit", new ShaderProgram(FileHandler::CompletePath("aogl.vert"), FileHandler::CompletePath("aogl_gPass.frag")));

	//////////////////// WIREFRAME shaders ////////////////////////
	addDefault("defaultUnlit", new ShaderProgram(FileHandler::CompletePath("wireframe.vert"), FileHandler::CompletePath("wireframe.frag")));

	//////////////////// TERRAIN shaders ////////////////////////
	addDefault("defaultTerrain", new ShaderProgram(FileHandler::CompletePath("terrain.vert"), FileHandler::CompletePath("terrain.frag")));

	//////////////////// TERRAIN EDITION shaders ////////////////////////
	addDefault("defaultTerrainEdition", new ShaderProgram(FileHandler::CompletePath("terrainEdition.vert"), FileHandler::CompletePath("terrainEdition.frag")));

	//////////////////// DRAW ON TEXTURE shaders ////////////////////////
	addDefault("defaultDrawOnTexture", new ShaderProgram(FileHandler::CompletePath("drawOnTexture.vert"), FileHandler::CompletePath("drawOnTexture.frag")));

	//////////////////// GRASS FIELD shaders ////////////////////////
	addDefault("defaultGrassField", new ShaderProgram(FileHandler::CompletePath("grassField.vert"), FileHandler::CompletePath("grassField.frag")));

	//////////////////// GRASS FIELD shaders ////////////////////////
	addDefault("wireframeInstanced", new ShaderProgram(FileHandler::CompletePath("wireframeInstanced.vert"), FileHandler::CompletePath("wireframeInstanced.frag")));

	//////////////////// DEBUG DRAWER shaders ////////////////////////
	addDefault("debugDrawer", new ShaderProgram(FileHandler::CompletePath("debugDrawer.vert"), FileHandler::CompletePath("debugDrawer.frag")));
}
*/

//template<>
//void ResourceFactory<Material>::add(const FileHandler::CompletePath& path)
//{
//	Material* newResource = makeNewMaterialInstance(path);
//	newResource->init(path);
//
//	m_resources[path] = newResource;
//	m_resourceMapping[path] = ++s_resourceCount;
//	m_resourcesFromHashKey[s_resourceCount] = newResource;
//}
//
//template<>
//void ResourceFactory<Material>::add(const FileHandler::CompletePath& path, unsigned int hashKey)
//{
//	Material* newResource = makeNewMaterialInstance(path);
//	newResource->init(path);
//
//	m_resources[path] = newResource;
//	m_resourceMapping[path] = hashKey;
//	m_resourcesFromHashKey[hashKey] = newResource;
//}


//Cube Texture
template<>
void ResourceFactory<CubeTexture>::initDefaults()
{
	auto newTex = new CubeTexture(255, 255, 255);
	newTex->initGL();
	newTex->name = "default";
	addDefault(newTex->name, newTex);
}

//Textures
template<>
void ResourceFactory<Texture>::initDefaults()
{
	//default diffuse
	auto newTex = new Texture(glm::vec3(255, 255, 255));
	newTex->initGL();
	//newTex->name = "default";
	addDefault("default", newTex);

	//default normal
	newTex = new Texture(0, 0, 125);
	newTex->initGL();
	//newTex->name = "defaultNormal";
	addDefault("defaultNormal", newTex);
}


//Materials
template<>
void ResourceFactory<Material>::initDefaults()
{
	/*
	//we construct by default one material instance by shader program
	for (auto& it = getProgramFactory().resourceBegin(); it != getProgramFactory().resourceEnd(); it++)
	{
		Material* newMat = it->second->makeNewMaterialInstance();
		addDefault(newMat->getCompletePath().getFilename(), newMat);
	}
	*/
	
	Material* newMat = new MaterialLit(*getProgramFactory().get("lit"));
	addDefault("defaultLit", newMat);

	newMat = new MaterialSkybox(*getProgramFactory().get("skybox"));
	addDefault("defaultSkybox", newMat);

	newMat = new MaterialUnlit(*getProgramFactory().get("unlit"));
	addDefault("wireframe", newMat);

	newMat = new MaterialInstancedUnlit(*getProgramFactory().get("wireframeInstanced"));
	addDefault("wireframeInstanced", newMat);

	newMat = new MaterialGrassField(*getProgramFactory().get("grassField"));
	addDefault("grassfield", newMat);

	newMat = new MaterialBillboard(*getProgramFactory().get("billboard"));
	addDefault("billboard", newMat);

	newMat = new MaterialParticles(*getProgramFactory().get("particles"));
	addDefault("particles", newMat);

	newMat = new MaterialParticlesCPU(*getProgramFactory().get("particlesCPU"));
	addDefault("particlesCPU", newMat);

	newMat = new MaterialParticleSimulation(*getProgramFactory().get("particleSimulation"));
	addDefault("particleSimulation", newMat);

	newMat = new MaterialDebugDrawer(*getProgramFactory().get("debugDrawer"));
	addDefault("debugDrawer", newMat);	
}

//Mesh
template<>
void ResourceFactory<Mesh>::initDefaults()
{
	Mesh* cube = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_TANGENTS));
	cube->vertices = { 0.5,0.5,-0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,
		-0.5,0.5,-0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  -0.5,-0.5,-0.5,
		-0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,
		-0.5,0.5,-0.5,  0.5,0.5,-0.5,  0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,
		0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5,
		-0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5 };

	cube->normals = { 1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,
		0,1,0,  0,1,0,  0,1,0,  0,1,0,
		0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0 };

	cube->tangents = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0 };

	cube->uvs = { 0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0 };

	cube->triangleIndex = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };

	cube->initGl();
	cube->computeBoundingBox();


	Mesh* cubeWireFrame = new Mesh(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
	cubeWireFrame->triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cubeWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cubeWireFrame->vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cubeWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cubeWireFrame->initGl();
	cubeWireFrame->computeBoundingBox();

	std::vector<float> tmpVertices;
	Mesh* capsuleWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		float y = 0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f);
		y = y > 0 ? y + 1.f : y - 1.f;
		tmpVertices.push_back(y); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		float y = 0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f);
		y = y > 0 ? y + 1.f : y - 1.f;
		tmpVertices.push_back(y); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(1.f); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(-1.f); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(-0.5f); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0.5f); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(-0.5); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0); //z
	}
	//CapsuleWireFrame->triangleIndex = { };
	//CapsuleWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	capsuleWireFrame->vertices = tmpVertices;
	//CapsuleWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	capsuleWireFrame->initGl();
	capsuleWireFrame->computeBoundingBox();

	tmpVertices.clear();

	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(0); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}

	Mesh* sphereWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
	sphereWireFrame->vertices = tmpVertices;
	sphereWireFrame->initGl();
	sphereWireFrame->computeBoundingBox();
	tmpVertices.clear();

	Mesh* plane = new Mesh();
	plane->triangleIndex = { 0, 1, 2, 2, 3, 0 };
	plane->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	plane->vertices = { -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 };
	plane->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane->initGl();
	plane->computeBoundingBox();

	Mesh* quad = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_VERTICES), 2);
	quad->triangleIndex = { 0, 1, 2, 0, 2, 3 };
	quad->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	quad->vertices = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	quad->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	quad->initGl();
	//quad->computeBoundingBox();

	//addDefault("default", cube);
	addDefault("cube", cube);
	addDefault("cubeWireframe", cubeWireFrame);
	addDefault("capsuleWireframe", capsuleWireFrame);
	addDefault("sphereWireframe", sphereWireFrame);
	addDefault("plane", plane);
	addDefault("quad", quad);
}



void initAllResourceFactories()
{
	//Shader Programes
	getResourceFactory<ShaderProgram>().initDefaults();

	//Cube Texture
	getResourceFactory<CubeTexture>().initDefaults();

	//Textures
	getResourceFactory<Texture>().initDefaults();

	//Materials
	getResourceFactory<Material>().initDefaults();

	//Mesh
	getResourceFactory<Mesh>().initDefaults();

	//Animations : nothing by default
}

void clearAllResourceFactories()
{
	getMeshFactory().clear();
	getMaterialFactory().clear();
	getTextureFactory().clear();
	getCubeTextureFactory().clear();
	getProgramFactory().clear();
}

//access helper
ResourceFactory<ShaderProgram>& getProgramFactory()
{
	return ResourceFactory<ShaderProgram>::instance();
}

ResourceFactory<Mesh>& getMeshFactory()
{
	return ResourceFactory<Mesh>::instance();
}

ResourceFactory<Texture>& getTextureFactory()
{
	return ResourceFactory<Texture>::instance();
}

ResourceFactory<CubeTexture>& getCubeTextureFactory()
{
	return ResourceFactory<CubeTexture>::instance();
}

ResourceFactory<Material>& getMaterialFactory()
{
	return ResourceFactory<Material>::instance();
}

ResourceFactory<SkeletalAnimation>& getSkeletalAnimationFactory()
{
	return ResourceFactory<SkeletalAnimation>::instance();
}

template<>
ResourceType getResourceType<Texture>()
{
	return ResourceType::TEXTURE;
}

template<>
ResourceType getResourceType<CubeTexture>()
{
	return ResourceType::CUBE_TEXTURE;
}

template<>
ResourceType getResourceType<Mesh>()
{
	return ResourceType::MESH;
}

template<>
ResourceType getResourceType<SkeletalAnimation>()
{
	return ResourceType::SKELETAL_ANIMATION;
}

template<>
ResourceType getResourceType<Material>()
{
	return ResourceType::MATERIAL;
}

template<>
ResourceType getResourceType<ShaderProgram>()
{
	return ResourceType::PROGRAME;
}

ResourceType getResourceTypeFromFileType(FileHandler::FileType fileType)
{
	switch (fileType)
	{
	case FileHandler::NONE:
		return ResourceType::NONE;
	case FileHandler::IMAGE:
		return ResourceType::TEXTURE;
	case FileHandler::CUBE_TEXTURE:
		return ResourceType::CUBE_TEXTURE;
	case FileHandler::MESH:
		return ResourceType::MESH;
	case FileHandler::SOUND:
		return ResourceType::NONE; //TODO sound
	case FileHandler::MATERIAL:
		return ResourceType::MATERIAL;
	default:
		break;
	}
}

void addResourceToFactory(const FileHandler::CompletePath& completePath)
{
	ResourceType resourceType = getResourceTypeFromFileType(completePath.getFileType());

	switch (resourceType)
	{
	case NONE:
		break;
	case PROGRAME:
		getResourceFactory<ShaderProgram>().add(completePath);
		break;
	case TEXTURE:
		getResourceFactory<Texture>().add(completePath);
		break;
	case CUBE_TEXTURE:
		getResourceFactory<CubeTexture>().add(completePath); //TODO
		break;
	case MESH:
		getResourceFactory<Mesh>().add(completePath);
		break;
	case SKELETAL_ANIMATION:
		//getResourceFactory<SkeletalAnimation>().add(completePath); //TODO
		break;
	case MATERIAL:
		getResourceFactory<Material>().add(completePath); //TODO
		break;
	default:
		break;
	}

}

void renameResourceInFactory(const FileHandler::CompletePath& oldResourcePath, const FileHandler::CompletePath& newResourcePath)
{
	assert(oldResourcePath.getFileType() != newResourcePath.getFileType());

	ResourceType resourceType = getResourceTypeFromFileType(oldResourcePath.getFileType());

	switch (resourceType)
	{
	case NONE:
		break;
	case PROGRAME:
		//getResourceFactory<ShaderProgram>().changeResourceKey(oldResourcePath, newResourcePath);
		std::cout << "warning : can't edit shaderProgram factory ! in : renameResourceInFactory()"<<std::endl;
		break;
	case TEXTURE:
		getResourceFactory<Texture>().changeResourceKey(oldResourcePath, newResourcePath);
		break;
	case CUBE_TEXTURE:
		//getResourceFactory<CubeTexture>().changeResourceKey(oldResourcePath, newResourcePath); //TODO
		break;
	case MESH:
		getResourceFactory<Mesh>().changeResourceKey(oldResourcePath, newResourcePath);
		break;
	case SKELETAL_ANIMATION:
		//getResourceFactory<SkeletalAnimation>().changeResourceKey(oldResourcePath, newResourcePath); //TODO
		break;
	case MATERIAL:
		//getResourceFactory<Material>().changeResourceKey(oldResourcePath, newResourcePath); //TODO
		break;
	default:
		break;
	}
}

void removeResourceFromFactory(const FileHandler::CompletePath& resourcePath)
{
	ResourceType resourceType = getResourceTypeFromFileType(resourcePath.getFileType());

	switch (resourceType)
	{
	case NONE:
		break;
	case PROGRAME:
		//getResourceFactory<ShaderProgram>().erase(resourcePath);
		std::cout << "warning : can't edit shaderProgram factory ! in : renameResourceInFactory()" << std::endl;
		break;
	case TEXTURE:
		getResourceFactory<Texture>().erase(resourcePath);
		break;
	case CUBE_TEXTURE:
		//getResourceFactory<CubeTexture>().erase(resourcePath); //TODO
		break;
	case MESH:
		getResourceFactory<Mesh>().erase(resourcePath);
		break;
	case SKELETAL_ANIMATION:
		//getResourceFactory<SkeletalAnimation>().erase(resourcePath); //TODO
		break;
	case MATERIAL:
		//getResourceFactory<Material>().erase(resourcePath); //TODO
		break;
	default:
		break;
	}
}

void removeAllResourcesFromFactories()
{
	getResourceFactory<ShaderProgram>().clear();
	getResourceFactory<Texture>().clear();
	getResourceFactory<CubeTexture>().clear();
	getResourceFactory<Mesh>().clear();
	getResourceFactory<SkeletalAnimation>().clear();
	getResourceFactory<Material>().clear();
}

template<>
inline const std::string& getResourceExtention<Material>()
{
	return ".mat";
}


//
//ProgramFactory::ProgramFactory()
//{
//
//	//////////////////// CPU PARTICLE shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_particleCPU = compile_shader_from_file(GL_VERTEX_SHADER, "particleCPU.vert");
//	GLuint fragShaderId_particleCPU = compile_shader_from_file(GL_FRAGMENT_SHADER, "particleCPU.frag");
//
//	GLuint programObject_particleCPU = glCreateProgram();
//	glAttachShader(programObject_particleCPU, vertShaderId_particleCPU);
//	glAttachShader(programObject_particleCPU, fragShaderId_particleCPU);
//
//	glLinkProgram(programObject_particleCPU);
//	if (check_link_error(programObject_particleCPU) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// PARTICLE shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_particle = compile_shader_from_file(GL_VERTEX_SHADER, "particle.vert");
//	GLuint geomShaderId_particle = compile_shader_from_file(GL_GEOMETRY_SHADER, "particle.geom");
//	GLuint fragShaderId_particle = compile_shader_from_file(GL_FRAGMENT_SHADER, "particle.frag");
//
//	GLuint programObject_particle = glCreateProgram();
//	glAttachShader(programObject_particle, vertShaderId_particle);
//	glAttachShader(programObject_particle, geomShaderId_particle);
//	glAttachShader(programObject_particle, fragShaderId_particle);
//
//	glLinkProgram(programObject_particle);
//	if (check_link_error(programObject_particle) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// PARTICLE SIMULATION shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_particleSimulation = compile_shader_from_file(GL_VERTEX_SHADER, "particleSimulation.vert");
//	GLuint geomShaderId_particleSimulation = compile_shader_from_file(GL_GEOMETRY_SHADER, "particleSimulation.geom");
//
//	GLuint programObject_particleSimulation = glCreateProgram();
//	glAttachShader(programObject_particleSimulation, vertShaderId_particleSimulation);
//	glAttachShader(programObject_particleSimulation, geomShaderId_particleSimulation);
//
//	const GLchar* feedbackVaryings[1] = { "outPositions" }; //, "outVelocities", "outForces", "outElapsedTimes", "outLifeTimes", "outColors", "outSizes"};
//	glTransformFeedbackVaryings(programObject_particleSimulation, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
//
//	glLinkProgram(programObject_particleSimulation);
//	if (check_link_error(programObject_particleSimulation) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// BILLBOARD shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_billboard = compile_shader_from_file(GL_VERTEX_SHADER, "billboard.vert");
//	GLuint fragShaderId_billboard = compile_shader_from_file(GL_FRAGMENT_SHADER, "billboard.frag");
//
//	GLuint programObject_billboard = glCreateProgram();
//	glAttachShader(programObject_billboard, vertShaderId_billboard);
//	glAttachShader(programObject_billboard, fragShaderId_billboard);
//
//	glLinkProgram(programObject_billboard);
//	if (check_link_error(programObject_billboard) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// SKYBOX shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_skybox = compile_shader_from_file(GL_VERTEX_SHADER, "skybox.vert");
//	GLuint fragShaderId_skybox = compile_shader_from_file(GL_FRAGMENT_SHADER, "skybox.frag");
//
//	GLuint programObject_skybox = glCreateProgram();
//	glAttachShader(programObject_skybox, vertShaderId_skybox);
//	glAttachShader(programObject_skybox, fragShaderId_skybox);
//
//	glLinkProgram(programObject_skybox);
//	if (check_link_error(programObject_skybox) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// 3D Gpass shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_gpass = compile_shader_from_file(GL_VERTEX_SHADER, "aogl.vert");
//	GLuint fragShaderId_gpass = compile_shader_from_file(GL_FRAGMENT_SHADER, "aogl_gPass.frag");
//
//	GLuint programObject_gPass = glCreateProgram();
//	glAttachShader(programObject_gPass, vertShaderId_gpass);
//	glAttachShader(programObject_gPass, fragShaderId_gpass);
//
//	glLinkProgram(programObject_gPass);
//	if (check_link_error(programObject_gPass) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// WIREFRAME shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_wireframe = compile_shader_from_file(GL_VERTEX_SHADER, "wireframe.vert");
//	GLuint fragShaderId_wireframe = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframe.frag");
//
//	GLuint programObject_wireframe = glCreateProgram();
//	glAttachShader(programObject_wireframe, vertShaderId_wireframe);
//	glAttachShader(programObject_wireframe, fragShaderId_wireframe);
//
//	glLinkProgram(programObject_wireframe);
//	if (check_link_error(programObject_wireframe) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// TERRAIN shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_terrain = compile_shader_from_file(GL_VERTEX_SHADER, "terrain.vert");
//	GLuint fragShaderId_terrain = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrain.frag");
//
//	GLuint programObject_terrain = glCreateProgram();
//	glAttachShader(programObject_terrain, vertShaderId_terrain);
//	glAttachShader(programObject_terrain, fragShaderId_terrain);
//
//	glLinkProgram(programObject_terrain);
//	if (check_link_error(programObject_terrain) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// TERRAIN EDITION shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_terrainEdition = compile_shader_from_file(GL_VERTEX_SHADER, "terrainEdition.vert");
//	GLuint fragShaderId_terrainEdition = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrainEdition.frag");
//
//	GLuint programObject_terrainEdition = glCreateProgram();
//	glAttachShader(programObject_terrainEdition, vertShaderId_terrainEdition);
//	glAttachShader(programObject_terrainEdition, fragShaderId_terrainEdition);
//
//	glLinkProgram(programObject_terrainEdition);
//	if (check_link_error(programObject_terrainEdition) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// DRAW ON TEXTURE shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_drawOnTexture = compile_shader_from_file(GL_VERTEX_SHADER, "drawOnTexture.vert");
//	GLuint fragShaderId_drawOnTexture = compile_shader_from_file(GL_FRAGMENT_SHADER, "drawOnTexture.frag");
//
//	GLuint programObject_drawOnTexture = glCreateProgram();
//	glAttachShader(programObject_drawOnTexture, vertShaderId_drawOnTexture);
//	glAttachShader(programObject_drawOnTexture, fragShaderId_drawOnTexture);
//
//	glLinkProgram(programObject_drawOnTexture);
//	if (check_link_error(programObject_drawOnTexture) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// GRASS FIELD shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_grassField = compile_shader_from_file(GL_VERTEX_SHADER, "grassField.vert");
//	GLuint fragShaderId_grassField = compile_shader_from_file(GL_FRAGMENT_SHADER, "grassField.frag");
//
//	GLuint programObject_grassField = glCreateProgram();
//	glAttachShader(programObject_grassField, vertShaderId_grassField);
//	glAttachShader(programObject_grassField, fragShaderId_grassField);
//
//	glLinkProgram(programObject_grassField);
//	if (check_link_error(programObject_grassField) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// GRASS FIELD shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_wireframeInstanced = compile_shader_from_file(GL_VERTEX_SHADER, "wireframeInstanced.vert");
//	GLuint fragShaderId_wireframeInstanced = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframeInstanced.frag");
//
//	GLuint programObject_wireframeInstanced = glCreateProgram();
//	glAttachShader(programObject_wireframeInstanced, vertShaderId_wireframeInstanced);
//	glAttachShader(programObject_wireframeInstanced, fragShaderId_wireframeInstanced);
//
//	glLinkProgram(programObject_wireframeInstanced);
//	if (check_link_error(programObject_wireframeInstanced) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	//////////////////// DEBUG DRAWER shaders ////////////////////////
//	// Try to load and compile shaders
//	GLuint vertShaderId_debugDrawer = compile_shader_from_file(GL_VERTEX_SHADER, "debugDrawer.vert");
//	GLuint fragShaderId_debugDrawer  = compile_shader_from_file(GL_FRAGMENT_SHADER, "debugDrawer.frag");
//
//	GLuint programObject_debugDrawer = glCreateProgram();
//	glAttachShader(programObject_debugDrawer, vertShaderId_debugDrawer);
//	glAttachShader(programObject_debugDrawer, fragShaderId_debugDrawer);
//
//	glLinkProgram(programObject_debugDrawer);
//	if (check_link_error(programObject_debugDrawer) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	m_defaults["defaultLit"] = programObject_gPass;
//	m_defaults["defaultUnlit"] = programObject_wireframe;
//	m_defaults["defaultSkybox"] = programObject_skybox;
//	m_defaults["defaultTerrain"] = programObject_terrain;
//	m_defaults["defaultTerrainEdition"] = programObject_terrainEdition;
//	m_defaults["defaultDrawOnTexture"] = programObject_drawOnTexture;
//	m_defaults["defaultGrassField"] = programObject_grassField;
//	m_defaults["wireframeInstanced"] = programObject_wireframeInstanced;
//	m_defaults["defaultBillboard"] = programObject_billboard;
//	m_defaults["defaultParticles"] = programObject_particle;
//	m_defaults["defaultParticlesCPU"] = programObject_particleCPU;
//	m_defaults["particleSimulation"] = programObject_particleSimulation;
//	m_defaults["debugDrawer"] = programObject_debugDrawer;
//}
//
//void ProgramFactory::add(const std::string& name, GLuint programId)
//{
//	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
//		return;
//
//	m_programs[name] = programId;
//}
//
//GLuint ProgramFactory::get(const std::string& name)
//{
//	assert(m_programs.find(name) != m_programs.end());
//
//	return m_programs[name];
//}
//
//bool ProgramFactory::contains(const std::string& name)
//{
//	return m_programs.find(name) != m_programs.end();
//}
//
//void ProgramFactory::drawUI()
//{
//	ImGui::PushID("programFactory");
//
//	for (auto& p : m_programs)
//	{
//		ImGui::Text(p.first.c_str());
//	}
//
//	ImGui::PopID();
//}
//
//void ProgramFactory::clear()
//{
//	for (auto& it = m_programs.begin(); it != m_programs.end();)
//	{
//		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
//		{
//			glDeleteProgram(it->second);
//			it = m_programs.erase(it);
//		}
//		else
//			it++;
//	}
//}
//
//void ProgramFactory::save(Json::Value & objectRoot) const
//{
//	//objectRoot["size"] = m_programs.size();
//	//int i = 0;
//	//for (auto it = m_programs.begin(); it != m_programs.end(); it++)
//	//{
//	//	objectRoot["keys"][i] = it->first;
//	//	objectRoot["values"][i] = it->second;
//	//	i++;
//	//}
//}
//
//void ProgramFactory::load(const Json::Value & entityRoot)
//{
//	//int size = entityRoot.get("size", 0).asInt();
//	//for (int i = 0; i < size; i++)
//	//{
//	//	std::string key = entityRoot["keys"][i].asString();
//	//	GLuint glId = entityRoot["values"][i].asInt();
//	//	m_programs[key] = glId;
//	//}
//}
//
/////////////////////////////////////////
//
//TextureFactory::TextureFactory()
//{
//	auto newTex = new Texture(255, 255, 255);
//	newTex->initGL();
//	
//	newTex->name = "default";
//	m_textures["default"] = newTex;
//
//	newTex = new Texture(0, 0, 125);
//	newTex->initGL();
//
//	newTex->name = "defaultNormal";
//	m_textures["defaultNormal"] = newTex;
//}
//
//void TextureFactory::add(const std::string& name, const std::string& path)
//{
//	if (name == "default" || name == "defaultNormal") //can't override default key
//		return;
//
//	auto newTexture = new Texture(path);
//
//	newTexture->name = name;
//	m_textures[name] = newTexture;
//}
//
//void TextureFactory::add(const std::string& name, Texture* texture)
//{
//	if (name == "default" || name == "defaultNormal") //can't override default key
//		return;
//
//	texture->name = name;
//	m_textures[name] = texture;
//}
//
//Texture* TextureFactory::get(const std::string& name)
//{
//	return m_textures[name];
//}
//
//bool TextureFactory::contains(const std::string& name)
//{
//	return (m_textures.find(name) != m_textures.end()); 
//}
//
//void TextureFactory::drawUI()
//{
//	ImGui::PushID("textureFactory");
//
//	ImGui::PushItemWidth(70);
//	ImGui::InputText("name", name, 20);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	ImGui::PushItemWidth(180);
//
//	ImGui::InputFilePath("path", path, 50);
//
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	if (ImGui::SmallButton("add"))
//	{
//		add(name, path);
//	}
//	
//
//	for (auto& t : m_textures)
//	{
//		ImGui::Text(t.first.c_str());
//		ImGui::SameLine();
//		ImGui::Text(t.second->path.c_str());
//	}
//
//	ImGui::PopID();
//}
//
//void TextureFactory::clear()
//{
//	for (auto& it = m_textures.begin(); it != m_textures.end();)
//	{
//		if (it->first != "default")
//		{
//			it->second->freeGL();
//			delete it->second;
//			it = m_textures.erase(it);
//		}
//		else
//			it++;
//	}
//}
//
//void TextureFactory::save(Json::Value & entityRoot) const
//{
//	entityRoot["size"] = m_textures.size();
//	int i = 0;
//	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
//	{
//		entityRoot["keys"][i] = it->first;
//		entityRoot["values"][i] = it->second->path;
//		i++;
//	}
//}
//
//void TextureFactory::load(const Json::Value & entityRoot)
//{
//	int size = entityRoot.get("size", 0).asInt();
//	for (int i = 0; i < size; i++)
//	{
//		std::string textureName = entityRoot["keys"][i].asString();
//		std::string texturePath = entityRoot["values"][i].asString();
//		add(textureName, texturePath);
//	}
//}
//
///////////////////////////////////////////
//
//
//CubeTextureFactory::CubeTextureFactory()
//{
//	auto newTex = new CubeTexture(255, 255, 255);
//	newTex->initGL();
//
//	newTex->name = "default";
//	m_textures["default"] = newTex;
//}
//
//void CubeTextureFactory::add(const std::string& name, const std::vector<std::string>& paths)
//{
//	if (name == "default") //can't override default key
//		return;
//
//	auto newTexture = new CubeTexture(paths);
//
//	newTexture->name = name;
//	m_textures[name] = newTexture;
//}
//
//void CubeTextureFactory::add(const std::string& name, CubeTexture* textureId)
//{
//	if (name == "default") //can't override default key
//		return;
//
//	textureId->name = name;
//	m_textures[name] = textureId;
//}
//
//CubeTexture* CubeTextureFactory::get(const std::string& name)
//{
//	return m_textures[name];
//}
//
//bool CubeTextureFactory::contains(const std::string& name)
//{
//	return (m_textures.find(name) != m_textures.end());
//}
//
//void CubeTextureFactory::drawUI()
//{
//	ImGui::PushID("textureFactory");
//
//	ImGui::PushItemWidth(70);
//	ImGui::InputText("name", name, 20);
//	ImGui::PopItemWidth();
//
//	ImGui::InputFilePath("path", paths[0], 50);
//	ImGui::InputFilePath("path", paths[2], 50);
//	ImGui::InputFilePath("path", paths[3], 50);
//	ImGui::InputFilePath("path", paths[4], 50);
//	ImGui::InputFilePath("path", paths[5], 50);
//
//	//ImGui::InputText("path right", paths[0], 50);
//	//ImGui::InputText("path left", paths[1], 50);
//	//ImGui::InputText("path top", paths[2], 50);
//	//ImGui::InputText("path bottom", paths[3], 50);
//	//ImGui::InputText("path back", paths[4], 50);
//	//ImGui::InputText("path front", paths[5], 50);
//
//	if (ImGui::SmallButton("add"))
//	{
//		std::vector<std::string> tmpPaths;
//		for (int i = 0; i < 6; i++)
//		{
//			tmpPaths.push_back(paths[i]);
//		}
//
//		add(name, tmpPaths);
//	}
//
//
//	for (auto& t : m_textures)
//	{
//		ImGui::Text(t.first.c_str());
//	}
//
//	ImGui::PopID();
//}
//
//void CubeTextureFactory::clear()
//{
//	for (auto& it = m_textures.begin(); it != m_textures.end();)
//	{
//		if (it->first != "default")
//		{
//			it->second->freeGL();
//			delete it->second;
//			it = m_textures.erase(it);
//		}
//		else
//			it++;
//	}
//}
//
//void CubeTextureFactory::save(Json::Value & entityRoot) const
//{
//	entityRoot["size"] = m_textures.size();
//	int i = 0;
//	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
//	{
//		entityRoot["keys"][i] = it->first;
//
//		entityRoot["values"][i]["size"] = it->second->paths.size();
//		for (int p = 0; p < it->second->paths.size(); p++)
//			entityRoot["values"][i]["paths"][p] = it->second->paths[p];
//		i++;
//	}
//}
//
//void CubeTextureFactory::load(const Json::Value & entityRoot)
//{
//	int size = entityRoot.get("size", 0).asInt();
//	for (int i = 0; i < size; i++)
//	{
//		std::string cubeTextureName = entityRoot["keys"][i].asString();
//
//		int pathSize = entityRoot["values"][i].get("size",0).asInt();
//		std::vector<std::string> cubeTexturePaths;
//		for (int p = 0; p < pathSize; p++)
//			cubeTexturePaths.push_back(entityRoot["values"][i]["paths"].get(p,"default").asString());
//
//		add(cubeTextureName, cubeTexturePaths);
//	}
//}
//
//
///////////////////////////////////////////
//
//MeshFactory::MeshFactory()
//{
//	Mesh* cube = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_TANGENTS));
//	cube->vertices = { 0.5,0.5,-0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,
//		-0.5,0.5,-0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  -0.5,-0.5,-0.5,
//		-0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,
//		-0.5,0.5,-0.5,  0.5,0.5,-0.5,  0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,
//		0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5,
//		-0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5 };
//
//	cube->normals = { 1,0,0,  1,0,0,  1,0,0,  1,0,0,
//		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
//		0,0,1,  0,0,1,  0,0,1,  0,0,1,
//		0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,
//		0,1,0,  0,1,0,  0,1,0,  0,1,0,
//		0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0 };
//
//	cube->tangents = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,
//		0,0,1,  0,0,1,  0,0,1,  0,0,1,
//		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
//		1,0,0,  1,0,0,  1,0,0,  1,0,0,
//		1,0,0,  1,0,0,  1,0,0,  1,0,0,
//		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0 };
//
//	cube->uvs = { 0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
//		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
//		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
//		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
//		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
//		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0 };
//
//	cube->triangleIndex = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };
//
//	cube->initGl();
//	cube->name = "cube";
//	cube->path = "";
//	cube->computeBoundingBox();
//
//
//	Mesh* cubeWireFrame = new Mesh(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
//	cubeWireFrame->triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
//	cubeWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
//	cubeWireFrame->vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
//	cubeWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
//	cubeWireFrame->initGl();
//	cubeWireFrame->name = "cubeWireframe";
//	cubeWireFrame->path = "";
//	cubeWireFrame->computeBoundingBox();
//
//	std::vector<float> tmpVertices;
//	Mesh* capsuleWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
//		float y = 0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f);
//		y = y > 0 ? y + 1.f : y - 1.f;
//		tmpVertices.push_back(y); //y
//		tmpVertices.push_back(0); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0); //x
//		float y = 0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f);
//		y = y > 0 ? y + 1.f : y - 1.f;
//		tmpVertices.push_back(y); //y
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
//		tmpVertices.push_back(1.f); //y
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
//		tmpVertices.push_back(-1.f); //y
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0); //x
//		tmpVertices.push_back(-1.f + (i/9.f)*2.f); //y
//		tmpVertices.push_back(-0.5f); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0); //x
//		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
//		tmpVertices.push_back(0.5f); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(-0.5); //x
//		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
//		tmpVertices.push_back(0); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5); //x
//		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
//		tmpVertices.push_back(0); //z
//	}
//	//CapsuleWireFrame->triangleIndex = { };
//	//CapsuleWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
//	capsuleWireFrame->vertices = tmpVertices;
//	//CapsuleWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
//	capsuleWireFrame->initGl();
//	capsuleWireFrame->name = "capsuleWireframe";
//	capsuleWireFrame->path = "";
//	capsuleWireFrame->computeBoundingBox();
//	
//	tmpVertices.clear();
//
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //y
//		tmpVertices.push_back(0); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
//		tmpVertices.push_back(0); //y
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
//	}
//	for (int i = 0; i < 10; i++) {
//		tmpVertices.push_back(0); //x
//		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //y
//		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
//	}
//
//	Mesh* sphereWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
//	sphereWireFrame->vertices = tmpVertices;
//	sphereWireFrame->initGl();
//	sphereWireFrame->name = "sphereWireframe";
//	sphereWireFrame->path = "";
//	sphereWireFrame->computeBoundingBox();
//	tmpVertices.clear();
//
//	Mesh* plane = new Mesh();
//	plane->triangleIndex = { 0, 1, 2, 2, 3, 0 };
//	plane->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
//	plane->vertices = { -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5};
//	plane->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
//	plane->initGl();
//	plane->name = "plane";
//	plane->path = "";
//	plane->computeBoundingBox();
//
//	Mesh* quad = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_VERTICES), 2);
//	quad->triangleIndex = { 0, 1, 2, 0, 2, 3 };
//	quad->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
//	quad->vertices = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
//	quad->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
//	quad->initGl();
//	quad->name = "quad";
//	quad->path = "";
//	//quad->computeBoundingBox();
//
//	m_meshes["default"] = cube;
//	m_meshes["cube"] = cube;
//	m_meshes["cubeWireframe"] = cubeWireFrame;
//	m_meshes["capsuleWireframe"] = capsuleWireFrame;
//	m_meshes["sphereWireframe"] = sphereWireFrame;
//	m_meshes["plane"] = plane;
//	m_meshes["quad"] = plane;
//
//	m_defaults.push_back("default");
//	m_defaults.push_back("cube");
//	m_defaults.push_back("cubeWireframe");
//	m_defaults.push_back("capsuleWireframe");
//	m_defaults.push_back("sphereWireframe");
//	m_defaults.push_back("plane");
//	m_defaults.push_back("quad");
//}
//
//void MeshFactory::add(const std::string& name, Mesh* mesh)
//{
//	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
//		return;
//
//	mesh->name = name;
//	m_meshes[name] = mesh;
//}
//
//void MeshFactory::add(const std::string & name, const std::string & path)
//{
//	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
//		return;
//
//	Mesh* newMesh = new Mesh(path, name);
//
//	//newMesh->name = name;
//	m_meshes[name] = newMesh;
//}
//
//Mesh* MeshFactory::get(const std::string& name)
//{
//	return m_meshes[name];
//}
//
//bool MeshFactory::contains(const std::string& name)
//{
//	return m_meshes.find(name) != m_meshes.end();
//}
//
//void MeshFactory::drawUI()
//{
//	ImGui::PushID("meshFactory");
//
//	ImGui::PushItemWidth(70);
//	ImGui::InputText("name", name, 20);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	ImGui::PushItemWidth(180);
//	ImGui::InputFilePath("path", path, 50);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	if (ImGui::SmallButton("add"))
//	{
//		add(name, path); 
//	}
//
//
//	for (auto& m : m_meshes)
//	{
//		if (&m == nullptr)
//			continue;
//
//		ImGui::Text(m.first.c_str());
//		ImGui::SameLine();
//		ImGui::Text(m.second->path.c_str());
//	}
//
//	ImGui::PopID();
//}
//
//void MeshFactory::clear()
//{
//	for (auto& it = m_meshes.begin(); it != m_meshes.end();)
//	{
//		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
//		{
//			it->second->freeGl();
//			delete it->second;
//			it = m_meshes.erase(it);
//		}
//		else
//			it++;
//	}
//}
//
//void MeshFactory::save(Json::Value & entityRoot) const
//{
//	entityRoot["size"] = m_meshes.size();
//	int i = 0;
//	for (auto it = m_meshes.begin(); it != m_meshes.end(); it++)
//	{
//		entityRoot["keys"][i] = it->first;
//		entityRoot["values"][i] = it->second != nullptr ? it->second->path : "";
//		i++;
//	}
//}
//
//void MeshFactory::load(const Json::Value & entityRoot)
//{
//	int size = entityRoot.get("size", 0).asInt();
//	for (int i = 0; i < size; i++)
//	{
//		std::string meshName = entityRoot["keys"][i].asString();
//		std::string meshPath = entityRoot["values"][i].asString();
//		
//		//don't laod mesh without mesh path these meshes are renerated with code : 
//		if(meshPath != "")
//			add(meshName, meshPath);
//	}
//}
//
///////////////////////////////////////////
//
//SkeletalAnimationFactory::SkeletalAnimationFactory()
//{
//	
//}
//
//void SkeletalAnimationFactory::add(const std::string& meshName, const std::string& animationName, SkeletalAnimation* animation)
//{
//	m_animations[meshName][animationName] = animation;
//}
//
//SkeletalAnimation* SkeletalAnimationFactory::get(const std::string& meshName, const std::string& animationName)
//{
//	return m_animations[meshName][animationName];
//}
//
//bool SkeletalAnimationFactory::contains(const std::string& meshName, const std::string& animationName)
//{
//	return m_animations.find(meshName) != m_animations.end() && m_animations[meshName].find(animationName) != m_animations[meshName].end();
//}
//
//void SkeletalAnimationFactory::drawUI()
//{
//	ImGui::PushID("skeletalAnimationFactory");
//
//	/*
//	ImGui::PushItemWidth(70);
//	ImGui::InputText("name", name, 20);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	ImGui::PushItemWidth(180);
//	ImGui::InputFilePath("path", path, 50);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	if (ImGui::SmallButton("add"))
//	{
//		add(name, path);
//	}*/
//
//
//	for (auto& mesh : m_animations)
//	{
//		for (auto& anim : mesh.second)
//		{
//			if (anim.second == nullptr)
//				continue;
//
//			ImGui::Text( (mesh.first +"::"+ anim.first).c_str() );
//		}
//	}
//
//	ImGui::PopID();
//}
//
//std::map<std::string, std::map<std::string, SkeletalAnimation*>>::iterator SkeletalAnimationFactory::clear(const std::string& meshName)
//{
//	for (auto& it = m_animations[meshName].begin(); it != m_animations[meshName].end(); it++)
//	{
//		delete it->second;
//
//	}
//
//	return m_animations.erase(m_animations.find(meshName));
//}
//
//void SkeletalAnimationFactory::clear()
//{
//	for (auto& it = m_animations.begin(); it != m_animations.end();)
//	{
//		it = clear(it->first);
//	}
//}
//
//// TODO : for the moment animations are synchronized with meshes : 
//void SkeletalAnimationFactory::save(Json::Value & entityRoot) const
//{
//	//entityRoot["size"] = m_animations.size();
//	//int i = 0;
//	//for (auto it = m_animations.begin(); it != m_animations.end(); it++)
//	//{
//	//	entityRoot["keys"][i] = it->first;
//	//	entityRoot["values"][i] = it->second != nullptr ? it->second->path : "";
//	//	i++;
//	//}
//}
//
//void SkeletalAnimationFactory::load(const Json::Value & entityRoot)
//{
//	//int size = entityRoot.get("size", 0).asInt();
//	//for (int i = 0; i < size; i++)
//	//{
//	//	std::string animationName = entityRoot["keys"][i].asString();
//	//	std::string animationPath = entityRoot["values"][i].asString();
//
//	//	//don't laod mesh without mesh path these meshes are renerated with code : 
//	//	if (animationPath != "")
//	//		add(animationName, animationPath);
//	//}
//}
//
//
////////////////////////////////////////////
//
//MaterialFactory::MaterialFactory()
//{
//	Material* newMat = new MaterialLit(getProgramFactory().get("defaultLit"), getTextureFactory().get("default"), getTextureFactory().get("default"), getTextureFactory().get("default"), 50);
//	newMat->name = "default";
//	m_materials["default"] = newMat;
//
//	newMat = new MaterialUnlit(getProgramFactory().get("defaultUnlit"));
//	newMat->name = "wireframe";
//	m_materials["wireframe"] = newMat;
//
//	newMat = new MaterialInstancedUnlit(getProgramFactory().get("wireframeInstanced"));
//	newMat->name = "wireframeInstanced";
//	m_materials["wireframeInstanced"] = newMat;
//
//	newMat = new MaterialGrassField(getProgramFactory().get("defaultGrassField"));
//	newMat->name = "grassfield";
//	m_materials["grassfield"] = newMat;
//
//	newMat = new MaterialBillboard(getProgramFactory().get("defaultBillboard"));
//	newMat->name = "billboard";
//	m_materials["billboard"] = newMat;
//
//	newMat = new MaterialParticles(getProgramFactory().get("defaultParticles"));
//	newMat->name = "particles";
//	m_materials["particles"] = newMat;
//
//	newMat = new MaterialParticlesCPU(getProgramFactory().get("defaultParticlesCPU"));
//	newMat->name = "particlesCPU";
//	m_materials["particlesCPU"] = newMat;
//
//	newMat = new MaterialParticleSimulation(getProgramFactory().get("particleSimulation"));
//	newMat->name = "particleSimulation";
//	m_materials["particleSimulation"] = newMat;
//
//	newMat = new MaterialDebugDrawer(getProgramFactory().get("debugDrawer"));
//	newMat->name = "debugDrawer";
//	m_materials["debugDrawer"] = newMat;
//	
//	m_defaults.push_back("default");
//	m_defaults.push_back("wireframe");
//	m_defaults.push_back("wireframeInstanced");
//	m_defaults.push_back("grassfield");
//	m_defaults.push_back("billboard");
//	m_defaults.push_back("particles");
//	m_defaults.push_back("particlesCPU");
//	m_defaults.push_back("particleSimulation");
//	m_defaults.push_back("debugDrawer");
//}
//
//void MaterialFactory::add(const std::string& name, Material* material)
//{
//	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
//		return;
//
//	material->name = name;
//	m_materials[name] = material;
//}
//
////Material* MaterialFactory::get(const std::string& name)
////{
////	return m_materials[name];
////}
//
//void MaterialFactory::drawUI()
//{
//	ImGui::PushID("materialFactory");
//
//	ImGui::PushItemWidth(70);
//	ImGui::InputText("name", name, 20);
//	ImGui::PopItemWidth();
//	ImGui::SameLine();
//	if (ImGui::SmallButton("add"))
//	{
//		add(name, new MaterialLit() );
//	}
//
//
//	for (auto& m : m_materials)
//	{
//		if (ImGui::CollapsingHeader(m.first.c_str()))
//		{
//			m.second->drawUI();
//		}
//	}
//
//	ImGui::PopID();
//}
//
//void MaterialFactory::clear()
//{
//	for (auto& it = m_materials.begin(); it != m_materials.end();)
//	{
//		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
//		{
//			delete it->second;
//			it = m_materials.erase(it);
//		}
//		else
//			it++;
//	}
//}
//
//void MaterialFactory::save(Json::Value & entityRoot) const
//{
//
//	//We only save lit materials : 
//	std::vector<MaterialLit*> litMaterials;
//	for (auto it = m_materials.begin(); it != m_materials.end(); it++)
//	{
//		MaterialLit* matLitCasted = dynamic_cast<MaterialLit*>(it->second);
//		if (matLitCasted != nullptr)
//		{
//			litMaterials.push_back(matLitCasted);
//		}
//	}
//
//	entityRoot["size"] = litMaterials.size();
//
//	for (int i = 0; i < litMaterials.size(); i++)
//	{
//		if (std::find(m_defaults.begin(), m_defaults.end(), name) == m_defaults.end()) //we don't save default materials
//		{
//			entityRoot["keys"][i] = litMaterials[i]->name;
//			litMaterials[i]->save(entityRoot["values"][i]);
//		}
//	}
//}
//
//void MaterialFactory::load(const Json::Value & entityRoot)
//{
//	//we only load lit materials : 
//	int size = entityRoot.get("size", 0).asInt();
//	for (int i = 0; i < size; i++)
//	{
//		std::string materialName = entityRoot["keys"][i].asString();
//		if (std::find(m_defaults.begin(), m_defaults.end(), name) == m_defaults.end()) //we don't load default materials
//		{
//			MaterialLit* newMaterial = new MaterialLit();
//			newMaterial->load(entityRoot["values"][i]);
//			add(materialName, newMaterial);
//		}
//	}
//}
