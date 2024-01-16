#include "User.hpp"

User::User(){}

User::User(User const &cpy){(void)cpy;}

User::~User(){}

User &User::operator=(User const &rhs)
{
    if (this != &rhs)
    {
    }
    return (*this);
}

bool has_cr_lf(const std::string &input)
{
    size_t lastCrPos = input.rfind('\r');
    size_t lastNlPos = input.rfind('\n');

    if (lastCrPos == std::string::npos || lastNlPos == std::string::npos)
        return (false);
    if (lastCrPos + 1 == lastNlPos)
        return (true);
    return (false);
}

//kesken

int User::appendInput(std::string const &msg)
{
	if (_userInput.empty())
		return (0);
	while (msg.find("\r\n") != std::string::npos)
	{
		std::string cmd = msg.substr(0, msg.find("\r\n"));
		msg.erase(0, msg.find("\r\n"));

	}
	if (_userInput[0].find('\n'))
	_userInput.push_back(msg);
	return (0);
}

std::string const User::getMessage()
{
	if (_userInput.empty())
	 	return (std::string());
	std::string msg = _userInput[0];
	_userInput.erase(_userInput.begin());
	return (msg);
}

t_client &User::getUserInfo()
{
	return (_userInfo);
}