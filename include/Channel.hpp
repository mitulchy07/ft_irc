/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:01 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/18 20:15:03 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <cstddef>

class Client;
class Server;

class Channel
{
private:
	std::string				_name;
	std::string				_topic;
	std::vector<Client *>	_clients;
	std::vector<Client *>	_operators;

public:
	Channel();
	Channel(const std::string &name);
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);
	~Channel();

	const std::string &getName() const;
	const std::string &getTopic() const;

	void setTopic(const std::string &topic);

	void addClient(Client *client);
	void removeClient(Client *client);

	void addOperator(Client *client);
	void removeOperator(Client *client);

	bool hasClient(Client *client) const;
	bool isOperator(Client *client) const;

	size_t getClientCount() const;

	const std::vector<Client *> &getClients() const;
	const std::vector<Client *> &getOperators() const;

	void broadcast(Server &server, Client *sender,
		const std::string &msg);
};

#endif