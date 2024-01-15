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

int User::newMessage(std::string const &msg)
{
	_messages.push_back(msg);
	return (0);
}

std::string const User::getMessage()
{
	if (_messages.empty())
	 	return (std::string());
	std::string msg = _messages[0];
	_messages.erase(_messages.begin());
	return (msg);
}

t_client &User::getUserInfo()
{
	return (_userInfo);
}