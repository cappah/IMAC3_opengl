#include "Animator.h"
//forwards : 
#include "Scene.h"
#include "Factories.h"

#include "EditorGUI.h"


Animator::Animator(): Component(ComponentType::ANIMATOR), m_skeleton(nullptr), m_currentAnimName(""), m_currentSkeletonName(""), m_isPlaying(false)
{
}


Animator::~Animator()
{
}

void Animator::setSkeleton(Skeleton * skeleton)
{
	m_skeleton = skeleton;
}

void Animator::addAnimation(SkeletalAnimation * animation)
{
	m_animations[animation->getName()] = animation;
}

void Animator::removeAnimation(SkeletalAnimation * animation)
{
	auto findIt = std::find_if(m_animations.begin(), m_animations.end(), [&animation](const std::pair<std::string, SkeletalAnimation*>& key) { return key.second == animation; });
	m_animations.erase(findIt);
	//m_animations.erase(std::remove(m_animations.begin(), m_animations.end(), animation), m_animations.end());
}

void Animator::updateAnimations(float timeInSecond)
{
	if (m_isPlaying && m_skeleton != nullptr && m_animations.find(m_currentAnimName) != m_animations.end())
		m_skeleton->playAnimationStep(timeInSecond, *m_animations[m_currentAnimName]);
}

void Animator::play()
{
	m_isPlaying = true;
}

void Animator::play(const std::string & animationName)
{
	m_isPlaying = true;
	m_currentAnimName = animationName;
}

void Animator::drawUI(Scene & scene)
{
	char tmpSkeletalPath[100];
	m_skeletonName.copy(tmpSkeletalPath, m_skeletonName.size());
	tmpSkeletalPath[m_skeletonName.size()] = '\0';

	//%NOCOMMIT%
	//if (ImGui::InputText("skeleton/mesh name", tmpSkeletalName, 20)) {

	//	m_skeletonName = tmpSkeletalName;

	//	if (getMeshFactory().contains(m_skeletonName)) {
	//		ResourcePtr<Mesh> tmpMesh = getMeshFactory().get(m_skeletonName);
	//		if (tmpMesh.isValid()) {
	//			m_currentSkeletonName = m_skeletonName;
	//			m_skeleton = tmpMesh->getSkeleton();
	//		}
	//	}
	//}

	//Get mesh skeleton from mesh
	ResourcePtr<Mesh> resourcePtrQuery;
	EditorGUI::ResourceField<Mesh>(resourcePtrQuery, "skeleton/mesh name", tmpSkeletalPath, 100);
	if (resourcePtrQuery.isValid())
	{
		m_skeleton = resourcePtrQuery->getSkeleton();
		m_skeletonAnimations = getSkeletalAnimationFactory().get(FileHandler::CompletePath(tmpSkeletalPath));
	}


	char tmpAnimationName[60];
	m_animationName.copy(tmpAnimationName, m_animationName.size());
	tmpAnimationName[m_animationName.size()] = '\0';

	//%NOCOMMIT%
	//if (ImGui::InputText("animation name", tmpAnimationName, 60))
	//	m_animationName = tmpAnimationName;
	//ImGui::SameLine();
	//if (ImGui::Button("add")){
	//	if (getSkeletalAnimationFactory().contains(m_currentSkeletonName, m_animationName)) {
	//		SkeletalAnimation* tmpAnim = getSkeletalAnimationFactory().get(m_currentSkeletonName, m_animationName);
	//		if (tmpAnim != nullptr) {
	//			if (m_animations.find(m_currentAnimName) == m_animations.end())
	//				m_currentAnimName = m_animationName;
	//			m_animations[m_animationName] = tmpAnim;
	//		}
	//	}
	//}

	//Get mesh skeleton from mesh
	ResourcePtr<MeshAnimations> resourcePtrQuery;
	EditorGUI::ResourceField<MeshAnimations>(resourcePtrQuery, "mesh animations", tmpSkeletalName, 20);
	if (resourcePtrQuery.isValid())
	{
		m_skeleton = resourcePtrQuery->getSkeleton();
	}

	int imguiId = 0;
	for (auto& it = m_animations.begin(); it != m_animations.end();) {

		ImGui::PushID(imguiId);

		ImGui::Text(it->first.c_str());
		ImGui::SameLine();
		if (ImGui::Button("remove")) {
			if (m_currentAnimName == it->first)
				m_currentAnimName = m_animations.size() > 0 ? m_animations.begin()->first : "";
			it = m_animations.erase(it);
		}
		else
			it++;

		ImGui::PopID();
		imguiId++;
	}
}

void Animator::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Animator::addToScene(Scene & scene)
{
	scene.add(this);
}

void Animator::eraseFromEntity(Entity & entity)
{
	entity.erase(this);
}

void Animator::addToEntity(Entity & entity)
{
	entity.add(this);
}

Component * Animator::clone(Entity * entity)
{
	Animator* newAnimator= new Animator(*this);

	newAnimator->attachToEntity(entity);

	return newAnimator;
}

void Animator::save(Json::Value & componentRoot) const
{
	Component::save(componentRoot);

	componentRoot["skeletonName"] = m_currentSkeletonName;

	componentRoot["animationCount"] = m_animations.size();
	int animationId = 0;
	for (auto it = m_animations.begin(); it != m_animations.end(); it++)
		componentRoot["animationName"][animationId] = it->first;

	componentRoot["currentAnimName"] = m_currentAnimName;
}

void Animator::load(Json::Value & componentRoot)
{
	Component::load(componentRoot);

	m_currentSkeletonName = componentRoot.get("skeletonName", "").asString();
	m_skeletonName = m_currentSkeletonName;
	auto mesh = getMeshFactory().get(m_currentSkeletonName);
	if (mesh.isValid()) {
		m_skeleton = mesh->getSkeleton();
	}

	int animationCount = componentRoot.get("animationCount", 0).asInt();

	for (int i = 0; i < animationCount; i++) {

		std::string animationName = componentRoot["animationName"][i].asString();
		if (getSkeletalAnimationFactory().contains(m_currentSkeletonName, animationName)) {
			SkeletalAnimation* tmpAnim = getSkeletalAnimationFactory().get(m_currentSkeletonName, animationName);
			if (tmpAnim != nullptr) {
				if (m_animations.find(m_currentAnimName) == m_animations.end())
					m_currentAnimName = animationName;
				m_animations[animationName] = tmpAnim;
			}
		}
	}

	m_currentAnimName = componentRoot.get("currentAnimName", "").asString();
}
