#include "pch.h"
#include "CppUnitTest.h"
#include "..\StringHelper.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace ResUtilTest
{
	TEST_CLASS(StringHelperTest)
	{
	public:

		TEST_METHOD(join_HelloNiceWorld)
		{
			vector<string> v{ "Hello", "nice", "world" };
			Assert::AreEqual(StringHelper::join(v).c_str(), "Hello, nice, world");
		}

		TEST_METHOD(join_empty_results_empty)
		{
			vector<string> v{  };
			Assert::IsTrue(StringHelper::join(v) == "");
		}

		TEST_METHOD(join_custom_separator)
		{
			vector<string> v{ "Hello", "nice", "world"};
			Assert::AreEqual(StringHelper::join(v, "__").c_str(), "Hello__nice__world");
		}

		TEST_METHOD(join_one_element)
		{
			vector<string> v{ "Hello" };
			Assert::AreEqual(StringHelper::join(v, "__").c_str(), "Hello");
		}
	};
}
