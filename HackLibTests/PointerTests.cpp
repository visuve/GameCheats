#include "HackLibTests-PCH.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(PointerTests)
{
public:
	TEST_METHOD(Arithmetic)
	{
		// constexpr size_t Last = PointerSizeBytes - 1;
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 20;

			Pointer ptr2;
			ptr2.Bytes[0] = 30;

			Pointer ptr3 = ptr2 - ptr1;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(20));
			Assert::AreEqual(ptr2.Bytes[0], uint8_t(30));
			Assert::AreEqual(ptr3.Bytes[0], uint8_t(10));
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 20;

			Pointer ptr2;
			ptr2.Bytes[0] = 30;

			Pointer ptr3 = ptr2 + ptr1;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(20));
			Assert::AreEqual(ptr2.Bytes[0], uint8_t(30));
			Assert::AreEqual(ptr3.Bytes[0], uint8_t(50));
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 10;

			Pointer ptr2;
			ptr2.Bytes[0] = 10;

			Pointer ptr3 = ptr2 - ptr1;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(10));
			Assert::AreEqual(ptr2.Bytes[0], uint8_t(10));
			Assert::IsNull(ptr3.Value);
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 10;

			ptr1 + 10;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(10));
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 10;

			ptr1 += 10;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(20));
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 10;

			ptr1 - 10;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(10));
		}
		{
			Pointer ptr1;
			ptr1.Bytes[0] = 10;

			ptr1 -= 10;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(0));
		}
		{
			Pointer ptr1;
			std::memset(ptr1.Bytes, 0xFF, PointerSizeBytes);

			ptr1 -= 0XFF;

			Assert::AreEqual(ptr1.Bytes[0], uint8_t(0x00));
		}
		{
			Pointer ptr1;
			std::memset(ptr1.Bytes, 0xFF, PointerSizeBytes);

			ptr1 += 0XFF;

			// Assert::AreEqual(reinterpret_cast<size_t>(ptr1.Value), size_t(0xFF)); // Overflow
		}

	}
};