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

#include "interpreter.hpp"

using vman::core::InterpreterContext;

bool InterpreterContext::OpenFile(const std::string& path)
{
	std::fstream fStream(path, std::ios::binary | std::ios::in);
	if (fStream.is_open())
	{
		std::uintmax_t size = std::filesystem::file_size(path);
		fileBytes.resize(size);
		if (fileBytes.size() != size)
		{
			std::cerr << "[ERROR] Failed to allocate memory.\n";
			return false;
		}

		fStream.read(fileBytes.data(), size);

		if (!fStream)
		{
			std::cerr << "[ERROR] Failed to read file.\n";
			return false;
		}
		fStream.close();
		return true;
	}
	else std::cerr << "[ERROR] Failed to open file.\n";
	return false;
}

std::uint32_t InterpreterContext::Execute(void)
{
	vmb::Bridge b;
	vmb::Bridge::Parameter params;
	std::vector<vmb::Bridge::Parameter> vec;
	LoadLibrary("User32.dll");
	/*
	 * This is the program counter of the virtual machine.
	**/
	std::size_t PC;

	/*
	 * This variable stores the signature of a binary file and checks for compatibility.
	**/
	std::size_t vmSignature;
	memcpy(&PC, &fileBytes[0], sizeof(std::size_t));
	memcpy(&vmSignature, &fileBytes[8], sizeof(std::size_t));

	/*
	 * Virtual man checks here if the given binary file has the 1911 KUQEZI signature, 
	 * if virtual man can't detect this signature, it will refuse to disassemble or execute the binary.
	**/

	if (vmSignature != 0x495A4551554B1119)
	{
		std::cerr << "[ERROR] This is not a compatible virtual man binary.\n";
		return 0x777;
	}

	for (; PC < fileBytes.size(); PC++)
	{
		switch (fileBytes[PC])
		{
			case NOP: // NOP DOES NOTHING AND JUST SKIPS.
				break;

			case NFC:
			{
				#pragma warning ( push )
				#pragma warning ( disable : 6263 )
				#pragma warning ( disable :  6387)

				/*
				 * Either allocates memory on the stack or the heap.
				 * Chosen by the runtime.
				**/
				char* funcName = static_cast<CSTR>(_malloca(64));
				char* libName = static_cast<CSTR>(_malloca(128));

				memset(funcName, 0, 64);
				memset(libName, 0, 128);

				/*
				 * Copy the ascii representation of the library and function name inside the array
				 * strncpy automatically stops execution after hitting a \0 or if the pointer index
				 * exceeds the maximum sizes.
				**/
				strncpy(libName, &fileBytes[Registers[0]], 128);
				strncpy(funcName, &fileBytes[Registers[1]], 64);

				/*
				 * Perhaps a stupid name for this variable, anyways,
				 * in the binary stream, VirtualMAN reserves the first and second index (i + 0 and i + 1) for the opcode and return type
				 * Register 0 and 1 are reserved for library and function name.
				 *
				**/
				size_t regIndex = 2;

				/*
				 * Tokenize each parameter for the function passing.
				**/
				while (fileBytes[PC + regIndex])
				{
					params.paramType = fileBytes[PC + regIndex];
					params.value = &fileBytes[Registers[regIndex]];
					vec.push_back(params);
					regIndex++;
				}
				switch (fileBytes[PC + 1])
				{
				case vmb::Bridge::VMBCHAR: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<char>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBBOOL: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<bool>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBSHORT: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<short>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBINT: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<int>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBLONG: 
				{
					Registers[2] = static_cast<s32>(b.CallNativeFunction<long>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBLONG_LONG: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<long long>(libName, funcName, vec));
				} break;

				case vmb::Bridge::VMBFLOAT: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<float>(libName, funcName, vec));
				} break;
				case vmb::Bridge::VMBDOUBLE: 
				{ 
					Registers[2] = static_cast<s32>(b.CallNativeFunction<double>(libName, funcName, vec));
				} break;

				default: { std::cerr << "[ERROR] Failed to perform native call.\n"; } break;

				#pragma warning ( pop )
				}

				PC += regIndex + 1;
				_freea(funcName);
				_freea(libName);
			}
			
			break;

			case MOV:
			{
				s32 movValue;

				// Directly converts a big endian encoded byte array
				movValue = ((fileBytes[PC + 6] << 0) | (fileBytes[PC + 5] << 8) | (fileBytes[PC + 4] << 16) | (fileBytes[PC + 3] << 24)) / 0x100;
				Registers[fileBytes[PC + 1]] = movValue;
				PC += 5;
			}
			break;

			case ADD:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] + Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case SUB:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] - Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case DIV:
			{
				if (Registers[fileBytes[PC + 3]] == 0)
				{
					std::cerr << "[INTERNAL EXCEPTION] CODE EXECUTION HALTED. DIVISION BY ZERO ERROR.\n";
					std::cerr << "[REGISTER " << fileBytes[PC + 3] << "]: " << Registers[fileBytes[PC + 3]] << "\n";
					while (!getchar());
					exit(-1);
				}
				else
				{
					Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] / Registers[fileBytes[PC + 3]];
					PC += 3;
				}
			}
			break;

			case MUL:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] * Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case MOD:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] % Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case LSH:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] << Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case RSH:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] >> Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case AND:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] & Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case OR:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] | Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case XOR:
			{
				Registers[fileBytes[PC + 1]] = Registers[fileBytes[PC + 2]] ^ Registers[fileBytes[PC + 3]];
				PC += 3;
			}
			break;

			case NOT:
			{
				Registers[fileBytes[PC + 1]] = ~Registers[fileBytes[PC + 2]];
				PC += 2;
			}
			break;

			case JMP:
				PC = Registers[fileBytes[PC + 1]];
				break;

			case JIE:
				if (Registers[fileBytes[PC + 1]] == Registers[PC + 2])
				{
					PC = Registers[fileBytes[PC + 3]];
				}
				PC += 3;
				break;

			case JNE:
				if (Registers[fileBytes[PC + 1]] != Registers[PC + 2])
				{
					PC = Registers[fileBytes[PC + 3]];
				}
				PC += 3;
				break;
		}
	}
	return 0;
}
