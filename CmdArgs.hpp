#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <map>

template<typename T>
class CmdArgs
{
public:
	using StrType = std::basic_string<T>;

	CmdArgs(int argc, T** argv) : _args(argv, argv + argc)
	{}

	StrType TakeArg(StrType const& tag)
	{
		auto pos = std::find_if(std::cbegin(_args), std::cend(_args), [&](StrType const& arg)
		{
			return std::equal(std::cbegin(tag), std::cend(tag), std::cbegin(arg));
		});

		if (pos == std::cend(_args)) return T();
		auto arg = (*pos).substr(tag.size());
		_args.erase(pos);

		return arg;
	}

private:
	std::vector<StrType> _args;
};
