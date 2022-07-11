#include "disasm.hpp"

using vasm::Disassembler;

using namespace vman;
using namespace vman::core;
using vmb::Bridge;

/*
 * The disassembler should be treated as a separate application.
 * Note that I still kept it in one project due to being a demonstration project
 * for my IT apprenticeship, can change in the future.
**/

Disassembler::Disassembler(const std::string& path)
{
	LoadLibrary("User32.dll");
	std::fstream fStream(path, std::ios::binary | std::ios::in);
	if (fStream.is_open())
	{
		std::uintmax_t size = std::filesystem::file_size(path);
		fileBytes.resize(size);
		if (fileBytes.size() != size)
		{
			std::cerr << "[ERROR] Failed to allocate memory.\n";
			return;
		}

		fStream.read(fileBytes.data(), size);

		if (!fStream)
		{
			std::cerr << "[ERROR] Failed to read file.\n";
			return;
		}
		fStream.close();
		return;
	}
	else std::cerr << "[ERROR] Failed to open file.\n";
	return;
}

void Disassembler::Disassemble(void) const
{
	std::size_t i;
	std::size_t vmSignature;

	memcpy(&i, &fileBytes[0], sizeof(std::size_t));
	memcpy(&vmSignature, &fileBytes[8], sizeof(std::size_t));

	/*
	 * Virtual man checks here if the given binary file has the 1911 KUQEZI (described in the project documentation)
	 * signature, if virtual man can't detect this signature, it will refuse to disassemble or execute the binary.
	**/
	if (vmSignature != 0x495A4551554B1119)
	{
		std::cerr << "[ERROR] This is not a compatible virtual man binary.\n";
		return;
	}

	std::cout << "\n[INFO] Analyzing loaded libraries...\n\n";

	HMODULE hMods[1024];
	DWORD cbNeeded;
	HANDLE hProc;
	DWORD pid = GetCurrentProcessId();

	std::cout << "[INFO] VirtualMAN instance PID: \033[1;31m" << pid << "\033[0m\n\n";

	hProc = OpenProcess
	(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, 
		pid
	);

	if (hProc == nullptr)
	{
		std::cerr << "[ERROR] Failed to retrieve process id.\n";
		return;
	}

	if (EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int j = 0; j < (cbNeeded / sizeof(HMODULE)); ++j)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleBaseNameA(hProc, hMods[j], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::cout << "[\033[1;33mRESOLVED LIBRARY\033[0m] " << szModName << " (\033[1;31m0x" << hMods[j] << "\033[0m)\n";
			}
		}
	}

	std::cout << "\n";
	CloseHandle(hProc);

	std::cout << "[INFO] Entry point starts at \033[1;31m0x" << std::hex << i << "\033[0m\n";
	std::cout << "[INFO] Corresponding VirtualMAN signature detected: 777 \033[41m\033[1;30mKUQ E ZI!\033[0m\n";
	std::cout << "[INFO] Data section has a size of \033[1;31m0x" << std::hex << i - 1 << "\033[0m\n";

	std::cout << "\n[INFO] Analyzing data section...\n\n";

	for (size_t j = 0x10; j < i; j++)
	{
		if (fileBytes[j] >= 0x21 && fileBytes[j] <= 0x7E)
		{
			std::cout << "\033[1;31m0x" << std::hex << j << "\033[0m: ";
			std::cout << "\033[1;33m";
			while (fileBytes[j])
			{
				std::cout << fileBytes[j];
				j++;
			}
			std::cout << "\n";
		}
	}

	std::cout << "\n\033[0m[INFO] \033[0mAnalyzing opcodes...\n\n";

	for (; i < fileBytes.size(); i++)
	{
		switch (fileBytes[i])
		{
			case MOV:
			{
				s32 movValue;

				// Directly converts a big endian encoded byte array
				movValue = ((fileBytes[i + 6] << 0) | (fileBytes[i + 5] << 8) | (fileBytes[i + 4] << 16) | (fileBytes[i + 3] << 24)) / 0x100;
				
				std::cout 
					<< "\033[1;31m0x" 
					<< std::hex << i 
					<< "\033[0m: mov \033[1;33mr" 
					<< static_cast<int>(fileBytes[i + 1]) 
					<< "\033[0m, " 
					<< std::hex 
					<< "\033[1;33m0x" 
					<< movValue 
					<< "\n";
				i += 5;
			} break;

			case NFC:
			{
				std::cout << "\033[1;31m0x" << std::hex << i << "\033[0m: " << "nfc ";
				std::cout << "\033[1;33m";
				while (fileBytes[i])
				{
					++i;

					switch (fileBytes[i])
					{
					case Bridge::VMBBOOL: std::cout << "bool"; break;
					case Bridge::VMBCHAR: std::cout << "char"; break;
					case Bridge::VMBSHORT: std::cout << "short"; break;
					case Bridge::VMBINT: std::cout << "int"; break;
					case Bridge::VMBLONG: std::cout << "long"; break;
					case Bridge::VMBLONG_LONG: std::cout << "long long"; break;
					case Bridge::VMBFLOAT: std::cout << "float"; break;
					case Bridge::VMBDOUBLE: std::cout << "double"; break;
					case Bridge::VMBPOINTER: std::cout << "ptr"; break;
					}

					if (fileBytes[i + 1] != '\0')
					{
						std::cout << "\033[0m, \033[1;33m";
					}
				}
				
				std::cout << "\n";
			} break;

			case ADD:
			{
				std::cout << "add " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case SUB:
			{
				std::cout << "sub " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case DIV:
			{
				std::cout << "div " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case MUL:
			{
				std::cout << "mul " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case MOD:
			{
				std::cout << "mod " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case LSH:
			{
				std::cout << "lsh " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case RSH:
			{
				std::cout << "rsh " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case AND:
			{
				std::cout << "and " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case OR:
			{
				std::cout << "or " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case XOR:
			{
				std::cout << "xor " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case NOT:
			{
				std::cout << "not " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
			}
			break;

			case JMP:
				std::cout << "jmp " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 2;
				break;

			case JIE:
				std::cout << "jie " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
				break;

			case JNE:
				std::cout << "jie " << static_cast<int>(fileBytes[i + 1]) << ", " << static_cast<int>(fileBytes[i + 2]) << ", " << static_cast<int>(fileBytes[i + 3]);
				i += 3;
				break;

			case NOP:
			default:
			{
				std::cout << "\033[1;31m0x" << std::hex << i << "\033[0m: " << "nop\n";
			} break;
		}
	}
	
	std::cout << "\033[0m";
}