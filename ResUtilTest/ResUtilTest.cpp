#include "pch.h"
#include "CppUnitTest.h"
#include "..\ResTypes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace ResUtilTest
{
	TEST_CLASS(ResUtilTest)
	{
	public:
		
		TEST_METHOD(ResTypeName_MapsToValidIds)
		{
			wstring w;
			Assert::IsTrue(ResTypes::GetId(ResTypes::Strings::Bitmap, w) == RT_BITMAP);
		}

		TEST_METHOD(ResTypeName_Undefined_MapsToZero)
		{
			wstring w;
			Assert::IsTrue(ResTypes::GetId("asd", w) == ResTypes::UNDEFINED_TYPE);
			Assert::AreEqual(L"asd", w.c_str());
		}

		TEST_METHOD(Quoted_ResTypeName_mapsTo_undefined_and_unquotedName)
		{
			wstring w;
			Assert::AreEqual(ResTypes::UNDEFINED_TYPE, ResTypes::GetId("\"custom type\"", w));
			Assert::AreEqual(L"custom type", w.c_str());
		}

		TEST_METHOD(ResTypeId_MapsToValidNames)
		{
			Assert::IsTrue(ResTypes::GetName(RT_BITMAP) == ResTypes::Strings::Bitmap);
		}

		TEST_METHOD(Undefined_ResTypeId_MapsTo_EmptyName)
		{
			Assert::IsTrue(ResTypes::GetName(MAKEINTRESOURCE(12345)).empty());
		}

	};
}
