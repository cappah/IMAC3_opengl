#include "TestBehavior.h"
//forwards : 
#include "BehaviorFactory.h"
#include "Scene.h"
#include "Coroutine.h"


//constexpr int tatu() {
//	static int tutu;
//	BehaviorFactory::get().add(std::type_index(typeid(TestBehavior)), new TestBehavior());
//	return 1;
//} 


REGISTER_BEHAVIOUR(TestBehavior);

TestBehavior::TestBehavior()
{
	
}

TestBehavior::~TestBehavior()
{
}

void TestBehavior::start(Scene& scene)
{
	entity()->startCoroutine < void > (std::function<void()>([]() { std::cout << "coroutine called !" << std::endl; }), 1.f);
}

void TestBehavior::update(Scene& scene)
{
	std::cout << "testBehaviour update" << std::endl;
}

void TestBehavior::save(Json::Value & entityRoot) const
{
	Behavior::save(entityRoot);
}

void TestBehavior::load(Json::Value & entityRoot)
{
	Behavior::load(entityRoot);
}
