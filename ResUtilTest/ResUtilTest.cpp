#include "pch.h"
#include "CppUnitTest.h"
#include "..\ResTypes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ResUtilTest
{
	TEST_CLASS(ResUtilTest)
	{
	public:
		
		TEST_METHOD(ResTypeName_MapsToValidIds)
		{
			Assert::IsTrue(ResTypes::GetId(ResTypes::Strings::Bitmap) == RT_BITMAP);
		}

		TEST_METHOD(ResTypeName_InvalidMapsToZero)
		{
			Assert::IsTrue(ResTypes::GetId("asd") == 0);
		}

		TEST_METHOD(ResTypeName_UnknonwMapsToZero)
		{
			Assert::IsTrue(ResTypes::GetId(ResTypes::Strings::Unknown) == 0);
		}


		TEST_METHOD(ResTypeId_MapsToValidNames)
		{
			Assert::IsTrue(ResTypes::GetName(RT_BITMAP) == ResTypes::Strings::Bitmap);
		}

		TEST_METHOD(ResTypeId_InvalidMapsToUNKNOWN)
		{
			Assert::IsTrue(ResTypes::GetName(MAKEINTRESOURCE(12345)) == ResTypes::Strings::Unknown);
		}

	};
}
