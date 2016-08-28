#pragma once

template<typename T>
class ISingleton
{
private:
	void operator=(const T& other){}

public:
	static T& instance()
	{
		static T instance;

		return instance;
	}
};