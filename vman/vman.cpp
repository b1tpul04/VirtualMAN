// vman.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "vman.h"
#include "core/interpreter.hpp"
#include "asm/disasm.hpp"


using namespace std;

int main(int argc, char* argv[])
{
	std::ios::sync_with_stdio(false);
	std::cout << "VirtualMAN Version " << vman::VERSION << " by " << "PHTNC<>" /*vman::AUTHOR*/ << "\n";
	std::cout << "Compilation date: " << __DATE__ << " " << __TIME__ << "\n";

	if (argc == 1)
	{
		std::cerr << "No arguments passed. USAGE: vman file.bin";
		return -1;
	}
	else
	{
		if (strcmp(argv[1], "-e") == 0)
		{
			vman::core::InterpreterContext context;
			context.OpenFile(argv[2]);
			context.Execute();
			return 0;
		}
		else if (strcmp(argv[1], "-d") == 0)
		{
			vasm::Disassembler disasm(argv[2]);
			disasm.Disassemble();
		}
		else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			std::cout << "USAGE: vman -e \"fileName.bin\" - Execute a virtual man compatible binary file.\n";
			std::cout << "USAGE: vman -d \"fileName.bin\" - Disassemble a virtual man compatible binary file.\n";
		}
		else
		{
			std::cerr << "Wrong parameter: " << argv[1] << "\n";
			std::cerr << "Type \"vman -h\" or \"vman --help\" for help.\n";
		}
	}
}
