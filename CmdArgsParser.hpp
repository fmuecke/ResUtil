#pragma once
#include <string>
#include <vector>
#include <sstream>

class CmdArgsParser
{
public:
	enum class RequiredArg { yes, no };

	struct ArgDefinition
	{
		ArgDefinition(std::string const& id, std::string const& description, RequiredArg required)
			: id{ id }
			, description{ description }
			, required{ required } 
		{}

		ArgDefinition(std::string const& id, std::string const& description)
			: ArgDefinition(id, description, RequiredArg::yes)
		{}

		std::string id;
		std::string description;
		RequiredArg required;
	};

	struct CommandSet
	{
		std::string command;
		std::string description;
		std::vector<ArgDefinition> args;
	};

	void Add(CommandSet&& set)
	{
		_commands.emplace_back(std::move(set));
	}

	void AddAdditionalHelp(std::string&& s)
	{
		_additionalHelp.emplace_back(std::move(s));
	}

	std::string HelpText() const
	{
		std::string result = "Usage: command /param1:value ... /paramN:value\n\n";
		size_t maxLen = 0;
		for (auto const& cmd : _commands) for (auto const& def : cmd.args) maxLen = std::max<size_t>(maxLen, def.id.size());
		for (auto const& cmd : _commands)
		{
			std::stringstream ss;
			ss << Pad(cmd.command,maxLen) << "  " << cmd.description << "\n";
			//ss << "  params\n";
			for (auto const& def : cmd.args)
			{
				ss << "  /" << Pad(def.id, maxLen) << " ";
				ss << def.description;;
				if (def.required == RequiredArg::no) ss << " (optional) ";
				ss << "\n";
			}
			
			ss << "\n";
			result.append(ss.str());
		}

		for (auto const& str : _additionalHelp)
		{
			result += "\n" + str;
		}

		return result;
	}

private:
	static std::string Pad(std::string s, size_t len)
	{
		while (s.size() < len) s.push_back(' ');
		return s;
	}

	std::vector<CommandSet> _commands;
	std::vector<std::string> _additionalHelp;
};
