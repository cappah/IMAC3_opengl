#pragma once

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <vector>



struct AABB
{
	glm::vec3 center;
	float halfSize;

	AABB(const glm::vec3& _center, float _halfSize) : center(_center), halfSize(_halfSize)
	{

	}

	bool contains(const glm::vec3& point)
	{
		//is position inside current node ?
		return(center.x - halfSize <= point.x && center.x + halfSize > point.x &&
			center.y - halfSize <= point.y && center.y + halfSize > point.y &&
			center.z - halfSize <= point.z && center.z + halfSize > point.z);
	}

	bool contains(const glm::vec3& otherCenter, float otherHalfSize)
	{
		return(center.x - halfSize <= otherCenter.x - otherHalfSize && center.x + halfSize > otherCenter.x + otherHalfSize &&
			center.y - halfSize <= otherCenter.y - otherHalfSize && center.y + halfSize > otherCenter.y + otherHalfSize &&
			center.z - halfSize <= otherCenter.z - otherHalfSize && center.z + halfSize > otherCenter.z + otherHalfSize);
	}

	bool containsOrIntersects(const glm::vec3& otherCenter, float otherHalfSize)
	{
		return(((center.x - halfSize < otherCenter.x - otherHalfSize && center.x + halfSize >= otherCenter.x - otherHalfSize) || (center.x - halfSize < otherCenter.x + otherHalfSize && center.x + halfSize >= otherCenter.x + otherHalfSize)) &&
			((center.y - halfSize < otherCenter.y - otherHalfSize && center.y + halfSize >= otherCenter.y - otherHalfSize) || (center.y - halfSize < otherCenter.y + otherHalfSize && center.y + halfSize >= otherCenter.y + otherHalfSize)) &&
			((center.z - halfSize < otherCenter.z - otherHalfSize && center.z + halfSize >= otherCenter.z - otherHalfSize) || (center.z - halfSize < otherCenter.z + otherHalfSize && center.z + halfSize >= otherCenter.z + otherHalfSize)));
	}

	bool containedIn(const glm::vec3& otherCenter, float otherHalfSize)
	{
		return( (center.x - halfSize > otherCenter.x - otherHalfSize && center.x + halfSize <= otherCenter.x + otherHalfSize ) &&
			center.y - halfSize > otherCenter.y - otherHalfSize && center.y + halfSize <= otherCenter.y + otherHalfSize &&
			center.z - halfSize > otherCenter.z - otherHalfSize && center.z + halfSize <= otherCenter.z + otherHalfSize);
	}

	bool containedInOrIntersectedBy(const glm::vec3& otherCenter, float otherHalfSize)
	{
		return( ( (center.x - halfSize > otherCenter.x - otherHalfSize && center.x - halfSize <= otherCenter.x + otherHalfSize) || (center.x + halfSize > otherCenter.x - otherHalfSize && center.x + halfSize <= otherCenter.x + otherHalfSize) ) &&
			( (center.y - halfSize > otherCenter.y - otherHalfSize && center.y - halfSize <= otherCenter.y + otherHalfSize) || (center.y + halfSize > otherCenter.y - otherHalfSize && center.y + halfSize <= otherCenter.y + otherHalfSize) ) &&
			( (center.z - halfSize > otherCenter.z - otherHalfSize && center.z - halfSize <= otherCenter.z + otherHalfSize) || (center.z + halfSize > otherCenter.z - otherHalfSize && center.z + halfSize <= otherCenter.z + otherHalfSize) ) );
	}
};

//A node placed in an octree. It is an AABB bounding box with 8 childs.
//Its size is 2*halfSize and its position is its parameter center.
template<typename T>
struct OctreeNode
{
	OctreeNode* childs[8];
	AABB* childBounds[8];

	glm::vec3 center;
	float halfSize;

	std::vector<glm::vec3> positions;
	std::vector<T*> elements;

	OctreeNode(glm::vec3 _center, float _halfSize) : center(_center), halfSize(_halfSize)
	{
		for (int i = 0; i < 8; i++)
			childs[i] = nullptr;

		childBounds[0] = new AABB(_center + glm::vec3(0.5f, 0.5f, 0.5f)*_halfSize, _halfSize*0.5f);

		childBounds[1] = new AABB(_center + glm::vec3(0.5f, 0.5f, -0.5f)*_halfSize, _halfSize*0.5f);
		childBounds[2] = new AABB(_center + glm::vec3(0.5f, -0.5f, 0.5f)*_halfSize, _halfSize*0.5f);
		childBounds[3] = new AABB(_center + glm::vec3(-0.5f, 0.5f, 0.5f)*_halfSize, _halfSize*0.5f);

		childBounds[4] = new AABB(_center + glm::vec3(0.5f, -0.5f, -0.5f)*_halfSize, _halfSize*0.5f);
		childBounds[5] = new AABB(_center + glm::vec3(-0.5f, 0.5f, -0.5f)*_halfSize, _halfSize*0.5f);
		childBounds[6] = new AABB(_center + glm::vec3(-0.5f, -0.5f, 0.5f)*_halfSize, _halfSize*0.5f);

		childBounds[7] = new AABB(_center + glm::vec3(-0.5f, -0.5f, -0.5f)*_halfSize, _halfSize*0.5f);
	}

	~OctreeNode()
	{
		for (int i = 0; i < 8; i++)
		{
			delete childBounds[i];
			if(childs[i] != nullptr)
				delete childs[i];
		}
	}

	//bool contains(const glm::vec3& point);
	//bool contains(T* element);
	//bool containedIn(const glm::vec3& position, float halfSize);
	void add(T* element, const glm::vec3& positon, int currentDepth, int maxDepth);
	void remove(T* element, const glm::vec3& positon, int currentDepth, int maxDepth);
	void remove(T* element, int currentDepth, int maxDepth);
	T* find(const glm::vec3& position, int currentDepth, int maxDepth);
	void findAll(const glm::vec3& position, int currentDepth, int maxDepth, std::vector<T*>& results);
	void findNeighbors(glm::vec3 position, float radius, int currentDepth, int maxDepth, std::vector<T*>& results);
	//recursivly get all centers and sizes of active nodes : 
	void getAllCenterAndSize(std::vector<glm::vec3>& centers, std::vector<float>& halfSizes);
	//get all elements contains in the current node and its childrens : 
	void getAllElements(std::vector<T*>& elements);
	//get all elements contains in the current node and its childrens if they are contains into a sphere of given center and radius : 
	void getAllElements(std::vector<T*>& _elements, const glm::vec3& center, float radius);
	//get the number of childs which are active (ie contains at least one element) : 
	int getActiveChildCount() const;
	//get number of elements contained in the node : 
	int getElementCount() const;
};

template<typename T>
int OctreeNode<T>::getElementCount() const
{
	return elements.size();
}

template<typename T>
int OctreeNode<T>::getActiveChildCount() const
{
	int sum = 0;
	for (int i = 0; i < 8; i++){
		if (childs[i] != nullptr){
			sum++;
		}
	}
	return sum;
}

template<typename T>
void OctreeNode<T>::getAllCenterAndSize(std::vector<glm::vec3>& centers, std::vector<float>& halfSizes)
{
	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr && (childs[i]->getElementCount() > 0 || childs[i]->getActiveChildCount() > 0) )
		{
			centers.push_back(childBounds[i]->center);
			halfSizes.push_back(childBounds[i]->halfSize);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr && (childs[i]->getElementCount() > 0 || childs[i]->getActiveChildCount() > 0))
		{
			childs[i]->getAllCenterAndSize(centers, halfSizes);
		}
	}
}

template<typename T>
void OctreeNode<T>::getAllElements(std::vector<T*>& _elements)
{
	for (int i = 0; i < elements.size(); i++)
	{
		_elements.push_back(elements[i]);
	}

	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr)
		{
			childs[i]->getAllElements(_elements);
		}
	}
}

template<typename T>
void OctreeNode<T>::getAllElements(std::vector<T*>& _elements, const glm::vec3& center, float radius)
{
	for (int i = 0; i < positions.size(); i++)
	{
		if (glm::distance(positions[i], center) < radius)
			_elements.push_back(elements[i]);
	}

	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr)
		{
			childs[i]->getAllElements(_elements, center, radius);
		}
	}
}


//template<typename T>
//bool OctreeNode<T>::contains(const glm::vec3& point)
//{
//	//is position inside current node ?
//	return(nodeCenter.x - nodeHalfSize < position.x && nodeCenter.x + nodeHalfSize > position.x &&
//		nodeCenter.y - nodeHalfSize < position.y && nodeCenter.y + nodeHalfSize > position.y &&
//		nodeCenter.z - nodeHalfSize < position.z && nodeCenter.z + nodeHalfSize > position.z);
//}
//
//template<typename T>
//bool OctreeNode<T>::contains(T* element)
//{
//	auto findIt = std::find(elements.begin(), elements.end(), element);
//
//	return (findIt != elements.end());
//}
//
//template<typename T>
//bool OctreeNode<T>::containedIn(const glm::vec3& position, float halfSize)
//{
//	return(nodeCenter.x - nodeHalfSize > position.x - halfSize && nodeCenter.x + nodeHalfSize < position.x + halfSize &&
//		nodeCenter.y - nodeHalfSize > position.y - halfSize && nodeCenter.y + nodeHalfSize < position.y + halfSize &&
//		nodeCenter.z - nodeHalfSize > position.z - halfSize && nodeCenter.z + nodeHalfSize < position.z + halfSize);
//}

template<typename T>
void OctreeNode<T>::add(T* element, const glm::vec3& position, int currentDepth, int maxDepth)
{
	//max depth reached, we add the element to the current node : 
	if (currentDepth >= maxDepth)
	{
		elements.push_back(element);
		positions.push_back(position);

		return;
	}

	//max depth not reached, we test childs :
	for (int i = 0; i < 8; i++)
	{
		if ( childBounds[i]->contains(position) )
		{
			if (childs[i] == nullptr)
				childs[i] = new OctreeNode<T>(childBounds[i]->center, childBounds[i]->halfSize);

			childs[i]->add(element, position, (currentDepth+1), maxDepth);

			return;
		}
	}
	//none of childs contains the current position, and max depth not reached (shouldn't happend for insertion based only on position...), 
	//we add the element to the current node :
	elements.push_back(element);
	positions.push_back(position);
}


template<typename T>
void OctreeNode<T>::remove(T* element, const glm::vec3& position, int currentDepth, int maxDepth)
{
	//max depth reached, we try to remove the element to the current node : 
	if (currentDepth >= maxDepth)
	{
		 std::vector<T*>::iterator nextIt = elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
		 int eraseIdx = std::distance(elements.begin(), nextIt);
		 if(eraseIdx < positions.size())
			positions.erase(positions.begin() + eraseIdx);

		 return;
	}

	//max depth not reached, we test childs :
	for (int i = 0; i < 8; i++)
	{
		if (childBounds[i]->contains(position))
		{
			childs[i]->remove(element, position, ++currentDepth, maxDepth);

			if (childs[i]->getElementCount() <= 0 && childs[i]->getActiveChildCount() <= 0) 
			{
				delete childs[i];
				childs[i] = nullptr;
			}

			return;
		}
	}
	//none of childs contains the current position, and max depth not reached (shouldn't happend for insertion based only on position...), 
	//we try to remove the element to the current node :
	std::vector<T*>::iterator nextIt = elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
	int eraseIdx = std::distance(elements.begin(), nextIt);
	if (eraseIdx < positions.size())
		positions.erase(positions.begin() + eraseIdx);
}


template<typename T>
void OctreeNode<T>::remove(T* element, int currentDepth, int maxDepth)
{
	//We search the element in the current node : 
	auto findIt = std::find(elements.begin(), elements.end(), element);

	//if the element is found we remove it from the container : 
	if (findIt != elements.end()) {
		int eraseIdx = findIt - elements.begin();
		elements.erase(findIt);
		positions.erase(eraseIdx);

		return;
	}
	//otherwise, we launch the remove call recursivly on childs :  
	else 
	{
		for (int i = 0; i < 8; i++) 
		{
			if (childBounds[i]->contains(element)) 
			{
				childs[i]->remove(element, position, ++currentDepth, maxDepth);

				if (childs[i]->getElementCount() <= 0 && childs[i]->getActiveChildCount() <= 0)	
				{
					delete childs[i];
					childs[i] = nullptr;
				}

				return;
			}
		}
	}
}

template<typename T>
T*  OctreeNode<T>::find(const glm::vec3& position, int currentDepth, int maxDepth)
{
	//max depth reached : 
	if (currentDepth >= maxDepth)
	{
		auto findIt = std::find(positions.begin(), positions.end(), position);
		if (findIt != positions.end())
			return elements[findIt - positions.begin()];
		else
			return nullptr;
	}

	//max depth not reached, we test childs :
	glm::vec3 nodeCenter = currentNode->center;
	float nodeHalfSize = currentNode->halfSize;

	for (int i = 0; i < 8; i++)
	{
		if (childBounds[i]->contains(position))
		{
			return childs[i]->find(position, ++currentDepth, maxDepth);
		}
	}
	//none of childs contains the current position, and max depth not reached (shouldn't happend for insertion based only on position...) :
	auto findIt = std::find(positions.begin(), positions.end(), position);
	if (findIt != positions.end())
		return elements[findIt - positions.begin()];
	else
		return nullptr;
}

template<typename T>
void  OctreeNode<T>::findAll(const glm::vec3& position, int currentDepth, int maxDepth, std::vector<T*>& results)
{
	//max depth reached :
	if (currentDepth >= maxDepth)
	{
		auto findIt = std::find(positions.begin(), positions.end(), position);
		if (findIt != positions.end())
			results.push_back( elements[findIt - positions.begin()] );

		return;
	}

	//max depth not reached, we test childs :
	glm::vec3 nodeCenter = currentNode->center;
	float nodeHalfSize = currentNode->halfSize;

	for (int i = 0; i < 8; i++)
	{
		if (childBounds[i]->contains(position))
		{
			return childs[i]->findAll(position, ++currentDepth, maxDepth, results);
		}
	}
	//none of childs contains the current position, and max depth not reached (shouldn't happend for insertion based only on position...) :
	auto findIt = std::find(positions.begin(), positions.end(), position);
	if (findIt != positions.end())
		results.push_back(elements[findIt - positions.begin()]);
}

template<typename T>
void OctreeNode<T>::findNeighbors(glm::vec3 position, float radius, int currentDepth, int maxDepth, std::vector<T*>& results)
{
	//max depth reached (leaf) :
	if (currentDepth >= maxDepth)
	{
		for (int i = 0; i < positions.size(); i++)
		{
 			if( glm::distance(positions[i], position) < radius)
				results.push_back(elements[i]);
		}
		return;
	}

	//max depth not reached, we test childs :
	for (int i = 0; i < 8; i++)
	{
		if (childBounds[i]->containedIn(position, radius))
		{
			//recusivly get all elements stored in child node and its children : 
			if (childs[i] != nullptr)
				childs[i]->getAllElements(results, position, radius);
		}
		else if (childBounds[i]->containsOrIntersects(position, radius))
		{
			if (childs[i] != nullptr)
				childs[i]->findNeighbors(position, radius, (currentDepth+1), maxDepth, results);
		}
	}

}


//A simple octree with a maximal depth of maxDepth. The depth is the only parameter which influences the insertion of an element in the octree.
template<typename T>
class Octree
{
private:
	OctreeNode<T>* m_root;
	int m_maxDepth;
	glm::vec3 m_center;
	float m_halfSize;

public:
	Octree(glm::vec3 center, float halfSize, int maxDepth = 3);
	~Octree();

	//add an element at the given position : 
	void add(T* element, glm::vec3 position);
	//remove an element at the given position : 
	void remove(T* element, glm::vec3 position);
	//remove an element, searchinf the all tree : 
	void remove(T* element);

	//find an element at the given position : 
	T* find(glm::vec3 position);
	//Find all elements which are at the given position
	const std::vector<T*>& findAll(glm::vec3 position);
	//return all elements near the given position (inside the radius) : 
	void findNeighbors(glm::vec3 position, float radius, std::vector<T*>& results);

	//move an element from previousPosition to newPosition in the octree :
	void update(T* element, glm::vec3 previousPosition, glm::vec3 newPosition);

	//half size of the root : 
	float getHalfSize() const;
	//center of the root : 
	glm::vec3 getCenter() const;

	void getAllCenterAndSize(std::vector<glm::vec3>& centers, std::vector<float>& halfSizes);
};


template<typename T>
Octree<T>::Octree(glm::vec3 center, float halfSize, int maxDepth) : m_maxDepth(maxDepth), m_center(center), m_halfSize(halfSize)
{
	m_root = new OctreeNode<T>(center, halfSize);
}

template<typename T>
Octree<T>::~Octree()
{
	delete m_root;
}

template<typename T>
void Octree<T>::add(T * element, glm::vec3 position)
{
	m_root->add(element, position, 0, m_maxDepth);
}

template<typename T>
void Octree<T>::remove(T * element, glm::vec3 position)
{
	m_root->remove(element, position, 0, m_maxDepth);
}

template<typename T>
void Octree<T>::remove(T* element)
{
	m_root->remove(element, 0, m_maxDepth);
}

template<typename T>
T* Octree<T>::find(glm::vec3 position)
{
	return m_root->find(position, 0, m_maxDepth);
}

template<typename T>
const std::vector<T*>& Octree<T>::findAll(glm::vec3 position)
{
	std::vector<T*> results;
	m_root->findAll(position, 0, m_maxDepth, results);
	return results;
}

template<typename T>
void Octree<T>::findNeighbors(glm::vec3 position, float radius, std::vector<T*>& results)
{
	m_root->findNeighbors(position, radius, 0, m_maxDepth, results);
}

template<typename T>
void Octree<T>::update(T * element, glm::vec3 previousPosition, glm::vec3 newPosition)
{
	m_root->remove(element, previousPosition, 0, m_maxDepth);
	m_root->add(element, newPosition, 0, m_maxDepth);
}

template<typename T>
float Octree<T>::getHalfSize() const
{
	if (m_root != nullptr)
		return m_root->halfSize;
	else
		return 0;
}

template<typename T>
glm::vec3 Octree<T>::getCenter() const
{
	if (m_root != nullptr)
		return m_root->center;
	else
		return glm::vec3(0, 0, 0);
}

template<typename T>
void Octree<T>::getAllCenterAndSize(std::vector<glm::vec3>& centers, std::vector<float>& halfSizes)
{
	centers.push_back(m_root->center);
	halfSizes.push_back(m_root->halfSize);

	m_root->getAllCenterAndSize(centers, halfSizes);
}