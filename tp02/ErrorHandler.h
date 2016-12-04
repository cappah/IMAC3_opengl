#pragma once

#include <string>
#include <iostream>

#define PRINT_ERROR(message)\
{\
	std::cerr << "----------------- Error detected -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : " << message << std::endl;\
}

#define PRINT_ERROR()\
{\
	std::cerr << "----------------- Error detected -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : no message" << std::endl;\
}

#define PRINT_WARNING(message)\
{\
	std::cerr << "----------------- Warning -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : " << message << std::endl;\
}

#define PRINT_WARNING()\
{\
	std::cerr << "----------------- Warning -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : no message" << std::endl;\
}