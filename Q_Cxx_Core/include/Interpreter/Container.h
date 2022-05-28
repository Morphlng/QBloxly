#pragma once
#include <string>
#include "Cxx_Core_API.h"

namespace CXX {

	class CXXAPI Container
	{
	public:
		Container(std::string name) :type(std::move(name)) {}
		virtual ~Container() = default;
		virtual std::string to_string() = 0;

	public:
		std::string type;
	};

}