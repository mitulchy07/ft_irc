/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:27 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/17 21:49:18 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) : _name(name), _clients(), _operators()
{
}

Channel::Channel(const Channel &other)
	: _name(other._name), _clients(other._clients),
	  _operators(other._operators)
{
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_clients = other._clients;
		_operators = other._operators;
	}
	return (*this);
}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
	return (_name);
}

size_t Channel::getClientCount() const
{
	return (_clients.size());
}

bool Channel::hasClient(Client *client) const
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i] == client)
			return (true);
	}
	return (false);
}

bool Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < _operators.size(); ++i)
	{
		if (_operators[i] == client)
			return (true);
	}
	return (false);
}

void Channel::addClient(Client *client)
{
	if (client == NULL || hasClient(client))
		return ;
	_clients.push_back(client);
}

void Channel::addOperator(Client *client)
{
	if (client == NULL || isOperator(client))
		return ;
	_operators.push_back(client);
}

void Channel::removeClient(Client *client)
{
	for (std::vector< Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == Client)
		{
			_clients.erase(it);
			return ;
		}
		
	}
}

void Channel::removeOp(Client *client)
{
	for (std::vector< Client *>::iterator it = _operators.begin(); it != _operators.end(): ++it)
	{
		if (*it == client)
		{
			_operators.erase(it);
			return ;
		}
	}
}


void Channel::broadcast(Server &server, Client *sender, const std::string &msg)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i] != sender)
			server.queueMessage(_clients[i]->getFd(), msg);
	}
}
