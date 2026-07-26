/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:30 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/27 00:06:57 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: _fd(-1), _hostname(), _inputBuffer(), _outputBuffer(),
	  _nickname(), _username(), _realname(),
	  _passwordAccepted(false), _registered(false)
{
}

Client::Client(int fd, const std::string &hostname)
	: _fd(fd), _hostname(hostname), _inputBuffer(), _outputBuffer(),
	  _nickname(), _username(), _realname(),
	  _passwordAccepted(false), _registered(false)
{
}

int Client::getFd() const
{
	return (_fd);
}

const std::string &Client::getHostname() const
{
	return (_hostname);
}

const std::string &Client::getNickname() const
{
	return (_nickname);
}

const std::string &Client::getUsername() const
{
	return (_username);
}

const std::string &Client::getRealname() const
{
	return (_realname);
}

bool Client::isPasswordAccepted() const
{
	return (_passwordAccepted);
}

bool Client::isRegistered() const
{
	return (_registered);
}

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
}

void Client::setRealname(const std::string &realname)
{
	_realname = realname;
}

void Client::setPasswordAccepted(bool accepted)
{
	_passwordAccepted = accepted;
}

void Client::setRegistered(bool registered)
{
	_registered = registered;
}

std::string &Client::inputBuffer()
{
	return (_inputBuffer);
}

std::string &Client::outputBuffer()
{
	return (_outputBuffer);
}
