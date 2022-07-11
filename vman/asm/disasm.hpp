#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include "../core/types.hpp"
#include "../core/opcodes.hpp"
#include "../vmb/vmb.hpp"


namespace vasm
{
	class Disassembler
	{
	private:
		std::vector<char> fileBytes;

	public:

		Disassembler(const std::string&);
		void Disassemble(void) const;
	};
}