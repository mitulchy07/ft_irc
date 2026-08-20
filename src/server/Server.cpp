/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:27 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/18 21:17:02 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Parser.hpp"
#include "Command.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Server::Server(const std::string &port, const std::string &password)
	: _port(parsePort(port)), _password(password), _serverFd(-1),
	  _running(false), _pollFds(), _clients(), _channels()
{
	if (_password.empty())
		throw (std::runtime_error("Password cannot be empty"));
}

Server::~Server()
{
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
		it != _pollFds.end(); ++it)
	{
		if (it->fd >= 0)
			close(it->fd);
	}
	_pollFds.clear();
	_clients.clear();
	_serverFd = -1;
}

int Server::getPort() const
{
	return (_port);
}

const std::string &Server::getPassword() const
{
	return (_password);
}

Client *Server::getClient(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);

	if (it == _clients.end())
		return (NULL);
	return (&it->second);
}

const Client *Server::getClient(int fd) const
{
	std::map<int, Client>::const_iterator it = _clients.find(fd);

	if (it == _clients.end())
		return (NULL);
	return (&it->second);
}

std::map<int, Client> &Server::getClients()
{
	return (_clients);
}

void Server::queueMessage(int fd, const std::string &message)
{
	Client *client = getClient(fd);

	if (client == NULL)
		return ;
	client->outputBuffer() += message;
	if (message.size() < 2
		|| message.substr(message.size() - 2) != "\r\n")
		client->outputBuffer() += "\r\n";
	setPollEvents(fd, POLLIN | POLLOUT);
}

bool Server::processLine(Client &client, const std::string &line)
{
	IrcMsg	msg;
// 	std::cout << "[client " << client.getFd() << "] " << line << std::endl;
	msg = Parser::parse(line);
	return (Command::execute(*this, client, msg));
}

Channel *Server::createChannel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return(&it->second);
	std::pair<std::map<std::string, Channel>::iterator, bool> result = _channels.insert(std::make_pair(name, Channel(name)));
	return (&result.first->second);
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it == _channels.end())
		return NULL;
	return(&it->second);
}

