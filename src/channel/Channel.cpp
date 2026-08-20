/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:27 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/20 03:13:00 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name)
    : _name(name), _clients(), _operators(), _topic(), _invited(),
      _inviteOnly(false), _topicRestricted(false), _hasKey(false),
      _key(), _userLimit(-1)
{
}

Channel::Channel(const Channel &other)
    : _name(other._name), _clients(other._clients),
      _operators(other._operators), _topic(other._topic),
      _invited(other._invited), _inviteOnly(other._inviteOnly),
      _topicRestricted(other._topicRestricted), _hasKey(other._hasKey),
      _key(other._key), _userLimit(other._userLimit)
{
}

Channel &Channel::operator=(const Channel &other)
{
    if (this != &other)
    {
        _name = other._name;
        _clients = other._clients;
        _operators = other._operators;
        _topic = other._topic;
        _invited = other._invited;
        _inviteOnly = other._inviteOnly;
        _topicRestricted = other._topicRestricted;
        _hasKey = other._hasKey;
        _key = other._key;
        _userLimit = other._userLimit;
    }
    return (*this);
}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
    return (_name);
}

const std::string &Channel::getTopic() const
{
    return (_topic);
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

size_t Channel::getClientCount() const
{
    return (_clients.size());
}

bool Channel::isInviteOnly() const
{
    return (_inviteOnly);
}

void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

bool Channel::isTopicRestricted() const
{
    return (_topicRestricted);
}

void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

bool Channel::hasKey() const
{
    return (_hasKey);
}

const std::string &Channel::getKey() const
{
    return (_key);
}

void Channel::setKey(const std::string &key)
{
    _key = key;
    _hasKey = true;
}

void Channel::removeKey()
{
    _key.clear();
    _hasKey = false;
}

int Channel::getUserLimit() const
{
    return (_userLimit);
}

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
}

void Channel::removeUserLimit()
{
    _userLimit = -1;
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
    removeOp(client);
    removeInvite(client);
    for (std::vector<Client *>::iterator it = _clients.begin();
        it != _clients.end(); ++it)
    {
        if (*it == client)
        {
            _clients.erase(it);
            return ;
        }
    }
}

void Channel::removeOp(Client *client)
{
    for (std::vector<Client *>::iterator it = _operators.begin();
        it != _operators.end(); ++it)
    {
        if (*it == client)
        {
            _operators.erase(it);
            return ;
        }
    }
}

bool Channel::isInvited(Client *client) const
{
    for (size_t i = 0; i < _invited.size(); ++i)
    {
        if (_invited[i] == client)
            return (true);
    }
    return (false);
}

void Channel::addInvite(Client *client)
{
    if (client == NULL || isInvited(client))
        return ;
    _invited.push_back(client);
}

void Channel::removeInvite(Client *client)
{
    for (std::vector<Client *>::iterator it = _invited.begin();
        it != _invited.end(); ++it)
    {
        if (*it == client)
        {
            _invited.erase(it);
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
