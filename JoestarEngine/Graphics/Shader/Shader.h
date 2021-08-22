#pragma once
#include <string>
namespace Joestar {
	class Shader {
	public:
		inline std::string& GetName() { return name; };
		inline void SetName(std::string n) { name = n; };
	private:
		std::string name;
	};
}