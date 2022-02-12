#pragma once

// https://github.com/torvalds/linux/blob/master/arch/x86/lib/x86-opcode-map.txt
namespace X86
{
	enum OpCode : uint8_t
	{
		AddEbGb = 0x00,
		AddEvGv = 0x01,
		AddGbEb = 0x02,
		AddGvEv = 0x03,
		AddALIb = 0x04,
		AddrAXIz = 0x05,

		SubEbGb = 0x28,
		SubEvGv = 0x29,
		SubGbEb = 0x2A,
		SubGvEv = 0x2B,
		SubALIb = 0x2C,
		SubrAXIz = 0x2D
	};
}
