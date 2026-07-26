/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:27 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/27 00:07:32 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Server::Server(const std::string &port, const std::string &password)
	: _port(parsePort(port)), _password(password), _serverFd(-1),
	  _running(false), _pollFds(), _clients()
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
	std::string command;
	size_t position = line.find(' ');

	if (position == std::string::npos)
		command = line;
	else
		command = line.substr(0, position);
	for (size_t i = 0; i < command.size(); ++i)
		command[i] = static_cast<char>(std::toupper(
			static_cast<unsigned char>(command[i])));
	std::cout << "[client " << client.getFd() << "] " << line << std::endl;
	if (command == "PING")
	{
		if (position == std::string::npos || position + 1 >= line.size())
			queueMessage(client.getFd(), ":irc.local 409 * :No origin specified");
		else
			queueMessage(client.getFd(), "PONG " + line.substr(position + 1));
	}
	else if (command == "QUIT")
		return (false);
	/* Replace this function body with Parser/Command dispatch later. */
	return (true);
}
