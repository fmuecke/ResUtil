#pragma once

template <typename T>
struct ResultBase
{
	ResultBase()
    {}
	explicit ResultBase(std::string&& err) : _msg{ std::move(err) }
	{}

	operator bool() const noexcept { return _msg.empty(); }
	auto Message() const noexcept { return _msg.c_str(); }
	
	static T ErrorInvalidArgs() noexcept { return T("Invalid argument"); }
	static T ErrorArgumentNull() noexcept { return T("Argument null"); }
	static T ErrorInvalidData() noexcept { return T("Invalid data"); }
	static T ErrorInvalidFile(const char* fileName) noexcept { return T(std::move(std::string("The file is invalid: ").append(fileName))); }
	static T ErrorInvalidType(const char* type) noexcept { return T(std::move(std::string("The type is invalid: ").append(type))); }

private:
	std::string _msg{};
};

template<typename T>
struct Result : public ResultBase<Result<T>>
{
    Result()
    {}
    explicit Result(std::string&& err) : ResultBase{ std::move(err) }
    {}
    explicit Result(T&& data) : _data{std::move(data)}
	{}

	auto Get() noexcept { return _data; }
	auto Get() const noexcept { return _data; }	

private:
	T _data;
};

template<>
struct Result<void> : public ResultBase<Result<void>>
{
    Result()
    {}
    explicit Result(std::string&& err) : ResultBase{std::move(err) }
    {}
};
