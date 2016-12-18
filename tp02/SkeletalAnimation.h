#pragma once
#include <map>

#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Animation.h"
#include "FileHandler.h"
#include "Resource.h"

class SkeletalAnimation: public Animation, public Resource
{
	Assimp::Importer* importer; //TODO virer ça
	aiAnimation* m_animation; //Carefull : the animation will be deleted when the mesh is deleted
	
public:
	SkeletalAnimation();
	SkeletalAnimation(aiAnimation* animation);
	~SkeletalAnimation();
	void init(const FileHandler::CompletePath& path, const ID& id) override
	{
		Resource::init(path, id);

		const std::string animName = path.getSubFileName();
		assert(!animName.empty());

		bool Ret = false;
		if (importer != nullptr)
			delete importer;
		importer = new Assimp::Importer();

		const aiScene* pScene = importer->ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		
		if (pScene->HasAnimations())
		{
			//TODO : meilleur recherche
			for (int i = 0; i < pScene->mNumAnimations; i++)
			{
				if (animName.compare(pScene->mAnimations[i]->mName.C_Str()) == 0)
				{
					m_animation = pScene->mAnimations[i];
				}
			}
		}
	}

	float getDurationInTick() const;
	float getAnimationTime() const;
	void setIsLooping(bool isLooping);
	std::string getName() const;
	float getTicksPerSecond() const;
	aiNodeAnim* getNodeAnim(const std::string& nodeName) const;
};

//animations attached to a mesh
struct MeshAnimations
{
private:
	std::map<std::string, SkeletalAnimation> m_animations;

public:
	MeshAnimations(const FileHandler::CompletePath& scenePath);
	bool contains(const std::string& name);
	SkeletalAnimation& get(const std::string& name);
};

