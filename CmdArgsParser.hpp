#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>

#ifndef RANGE
#define RANGE(c)	cbegin(c), cend(c)
#endif

class CmdArgsParser
{
public:
	
	struct ParseException : public std::exception
	{
		//explicit ParseException(const char* msg) : _msg{ msg } {}
		explicit ParseException(std::string&& msg) : _msg{ std::move(msg) } {}
		virtual const char* what() const override { return _msg.c_str(); }

	private:
		std::string _msg;
	};

	struct InvalidCommandArgsException : public ParseException
	{
		explicit InvalidCommandArgsException(std::string command, std::string&& msg)
			: ParseException(std::move(msg))
			, _cmd{ std::move(command) }
		{}
		const char* Command() const { return _cmd.c_str(); }

	private:
		std::string _cmd;
	};
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

	struct ParsedArgs
	{
		std::string command;
		std::map<std::string, std::string> args;
	};

	explicit CmdArgsParser(const char* title) : _programTitle{ title } {}

	void Add(CommandSet&& set) noexcept
	{
		_commands.emplace_back(std::move(set));
	}

	void AddAdditionalHelp(std::string&& s) noexcept
	{
		_additionalHelp.emplace_back(std::move(s));
	}

	void Parse(int argc, char** argv)
	{
		auto args = std::vector<std::string>(argv, argv + argc);

		// find command in args
		auto pos = std::find_first_of(RANGE(args), RANGE(_commands), [](std::string const& s, CommandSet const& c) { return c.command == s; });
		if (pos == cend(args))
		{
			throw ParseException("\nerror: no command specified\n");
		}
		_parsedArgs.command = *pos;
		args.erase(pos);
		
		auto const& cmdSet = *find_if(RANGE(_commands), [&](CommandSet const& c) { return c.command == _parsedArgs.command; });
		for (auto const& cmdArg : cmdSet.args)
		{
			auto&& value = TakeArg(args, cmdArg.id);
			if (value.empty())
			{
				throw InvalidCommandArgsException(_parsedArgs.command, std::string("\nerror: argument '") + cmdArg.id + "' is invalid\n");
			}
			_parsedArgs.args.emplace(cmdArg.id, std::move(value));
		}
	}

	std::string HelpText() const noexcept
	{
		return HelpText(std::string());
	}

	std::string HelpText(std::string&& command) const noexcept
	{
		std::string result = _programTitle + "\n\nUsage: command /param1:value ... /paramN:value\n\n";
		size_t maxLen = 0;
		for (auto const& cmd : _commands) for (auto const& def : cmd.args) maxLen = std::max<size_t>(maxLen, def.id.size());
		for (auto const& cmd : _commands)
		{
			if (!command.empty() && command != cmd.command)
			{
				continue;
			}

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

	std::string GetCommand() const noexcept
	{
		return _parsedArgs.command;
	}

	std::string GetValue(std::string const& s) const noexcept
	{ 
		auto pos =_parsedArgs.args.find(s);
		return pos != cend(_parsedArgs.args) ? pos->second : std::string();
	}

private:
	static std::string TakeArg(std::vector<std::string>& args, std::string const& tag) noexcept
	{
		auto fullTag = "/" + tag + ":";
		auto pos = std::find_if(RANGE(args), [&](std::string const& arg)
		{
			return std::equal(RANGE(fullTag), cbegin(arg));
		});

		if (pos == cend(args)) return std::string();
		auto arg = (*pos).substr(fullTag.size());
		args.erase(pos);
		return arg;
	}

	static std::string Pad(std::string s, size_t len)
	{
		while (s.size() < len) s.push_back(' ');
		return s;
	}

	std::string _programTitle;
	std::vector<CommandSet> _commands;
	std::vector<std::string> _additionalHelp;

	ParsedArgs _parsedArgs;
};
