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

#include <Windows.h>
#include <Psapi.h>

#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <map>

#include "../core/types.hpp"
#include "dyncall/dyncall.h"
#include "dyncall/dyncall_args.h"
#include "dyncall/dyncall_callf.h"
#include "dyncall/dynload.h"

#if defined (_MSC_VER)

#pragma warning ( push )
#pragma warning ( disable : 6387 )

#endif // _MSC_VER

namespace vman::vmb
{
	class Bridge
	{
	private:
		// A HANDLE to our own application, required for debugging purposes
		HANDLE hProc;
		DCCallVM* vm;

	public:
		/*
		 * During the execution of a native function in runtime, virtual man
		 * creates tokens that represent the parameters that will be pushed onto the stack.
		 * Through this, virtual man decides as what datatype each value gets pushed onto the stack.
		**/
		struct Parameter
		{
			// VirtualMAN requires the type of the value, so it can push the value correctly to the stack
			int paramType;

			// This represents the value that VirtualMAN pushes to the stack before executing a function.
			void* value;
		};

		/*
		 * This defines a list of possible datatypes that can be handled by virtual man.
		 * Note that VMBPOINTER is actually a char pointer representation inside virtual man.
		**/
		enum
		{
			VMBCHAR = 0x09,
			VMBBOOL = 0x01,
			VMBSHORT = 0x02,
			VMBINT = 0x03,
			VMBLONG = 0x04,
			VMBLONG_LONG = 0x05,
			VMBFLOAT = 0x06,
			VMBDOUBLE = 0x07,
			VMBPOINTER = 0x08,
		};

		Bridge(void);
		~Bridge(void);

		/*
		 * This conversion first passes a pointer address to a large enough variable, which is a uintptr.
		 * Depending on the compilation target, uintptr is either 4 bytes (32 bits) or 8 bytes (64 bits)
		 * Then we cast to the target type, whose size ranges from 1 to 8 bytes.
		 * This is mainly used to convert from void* to a target type.
		**/
		#define doConvert(x) static_cast<T>(reinterpret_cast<std::uintptr_t>(x))

		/*
		 * Due to the way on how a C++ handles translation units, we cannot create a templated function prototype
		 * and create an implementation inside the source files, this is a drawback of C++.
		**/
		template<class T>
		T CallNativeFunction(CCCSTR libName, CCCSTR funcName, const std::vector<Parameter>& params) noexcept
		{
			/*
			 * Dyncall operates through its own virtual machine, however, it's entirely managed by VirtualMAN.
			 * VirtualMAN decides according to the opcodes on what data is pushed into dyncall's stack and how a function is called.
			**/
			if (vm == nullptr)
			{
				vm = dcNewCallVM(4096);
				if (vm == nullptr) return doConvert(nullptr);

				/* 
				 * This function determines the ABI target.
				 * We're going to use the default target, this means that dyncall
				 * tries to find out what ABI it is running on and then operates conform to the detected ABI.
				**/
				dcMode(vm, DC_CALL_C_DEFAULT);
			}

			/*
			 * This function resets the dyncall stack
			**/
			dcReset(vm);

			FARPROC funcPtr = GetProcAddress(GetModuleHandleA(libName), funcName);
			printf("%s loaded at : 0x%p\n", libName, GetModuleHandleA("User32.dll"));
			printf("%s loaded at: 0x%p\n", funcName, funcPtr);
			if (funcPtr == nullptr) return doConvert(funcPtr);

			for (size_t i = 0; i < params.size(); ++i)
			{
				switch (params[i].paramType)
				{
				case VMBCHAR: dcArgChar(vm, *reinterpret_cast<char*>(params[i].value)); break;
				case VMBBOOL: dcArgBool(vm, *reinterpret_cast<bool*>(params[i].value)); break;
				case VMBSHORT: dcArgShort(vm, *reinterpret_cast<short*>(params[i].value)); break;
				case VMBINT: dcArgInt(vm, *reinterpret_cast<int*>(params[i].value)); break;
				case VMBLONG: dcArgLong(vm, *reinterpret_cast<long*>(params[i].value)); break;
				case VMBLONG_LONG: dcArgLongLong(vm, *reinterpret_cast<long long*>(params[i].value)); break;
				case VMBPOINTER:
				default: dcArgPointer(vm, params[i].value); break;
				}
			}
			return doConvert(dcCallPointer(vm, funcPtr));
		}
	};
};

#if defined (_MSC_VER)

#pragma warning ( pop )

#endif // _MSC_VER