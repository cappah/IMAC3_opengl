#pragma once

template<typename T>
class ISingleton
{
private:
	void operator=(const T& other){}

public:
	static T& get()
	{
		static T instance;

		return instance;
	}
};