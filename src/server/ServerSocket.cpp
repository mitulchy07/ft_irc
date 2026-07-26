/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:39 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/27 00:17:36 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

void Server::setNonBlocking(int fd) const
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error(std::string("fcntl: ") + std::strerror(errno)));
}

void Server::createListeningSocket()
{
	struct sockaddr_in address;
	int option = 1;

	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		throw (std::runtime_error(std::string("socket: ") + std::strerror(errno)));
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR,
			&option, sizeof(option)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw (std::runtime_error(std::string("setsockopt: ")
				+ std::strerror(errno)));
	}
	try
	{
		setNonBlocking(_serverFd);
	}
	catch (...)
	{
		close(_serverFd);
		_serverFd = -1;
		throw ;
	}
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(static_cast<unsigned short>(_port));
	if (bind(_serverFd, reinterpret_cast<struct sockaddr *>(&address),
			sizeof(address)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw (std::runtime_error(std::string("bind: ") + std::strerror(errno)));
	}
	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw (std::runtime_error(std::string("listen: ") + std::strerror(errno)));
	}
}

void Server::addPollFd(int fd, short events)
{
	struct pollfd item;

	item.fd = fd;
	item.events = events;
	item.revents = 0;
	_pollFds.push_back(item);
}

void Server::removePollFd(int fd)
{
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
		it != _pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollFds.erase(it);
			return ;
		}
	}
}

void Server::setPollEvents(int fd, short events)
{
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
		it != _pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			it->events = events;
			return ;
		}
	}
}
