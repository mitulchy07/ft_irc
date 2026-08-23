/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:36 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/23 23:44:04 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

void Server::run()
{
    int ready;

    if (_serverFd == -1)
        throw (std::runtime_error("Server is not initialized"));
    _running = true;
    std::cout << "IRC server listening on port " << _port << std::endl;
    while (_running && !_stopSignal)
    {
        ready = poll(&_pollFds[0], _pollFds.size(), -1);
        if (ready == -1)
        {
            if (errno == EINTR)
                continue ;
            throw (std::runtime_error(std::string("poll: ")
                    + std::strerror(errno)));
        }
        for (size_t i = 0; i < _pollFds.size() && ready > 0; )
        {
            short events = _pollFds[i].revents;
            int fd = _pollFds[i].fd;
            bool keepClient = true;

            if (events == 0)
            {
                ++i;
                continue ;
            }
            --ready;
            if (fd == _serverFd)
            {
                if (events & (POLLERR | POLLHUP | POLLNVAL))
                    throw (std::runtime_error("Listening socket poll error"));
                if (events & POLLIN)
                    acceptClients();
                ++i;
                continue ;
            }
            if (events & (POLLERR | POLLHUP | POLLNVAL))
                keepClient = false;
            if (keepClient && (events & POLLIN))
                keepClient = receiveFromClient(fd);
            if (keepClient && (events & POLLOUT))
                keepClient = flushClientOutput(fd);
            if (!keepClient)
                disconnectClient(fd, "connection closed");
            else
                ++i;
        }
    }
}