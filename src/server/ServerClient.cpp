/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:30 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/18 20:07:57 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

void Server::acceptClients()
{
    struct sockaddr_in address;
    socklen_t addressLength;
    int clientFd;

    addressLength = sizeof(address);
    clientFd = accept(_serverFd,
        reinterpret_cast<struct sockaddr *>(&address), &addressLength);
    if (clientFd == -1)
    {
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            std::cerr << "accept: " << std::strerror(errno) << std::endl;
        return ;
    }
    try
    {
        setNonBlocking(clientFd);
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << std::endl;
        close(clientFd);
        return ;
    }
    std::string host = inet_ntoa(address.sin_addr);
    _clients.insert(std::make_pair(clientFd, Client(clientFd, host)));
    addPollFd(clientFd, POLLIN);
    std::cout << "Client connected: fd=" << clientFd
        << " host=" << host << std::endl;
}

bool Server::processClientBuffer(Client &client)
{
    std::string &buffer = client.inputBuffer();
    size_t newline;

    while ((newline = buffer.find('\n')) != std::string::npos)
    {
        std::string line = buffer.substr(0, newline);
        buffer.erase(0, newline + 1);
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.size() > 510)
        {
            queueMessage(client.getFd(), "ERROR :Input line is too long");
            continue ;
        }
        if (!line.empty() && !processLine(client, line))
            return (false);
    }
    if (buffer.size() > 512)
        return (false);
    return (true);
}

bool Server::receiveFromClient(int fd)
{
    Client *client = getClient(fd);
    char buffer[4096];
    ssize_t bytes;

    if (client == NULL)
        return (false);
    bytes = recv(fd, buffer, sizeof(buffer), 0);
    if (bytes > 0)
    {
        client->inputBuffer().append(buffer, static_cast<size_t>(bytes));
        if (client->inputBuffer().size() > 65536)
            return (false);
        return (processClientBuffer(*client));
    }
    if (bytes == 0)
        return (false);
    if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
        return (true);
    return (false);
}

bool Server::flushClientOutput(int fd)
{
    Client *client = getClient(fd);
    ssize_t bytes;

    if (client == NULL)
        return (false);
    std::string &output = client->outputBuffer();
    if (output.empty())
    {
        setPollEvents(fd, POLLIN);
        return (true);
    }
    bytes = send(fd, output.c_str(), output.size(), 0);
    if (bytes > 0)
    {
        output.erase(0, static_cast<size_t>(bytes));
        if (output.empty())
            setPollEvents(fd, POLLIN);
        return (true);
    }
    if (bytes == -1
        && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK))
        return (true);
    return (false);
}

void Server::disconnectClient(int fd, const std::string &reason)
{
    std::map<int, Client>::iterator clientIt = _clients.find(fd);
    std::map<std::string, Channel>::iterator channelIt;
    Client *client;

    if (clientIt == _clients.end())
        return ;
    client = &clientIt->second;
    channelIt = _channels.begin();
    while (channelIt != _channels.end())
    {
        channelIt->second.removeClient(client);
        if (channelIt->second.getClientCount() == 0)
        {
            std::map<std::string, Channel>::iterator toErase = channelIt;
            ++channelIt;
            _channels.erase(toErase);
        }
        else
            ++channelIt;
    }
    std::cout << "Client disconnected: fd=" << fd;
    if (!reason.empty())
        std::cout << " reason=" << reason;
    std::cout << std::endl;
    removePollFd(fd);
    close(fd);
    _clients.erase(clientIt);
}