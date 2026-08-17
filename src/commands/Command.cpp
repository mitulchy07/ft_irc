/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 01:40:00 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/17 23:47:43 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"

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

bool    Command::execute(Server &server, Client &client, const IrcMsg &msg)
{
    if(msg.command.empty())
        return true;
    if (msg.command == "PING")
        return (Command::handlePing(server, client, msg));
    if (msg.command == "QUIT")
        return (Command::handleQuit(server, client, msg));
    if (msg.command == "PASS")
        return (Command::handlePass(server, client, msg));
    if (msg.command == "NICK")
        return (Command::handleNick(server, client, msg));
    if (msg.command == "USER")
        return (Command::handleUser(server, client, msg));
    if (msg.command == "KICK")
        return (Command::handleKick(server, client, msg));
    if(msg.command == "JOIN")
        return (Command::handleJoin(server, client, msg));
    if (msg.command == "PRIVMSG")
        return (Command::handlePrivmsg(server, client, msg));
    Command::sendUnknownCommand(server, client, msg);
    return true;
}

bool    Command::handleKick(Server &server, Client &client, Const IrcMsg &msg)
{
    std::string user;
    std::string targeted_user;
    std::string prefix;
    std::string reason;
    Channel *channel;
    Client *target;

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
        server.queueMessage(client.getFd(), ":ircserv ")
    }
    
}


Client *Command::findClientByNickname(Server &server, const std::string &nickname)
{
    std::map<int, Client> &clients = server.getClients();
    for(std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == nickname)
            return(&it->second);
    }
    return (NULL);
}

bool Command::handlePrivmsg(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    std::string target;
    std::string text;
    std::string prefix;
    Client  *receiver;
    Channel *channel;

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
        return true;
    }
    if(msg.params.size() < 2 || msg.params[1].empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 412 " + user + " :No text to send");
        return true;
    }
    target = msg.params[0];
    text = msg.params[1];
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    
    if (!target.empty() && target[0] == '#')
    {
        channel = server.getChannel(target);
        if(channel == NULL)
        {
            server.queueMessage(client.getFd(), ":ircserv 403 " + user + " " + target + " :No such channel");
		    return (true);
        }
        if (!channel->hasClient(&client))
        {
            server.queueMessage(client.getFd(), ":ircserv 404 " + user + " " + target + " :Can't send to channel");
            return true;
        }
        channel->broadcast(server, &client, prefix + " PRIVMSG " + target + " :" + text);
        return true;
    }
    
    receiver = Command::findClientByNickname(server, target);
    if (receiver == NULL)
    {
        server.queueMessage(client.getFd(), ":ircserv 401 " + user + " " + target + " :No such nick/channel");
        return true;
    }
    server.queueMessage(receiver->getFd(), prefix + " PRIVMSG " + target + " :" + text);
    return (true);
}

bool Command::handleJoin(Server &server, Client &client, const IrcMsg &msg)
{
    std::string     channelName;
    Channel *channel;
    std::string prefix;

    if (!client.isRegistered())
    {
		server.queueMessage(client.getFd(), ":ircserv 451 * :You have not registered");
		return (true);
	}
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + client.getNickname() + " JOIN :Not enough parameters");
        return true;
    }
    channelName = msg.params[0];
    if(channelName.empty() || channelName[0] != '#')
    {
		server.queueMessage(client.getFd(), ":ircserv 403 " + client.getNickname() + " " + channelName + " :No such channel");
		return (true);
	}
    channel = server.createChannel(channelName);
    if (channel->hasClient(&client))
        return (true);
    channel->addClient(&client);
    if (channel->getClientCount() == 1)
        channel->addOperator(&client);
    prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    server.queueMessage(client.getFd(), prefix + " JOIN " + channelName);
    return true;
    
}

bool Command::handlePass(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (client.isRegistered() || client.isPasswordAccepted())
    {
        server.queueMessage(client.getFd(), ":ircserv 462 " + user + " :You may not reregister" );
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " PASS :Not enough parameters");
        return true;
    }
    if (msg.params[0] != server.getPassword())
    {
        server.queueMessage(client.getFd(), ":ircserv 464 " + user + " :Password incorrect");
        return true;
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
		server.queueMessage(client.getFd(),
			":ircserv 431 " + user + " :No nickname given");
		return (true);
	}
	if (!Command::isValidNickname(msg.params[0]))
	{
		server.queueMessage(client.getFd(),
			":ircserv 432 " + user + " " + msg.params[0] + " :Erroneous nickname");
		return (true);
	}
	if (Command::isNicknameUsed(server, client, msg.params[0]))
	{
		server.queueMessage(client.getFd(),
			":ircserv 433 " + user + " " + msg.params[0] + " :Nickname is already in use");
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
        server.queueMessage(client.getFd(), ":ircserv 462 " + user + " :You may not reregister" );
        return (true);
    }
    if (msg.params.size() < 4)
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + " USER :Not enough parameters");
        return true;
    }
    client.setUsername(msg.params[0]);
    client.setRealname(msg.params[3]);
    Command::tryRegister(server, client);
    return(true);
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

    for(std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getFd() != client.getFd() && it->second.getNickname() == nickname)
        return true;
    }
    return(false);
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
    return true;
}

bool Command::handleQuit(Server &server, Client &client,
		const IrcMsg &msg)
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