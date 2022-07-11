#pragma once

/*
 * Copyright © 2022 PHTNC<>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
**/

#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "core.hpp"
#include "opcodes.hpp"
#include "../vmb/vmb.hpp"

namespace vman::core
{
	class InterpreterContext
	{
	private:
		/*
		 * During startup, virtual man reads an ifstream and copies each byte into this vector.
		 * VirtualMAN handles code through memory IO rather than file IO.
		**/
		std::vector<char> fileBytes;

		/*
		 * This array defines the virtual registers that are used by virtual man
		 * to store values that are being moved through the MOV opcode.
		 * This is also used by NFC to determine the location of each parameter value.
		 */
		std::array<s32, 12> Registers = {};

	public:
		bool OpenFile(const std::string&);
		std::uint32_t Execute(void);
	};
};