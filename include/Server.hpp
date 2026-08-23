/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:21 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/23 23:05:54 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <map>
#include <poll.h>
#include <string>
#include <vector>


class Server
{
	private:
		int	_port;
		std::string _password;
		int	_serverFd;
		bool _running;
		std::vector<struct pollfd> _pollFds;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
		
		Server();
		Server(const Server &other);
		Server &operator=(const Server &other);

		int	parsePort(const std::string &port) const;
		void createListeningSocket();
		void setNonBlocking(int fd) const;
		void addPollFd(int fd, short events);
		void removePollFd(int fd);
		void setPollEvents(int fd, short events);

		void acceptClients();
		bool receiveFromClient(int fd);
		bool flushClientOutput(int fd);
		bool processClientBuffer(Client &client);
		bool processLine(Client &client, const std::string &line);

	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		void init();
		void run();
		void stop();

		int	 getPort() const;
		const std::string &getPassword() const;
		Client *getClient(int fd);
		const Client *getClient(int fd) const;
		std::map<int, Client>	&getClients();

		void queueMessage(int fd, const std::string &message);
		void disconnectClient(int fd, const std::string &reason);
		Channel *getChannel(const	std::string &name);
		Channel	*createChannel(const std::string &name);
};

#endif
