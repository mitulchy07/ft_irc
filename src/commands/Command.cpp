/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 01:40:00 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/24 00:47:51 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <sstream>
#include <cstdlib>

Command::Command() {}

Command::Command(const Command &other)
{
    (void)other;
}

Command &Command::operator=(const Command &other)
{
    (void)other;
    return (*this);
}

bool Command::execute(Server &server, Client &client, const IrcMsg &msg)
{
    if (msg.command.empty())
        return (true);
    if (msg.command == "PING")
        return (Command::handlePing(server, client, msg));
    if (msg.command == "QUIT")
        return (Command::handleQuit(server, client, msg));
    if (msg.command == "TOPIC")
        return (Command::handleTopic(server, client, msg));
    if (msg.command == "PASS")
        return (Command::handlePass(server, client, msg));
    if (msg.command == "NICK")
        return (Command::handleNick(server, client, msg));
    if (msg.command == "USER")
        return (Command::handleUser(server, client, msg));
    if (msg.command == "KICK")
        return (Command::handleKick(server, client, msg));
    if (msg.command == "JOIN")
        return (Command::handleJoin(server, client, msg));
    if (msg.command == "INVITE")
        return (Command::handleInvite(server, client, msg));
    if (msg.command == "PRIVMSG")
        return (Command::handlePrivmsg(server, client, msg));
    if (msg.command == "MODE")
        return (Command::handleMode(server, client, msg));
    Command::sendUnknownCommand(server, client, msg);
    return (true);
}

static std::string intTostr(int n)
{
    std::stringstream ss;

    ss << n;
    return (ss.str());
}

static bool isPositive(const std::string &s)
{
    if (s.empty())
        return (false);
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] < '0' || s[i] > '9')
            return (false);
    }
    return (std::atoi(s.c_str()) > 0);
}

static std::string getModes(Channel *channel)
{
    std::string mode;
    std::string arg;

    mode = "+";
    if (channel->isInviteOnly())
        mode += "i";
    if (channel->isTopicRestricted())
        mode += "t";
    if (channel->hasKey())
    {
        mode += "k";
        arg += " " + channel->getKey();
    }
    if (channel->getUserLimit() != -1)
    {
        mode += "l";
        arg += " " + intTostr(channel->getUserLimit());
    }
    return (mode + arg);
}

bool Command::handleMode(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string mode;
    std::string channelName;
    std::string prefix;
    Channel     *channel;
    bool        adding;
    size_t      paramIndex;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 " + user + " :You have not registered");
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " MODE :Not enough parameters");
        return (true);
    }
    channelName = msg.params[0];
    channel = server.getChannel(channelName);
    if (channel == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + channelName + " :No such channel");
        return (true);
    }
    if (msg.params.size() == 1)
    {
        server.queueMessage(client.getFd(), ":ircserv 324 " + user + " " + channelName + " " + getModes(channel));
        return (true);
    }
    if (!channel->hasClient(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 442 " + user + " " + channelName + " :You're not on that channel");
        return (true);
    }
    if (!channel->isOperator(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 482 " + user + " " + channelName + " :You're not channel operator");
        return (true);
    }

    mode = msg.params[1];
    adding = true;
    paramIndex = 2;
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    for (size_t i = 0; i < mode.size(); ++i)
    {
        if (mode[i] == '+')
        {
            adding = true;
            continue ;
        }
        if (mode[i] == '-')
        {
            adding = false;
            continue ;
        }
        if (mode[i] == 'i')
        {
            channel->setInviteOnly(adding);
            if (adding)
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " +i");
            else
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " -i");
        }
        else if (mode[i] == 't')
        {
            channel->setTopicRestricted(adding);
            if (adding)
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " +t");
            else
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " -t");
        }
        else if (mode[i] == 'k')
        {
            if (adding)
            {
                if (paramIndex >= msg.params.size())
                {
                    server.queueMessage(client.getFd(), ":ircserv 461 " + user + " MODE :Not enough parameters");
                    return (true);
                }
                channel->setKey(msg.params[paramIndex]);
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " +k " + msg.params[paramIndex]);
                ++paramIndex;
            }
            else
            {
                channel->removeKey();
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " -k");
            }
        }
        else if (mode[i] == 'l')
        {
            if (adding)
            {
                if (paramIndex >= msg.params.size() || !isPositive(msg.params[paramIndex]))
                {
                    server.queueMessage(client.getFd(), ":ircserv 461 " + user + " MODE :Not enough parameters");
                    return (true);
                }
                channel->setUserLimit(std::atoi(msg.params[paramIndex].c_str()));
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " +l " + msg.params[paramIndex]);
                ++paramIndex;
            }
            else
            {
                channel->removeUserLimit();
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " -l");
            }
        }
        else if (mode[i] == 'o')
        {
            Client *target;

            if (paramIndex >= msg.params.size())
            {
                server.queueMessage(client.getFd(), ":ircserv 461 " + user + " MODE :Not enough parameters");
                return (true);
            }
            target = Command::findClientByNickname(server, msg.params[paramIndex]);
            if (target == NULL)
            {
                server.queueMessage(client.getFd(), ":ircserv 401 " + user + " " + msg.params[paramIndex] + " :No such nick/channel");
                return (true);
            }
            if (!channel->hasClient(target))
            {
                server.queueMessage(client.getFd(), ":ircserv 441 " + user + " " + msg.params[paramIndex] + " " + channelName + " :They aren't on that channel");
                return (true);
            }
            if (adding)
            {
                channel->addOperator(target);
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " +o " + msg.params[paramIndex]);
            }
            else
            {
                channel->removeOp(target);
                channel->broadcast(server, NULL, prefix + " MODE " + channelName + " -o " + msg.params[paramIndex]);
            }
            ++paramIndex;
        }
        else
        {
            server.queueMessage(client.getFd(), ":ircserv 472 " + user + " " + mode[i] + " :is unknown mode char to me");
        }
    }
    return (true);
}

bool Command::handleInvite(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string targetNick;
    std::string channelName;
    std::string prefix;
    Client      *target;
    Channel     *channel;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 " + user + " :You have not registered");
        return (true);
    }
    if (msg.params.size() < 2)
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " INVITE :Not enough parameters");
        return (true);
    }
    targetNick = msg.params[0];
    channelName = msg.params[1];
    target = Command::findClientByNickname(server, targetNick);
    if (target == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 401 " + user + " " + targetNick + " :No such nick/channel");
        return (true);
    }
    channel = server.getChannel(channelName);
    if (channel == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + channelName + " :No such channel");
        return (true);
    }
    if (!channel->hasClient(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 442 " + user + " " + channelName + " :You're not on that channel");
        return (true);
    }
    if (!channel->isOperator(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 482 " + user + " " + channelName + " :You're not channel operator");
        return (true);
    }
    if (channel->hasClient(target))
    {
        server.queueMessage(client.getFd(), ":ircserv 443 " + user + " " + targetNick + " " + channelName + " :is already on channel");
        return (true);
    }
    channel->addInvite(target);
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    server.queueMessage(target->getFd(), prefix + " INVITE " + target->getNickname() + " :" + channelName);
    server.queueMessage(client.getFd(), ":ircserv 341 " + user + " " + target->getNickname() + " " + channelName);
    return (true);
}

bool Command::handleTopic(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string channelName;
    std::string prefix;
    Channel     *channel;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 " + user + " :You have not registered");
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " TOPIC :Not enough parameters");
        return (true);
    }
    channelName = msg.params[0];
    channel = server.getChannel(channelName);
    if (channel == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + channelName + " :No such channel");
        return (true);
    }
    if (!channel->hasClient(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 442 " + user + " " + channelName + " :You're not on that channel");
        return (true);
    }
    if (msg.params.size() == 1)
    {
        if (channel->getTopic().empty())
            server.queueMessage(client.getFd(), ":ircserv 331 " + user + " " + channelName + " :No topic is set");
        else
            server.queueMessage(client.getFd(), ":ircserv 332 " + user + " " + channelName + " :" + channel->getTopic());
        return (true);
    }
    if (channel->isTopicRestricted() && !channel->isOperator(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 482 " + user + " " + channelName + " :You're not channel operator");
        return (true);
    }
    channel->setTopic(msg.params[1]);
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    channel->broadcast(server, NULL, prefix + " TOPIC " + channelName + " :" + msg.params[1]);
    return (true);
}

bool Command::handleKick(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string channelName;
    std::string targetNick;
    std::string reason;
    std::string prefix;
    Channel     *channel;
    Client      *target;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 " + user + " :You have not registered");
        return (true);
    }
    if (msg.params.size() < 2)
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " KICK :Not enough parameters");
        return (true);
    }
    channelName = msg.params[0];
    targetNick = msg.params[1];
    channel = server.getChannel(channelName);
    if (channel == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + channelName + " :No such channel");
        return (true);
    }
    if (!channel->hasClient(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 442 " + user + " " + channelName + " :You're not on that channel");
        return (true);
    }
    if (!channel->isOperator(&client))
    {
        server.queueMessage(client.getFd(), ":ircserv 482 " + user + " " + channelName + " :You're not channel operator");
        return (true);
    }
    target = Command::findClientByNickname(server, targetNick);
    if (target == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 401 " + user + " " + targetNick + " :No such nick/channel");
        return (true);
    }
    if (!channel->hasClient(target))
    {
        server.queueMessage(client.getFd(), ":ircserv 441 " + user + " " + targetNick + " " + channelName + " :They aren't on that channel");
        return (true);
    }
    if (msg.params.size() >= 3 && !msg.params[2].empty())
        reason = msg.params[2];
    else
        reason = user;
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    channel->broadcast(server, NULL, prefix + " KICK " + channelName + " " + targetNick + " :" + reason);
    channel->removeOp(target);
    channel->removeClient(target);
    return (true);
}

Client *Command::findClientByNickname(Server &server, const std::string &nickname)
{
    std::map<int, Client> &clients = server.getClients();

    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == nickname)
            return (&it->second);
    }
    return (NULL);
}

bool Command::handlePrivmsg(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string target;
    std::string text;
    std::string prefix;
    Client      *receiver;
    Channel     *channel;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 * :You have not registered");
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 411 " + client.getNickname() + " :No recipient given (PRIVMSG)");
        return (true);
    }
    if (msg.params.size() < 2 || msg.params[1].empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 412 " + user + " :No text to send");
        return (true);
    }
    target = msg.params[0];
    text = msg.params[1];
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    if (!target.empty() && target[0] == '#')
    {
        channel = server.getChannel(target);
        if (channel == NULL)
        {
            server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + target + " :No such channel");
            return (true);
        }
        if (!channel->hasClient(&client))
        {
            server.queueMessage(client.getFd(), ":ircserv 404 " + user + " " + target + " :Can't send to channel");
            return (true);
        }
        channel->broadcast(server, &client, prefix + " PRIVMSG " + target + " :" + text);
        return (true);
    }
    receiver = Command::findClientByNickname(server, target);
    if (receiver == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 401 " + user + " " + target + " :No such nick/channel");
        return (true);
    }
    server.queueMessage(receiver->getFd(), prefix + " PRIVMSG " + target + " :" + text);
    return (true);
}

bool Command::handleJoin(Server &server, Client &client, const IrcMsg &msg)
{
    std::string channelName;
    Channel     *channel;
    std::string prefix;

    if (!client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 451 * :You have not registered");
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + client.getNickname() + " JOIN :Not enough parameters");
        return (true);
    }
    channelName = msg.params[0];
    if (channelName.empty() || channelName[0] != '#')
    {
        server.queueMessage(client.getFd(), ":ircserv 403 " + client.getNickname() + " " + channelName + " :No such channel");
        return (true);
    }
    channel = server.getChannel(channelName);
    if (channel != NULL)
    {
        if (channel->hasClient(&client))
            return (true);
        if (channel->isInviteOnly() && !channel->isInvited(&client))
        {
            server.queueMessage(client.getFd(), ":ircserv 473 " + client.getNickname() + " " + channelName + " :Cannot join channel (+i)");
            return (true);
        }
        if (channel->hasKey())
        {
            if (msg.params.size() < 2 || msg.params[1] != channel->getKey())
            {
                server.queueMessage(client.getFd(), ":ircserv 475 " + client.getNickname() + " " + channelName + " :Cannot join channel (+k)");
                return (true);
            }
        }
        if (channel->getUserLimit() != -1 && channel->getClientCount() >= (size_t)channel->getUserLimit())
        {
            server.queueMessage(client.getFd(), ":ircserv 471 " + client.getNickname() + " " + channelName + " :Cannot join channel (+l)");
            return (true);
        }
    }
    else
        channel = server.createChannel(channelName);
    channel->addClient(&client);
    channel->removeInvite(&client);
    if (channel->getClientCount() == 1)
        channel->addOperator(&client);
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    //server.queueMessage(client.getFd(), prefix + " JOIN " + channelName);
    channel->broadcast(server, NULL, prefix + " JOIN " + channelName);
    return (true);
}

bool Command::handlePass(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (client.isRegistered() || client.isPasswordAccepted())
    {
        server.queueMessage(client.getFd(), ":ircserv 462 " + user + " :You may not reregister");
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " PASS :Not enough parameters");
        return (true);
    }
    if (msg.params[0] != server.getPassword())
    {
        server.queueMessage(client.getFd(), ":ircserv 464 " + user + " :Password incorrect");
        return (true);
    }
    client.setPasswordAccepted(true);
    Command::tryRegister(server, client);
    return (true);
}

bool Command::handleNick(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 431 " + user + " :No nickname given");
        return (true);
    }
    if (!Command::isValidNickname(msg.params[0]))
    {
        server.queueMessage(client.getFd(), ":ircserv 432 " + user + " " + msg.params[0] + " :Erroneous nickname");
        return (true);
    }
    if (Command::isNicknameUsed(server, client, msg.params[0]))
    {
        server.queueMessage(client.getFd(), ":ircserv 433 " + user + " " + msg.params[0] + " :Nickname is already in use");
        return (true);
    }
    client.setNickname(msg.params[0]);
    Command::tryRegister(server, client);
    return (true);
}

bool Command::handleUser(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 462 " + user + " :You may not reregister");
        return (true);
    }
    if (msg.params.size() < 4)
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " USER :Not enough parameters");
        return (true);
    }
    client.setUsername(msg.params[0]);
    client.setRealname(msg.params[3]);
    Command::tryRegister(server, client);
    return (true);
}

void Command::tryRegister(Server &server, Client &client)
{
    if (client.isRegistered())
        return ;
    if (!client.isPasswordAccepted())
        return ;
    if (client.getNickname().empty())
        return ;
    if (client.getUsername().empty())
        return ;
    client.setRegistered(true);
    server.queueMessage(client.getFd(), ":ircserv 001 " + client.getNickname() + " :Welcome to ft_irc, " + client.getNickname());
}

bool Command::isNicknameUsed(Server &server, Client &client, const std::string &nickname)
{
    std::map<int, Client> &clients = server.getClients();

    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getFd() != client.getFd() && it->second.getNickname() == nickname)
            return (true);
    }
    return (false);
}

bool Command::isValidNickname(const std::string &nickname)
{
    if (nickname.empty())
        return (false);
    if (nickname[0] == '#' || nickname[0] == ':' || nickname[0] == '&')
        return (false);
    for (size_t i = 0; i < nickname.size(); ++i)
    {
        if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '*'
            || nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@')
            return (false);
    }
    return (true);
}

bool Command::handlePing(Server &server, Client &client, const IrcMsg &msg)
{
    if (msg.params.empty())
        server.queueMessage(client.getFd(), ":ircserv 409 * :No origin specified");
    else
        server.queueMessage(client.getFd(), "PONG :" + msg.params[0]);
    return (true);
}

bool Command::handleQuit(Server &server, Client &client, const IrcMsg &msg)
{
    (void)server;
    (void)client;
    (void)msg;
    return (false);
}

void Command::sendUnknownCommand(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    server.queueMessage(client.getFd(), ":ircserv 421 " + user + " " + msg.command + " :Unknown command");
}
