/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:33 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/23 23:43:31 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <stdexcept>

volatile sig_atomic_t Server::_stopSignal = 0;

void Server::handleSignal(int sig)
{
    if (sig == SIGINT)
        _stopSignal = 1;
}

int Server::parsePort(const std::string &port) const
{
	char *end;
	long value;

	if (port.empty())
		throw (std::runtime_error("Port is empty"));
	errno = 0;
	value = std::strtol(port.c_str(), &end, 10);
	if (errno != 0 || *end != '\0' || value < 1 || value > 65535)
		throw (std::runtime_error("Port must be between 1 and 65535"));
	return (static_cast<int>(value));
}

void Server::init()
{
    if (_serverFd != -1)
        return ;

    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, Server::handleSignal);

    createListeningSocket();
    addPollFd(_serverFd, POLLIN);
}

void Server::stop()
{
	_running = false;
}
