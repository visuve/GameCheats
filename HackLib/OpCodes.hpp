#pragma once

#include <array>

// http://sparksandflames.com/files/x86InstructionChart.html
namespace X86
{
	enum OpCode : uint8_t
	{
		AddEbGb = 0x00,
		AddEvGv = 0x01,
		AddGbEb = 0x02,
		AddGvEv = 0x03,
		AddAlIb = 0x04,
		AddEaxIv = 0x05,

		PushEs = 0x06,
		PopEs = 0x07,

		OrEbGb = 0x8,
		OrEvGv = 0x9,
		OrGbEb = 0xA,
		OrGvEv = 0xB,
		OrAlIb = 0xC,
		OrEaxIv = 0xD,

		PushCs = 0xE,

		AdcEbGb = 0x10,
		AdcEvGv = 0x11,
		AdcGbEb = 0x12,
		AdcGvEv = 0x13,
		AdcAlIb = 0x14,
		AdcEaxIv = 0x15,

		PushSs = 0x16,
		PopSs = 0x17,

		SbbEbGb = 0x18,
		SbbEvGv = 0x19,
		SbbGbEb = 0x1A,
		SbbGvEv = 0x1B,
		SbbAlIb = 0x1C,
		SbbEaxIv = 0x1D,

		PushDs = 0x1E,
		PopDs = 0x1F,

		AndEbGb = 0x20,
		AndEvGv = 0x21,
		AndGbEb = 0x22,
		AndGvEv = 0x23,
		AndAlIb = 0x24,
		AndEaxIv = 0x25,

		Es = 0x26,
		Daa = 0x27,

		SubEbGb = 0x28,
		SubEvGv = 0x29,
		SubGbEb = 0x2A,
		SubGvEv = 0x2B,
		SubAlIb = 0x2C,
		SubrExIv = 0x2D,

		Cs = 0x2E,
		Das = 0x2F,

		XorEbGb = 0x30,
		XorEvGv = 0x31,
		XorGbEb = 0x32,
		XorGbEv = 0x33,
		XorAlIb = 0x34,
		XorEaxIv = 0x35,

		Ss = 0x36,
		Aaa = 0x37,

		CmpEbGb = 0x38,
		CmpEvGv = 0x39,
		CmpGbEb = 0x3A,
		CmpGvEv = 0x3B,
		CmpAlIb = 0x3C,
		CmpEaxIv = 0x3D,

		Ds = 0x3E,
		Aas = 0x3F,

		IncEax = 0x40,
		IncEcx = 0x41,
		IncEdx = 0x42,
		IncEbx = 0x43,
		IncEsp = 0x44,
		IncEbp = 0x45,
		IncEsi = 0x46,
		IncEdi = 0x47,
		
		DecEax = 0x48,
		DecEcx = 0x49,
		DecEdx = 0x4A,
		DecEbx = 0x4B,
		DecEsp = 0x4C,
		DecEbp = 0x4D,
		DecEsi = 0x4E,
		DecEdi = 0x4F,

		PushEax = 0x50,
		PushEcx = 0x51,
		PushEdx = 0x52,
		PushEbx = 0x53,
		PushEsp = 0x54,
		PushEbp = 0x55,
		PushEsi = 0x56,
		PushEdi = 0x57,

		PopEax = 0x58,
		PopEcx = 0x59,
		PopEdx = 0x5A,
		PopEbx = 0x5B,
		PopEsp = 0x5C,
		PopEbp = 0x5D,
		PopEsi = 0x5E,
		PopEdi = 0x5F,

		PushA = 0x60,
		PopA = 0x61,

		BoundGvMa = 0x62,
		ArplEwGw = 0x63,

		Fs = 0x64,
		Gs = 0x64,

		OpSize = 0x66,
		AdSize = 0x67,

		PushIv = 0x68,
		ImulGvEvIv = 0x69,
		
		PushIb = 0x6A,

		ImulGvEvIb = 0x6B,

		InsbYxDx = 0x6C,
		InsbYzDx = 0x6D,

		OutsbDxXb = 0x6E,
		OutswDxXv = 0x6F,

		JoJb = 0x70,
		JnoJb = 0x71,
		JbJb = 0x72,
		JnbJb = 0x73,
		JzJb = 0x74,
		JnzJb = 0x75,
		JbeJb = 0x76,
		JaJb = 0x77,
		JsJb = 0x78,
		JnsJb = 0x79,
		JpJb = 0x7A,
		JnpJb = 0x7B,
		JlJb = 0x7C,
		JnlJb = 0x7D,
		JleJb = 0x7E,
		JnleJb = 0x7F,

		AddEbIb = 0x80,
		AddEvIv = 0x81,

		SubEbIb = 0x82,
		SubEvIb = 0x83,

		TestEbGb = 0x84,
		TestEvGv = 0x85,

		XchgEbGb = 0x86,
		XchgEvGv = 0x87,
		
		MovEbGb = 0x88,
		MovEvGv = 0x89,
		MovGbEb = 0x8A,
		MovGvEv = 0x8B,
		MovEwSw = 0x8C,
		LeaGvM = 0x8D,
		MovSwEw = 0x8E,
		
		PopEv = 0x8F,

		Nop = 0x90,

		XchgEaxEcx = 0x91,
		XchgEaxEdx = 0x92,
		XchgEaxEbx = 0x93,
		XchgEaxEsp = 0x94,
		XchgEaxEbp = 0x95,
		XchgEaxEsi = 0x96,
		XchgEaxEdi = 0x97,

		JmpJz = 0xE9,
		JmpAp = 0xEA,
		JmpJb = 0xEB
	};

#ifndef _WIN64
	constexpr size_t JumpOpSize = 5;

	inline std::array<uint8_t, JumpOpSize> JumpRelative(Pointer distance)
	{
		return
		{
			0xE9,
			distance[0],
			distance[1],
			distance[2],
			distance[3]
		};
	}
#endif

}

namespace X64
{
#ifdef _WIN64
	constexpr size_t JumpOpSize = 14;

	inline std::array<uint8_t, JumpOpSize> JumpAbsolute(Pointer ptr)
	{
		return
		{
			0xFF,
			0x25,
			0x00,
			0x00,
			0x00,
			0x00,
			ptr[0],
			ptr[1],
			ptr[2],
			ptr[3],
			ptr[4],
			ptr[5],
			ptr[6],
			ptr[7]
		};
	}
#endif
}