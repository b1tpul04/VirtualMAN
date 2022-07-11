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

#include "types.hpp"

namespace vman::core
{
	// NOP value, does nothing but skips values.
	constexpr const u8 NOP = 0xF0;
	// Add two values
	constexpr const u8 ADD = 0x10;
	// Subtract one value from another
	constexpr const u8 SUB = 0x11;
	// Divide one value from another
	constexpr const u8 DIV = 0x12;
	// Multiply two values together
	constexpr const u8 MUL = 0x13;
	// Modulo two values
	constexpr const u8 MOD = 0x14;
	// Bitshifting to left
	constexpr const u8 LSH = 0x15;
	// Bitshifting to right
	constexpr const u8 RSH = 0x16;
	// Negates a value
	constexpr const u8 NOT = 0x17;
	// ANDs two values together
	constexpr const u8 AND = 0x18;
	// ORs two values together
	constexpr const u8 OR = 0x19;
	// Performs an eXclusive OR on two values
	constexpr const u8 XOR = 0x1A;
	// Jump to a specified location
	constexpr const u8 JMP = 0x20;
	// Jump if values of two registers are equal
	constexpr const u8 JIE = 0x21;
	// Jump if values of two specified registers are not equal.
	constexpr const u8 JNE = 0x22;
	// Move value into register.
	constexpr const u8 MOV = 0x25;
	// Call native function during runtime
	constexpr const u8 NFC = 0x27;
};