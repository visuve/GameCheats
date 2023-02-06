#include "Process.hpp"

TEST(ProcessTests, Header)
{
	DWORD pid = GetCurrentProcessId();
	Process current(pid);

	EXPECT_EQ(current.NtHeader().OptionalHeader.Subsystem, WORD(IMAGE_SUBSYSTEM_WINDOWS_CUI));
}

TEST(ProcessTests, ModuleNotFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	EXPECT_THROW(currentProcess.FindImportAddress(
		"This module does not exists",
		"Neither does this function"), std::range_error);
}

TEST(ProcessTests, FunctionNotFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	EXPECT_THROW(currentProcess.FindImportAddress(
		"KERNEL32.dll",
		"Neither does this function"), std::range_error);
}

TEST(ProcessTests, FunctionFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	// It's just called above. It has to be found
	Pointer fnptr = 
		currentProcess.FindImportAddress("KERNEL32.dll", "GetCurrentProcessId");

	EXPECT_NE(fnptr, Pointer());
}

TEST(ProcessTests, ResolveSecret)
{
	struct Node
	{
		Node* Head;
		Node* Tail;
		size_t Secrect;
	};

	// A simple linked list
	auto root = std::make_unique<Node>();
	auto a = std::make_unique<Node>();
	auto b = std::make_unique<Node>();
	auto c = std::make_unique<Node>();

	root->Head = nullptr;
	root->Tail = a.get();
	root->Secrect = 0x1111111;

	a->Head = nullptr;
	a->Tail = b.get();
	a->Secrect = 0xAAAAAAAA;

	b->Head = a.get();
	b->Tail = c.get();
	b->Secrect = 0xBBBBBBBB;

	c->Head = b.get();
	c->Tail = nullptr;
	c->Secrect = 0xCAFEC0DE;

	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	Pointer startingAddress(reinterpret_cast<uint8_t*>(root.get()));

	Pointer resultPointer = currentProcess.ResolvePointer(
		++startingAddress, // root->tail
		Pointer::Size, // a->tail
		Pointer::Size, // b->tail
		Pointer::Size + sizeof(size_t)); // c->secret

	size_t result = currentProcess.Read<size_t>(resultPointer);

	EXPECT_EQ(result, size_t(0xCAFEC0DE));
}

TEST(ProcessTests, Timeout)
{
	DWORD pid = GetCurrentProcessId();
	Process current(pid);

	EXPECT_EQ(current.WairForExit(0ms), DWORD(WAIT_TIMEOUT));
	EXPECT_EQ(current.WairForExit(1ms), DWORD(WAIT_TIMEOUT));
	EXPECT_EQ(current.WairForExit(2ms), DWORD(WAIT_TIMEOUT));
}

#ifndef _WIN64
TEST(ProcessTests, Jump)
{
	{
		auto jump = Process::JumpOp(0x009A0000u, 0x009A0201u);

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFCu);
		EXPECT_EQ(jump[2], 0x01u);
		EXPECT_EQ(jump[3], 0x00u);
		EXPECT_EQ(jump[4], 0x00u);
	}
	{
		auto jump = Process::JumpOp(0x009A0201u, 0x009A0000u);

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFAu);
		EXPECT_EQ(jump[2], 0xFDu);
		EXPECT_EQ(jump[3], 0xFFu);
		EXPECT_EQ(jump[4], 0xFFu);
	}
}
#endif