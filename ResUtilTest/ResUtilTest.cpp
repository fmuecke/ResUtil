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
			Assert::IsTrue(ResTypes::GetValue(ResTypes::Strings::Bitmap, w) == RT_BITMAP);
		}

		TEST_METHOD(Quoted_ResTypeName_mapsTo_undefined_and_wide_string)
		{
			wstring w;
			Assert::AreEqual(ResTypes::UNDEFINED_TYPE, ResTypes::GetValue("custom type", w));
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

		TEST_METHOD(ParseResIdString_number_to_resId)
		{
			wstring w;
			auto result = ResTypes::ParseResIdString("123", w);
			Assert::IsTrue(MAKEINTRESOURCEW(123) == result);
			Assert::IsTrue(w.empty());
		}

		TEST_METHOD(ParseResIdString_string_to_resId)
		{
			wstring w;
			auto result = ResTypes::ParseResIdString("asd asd", w);
			Assert::AreEqual(L"asd asd", result);
			Assert::AreEqual(L"asd asd", w.c_str());
		}

		TEST_METHOD(GetValue_number_to_resTypeId)
		{
			wstring w;
			auto result = ResTypes::GetValue(ResTypes::Strings::Bitmap, w);
			Assert::IsTrue(RT_BITMAP == result);
			Assert::IsTrue(w.empty());
		}

		TEST_METHOD(GetValue_string_to_resTypeId)
		{
			wstring w;
			auto result = ResTypes::GetValue("asd asd", w);
			Assert::IsTrue(ResTypes::UNDEFINED_TYPE == result);
			Assert::AreEqual(L"asd asd", w.c_str());
		}

		TEST_METHOD(GetName_number_to_resTypeName)
		{
			auto result = ResTypes::GetName(RT_BITMAP);
			Assert::AreEqual(ResTypes::Strings::Bitmap, result.c_str());
		}

		TEST_METHOD(GetName_string_to_resTypeName)
		{
			wstring w{ L"Test Test" };
			auto result = ResTypes::GetName(w.c_str());
			Assert::AreEqual("Test Test", result.c_str());
		}

	};
}
