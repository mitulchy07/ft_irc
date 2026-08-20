/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:01 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/20 03:13:00 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

class Client;
class Server;

class Channel
{
    private:
        std::string             _name;
        std::vector<Client *>   _clients;
        std::vector<Client *>   _operators;
        std::string             _topic;
        std::vector<Client *>   _invited;
        bool                    _inviteOnly;
        bool                    _topicRestricted;
        bool                    _hasKey;
        std::string             _key;
        int                     _userLimit;
        Channel();
        
    public:
        Channel(const std::string &name);
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
        ~Channel();
        
        bool                isInviteOnly() const;
        void                setInviteOnly(bool value);
        bool                isTopicRestricted() const;
        void                setTopicRestricted(bool value);
        bool                hasKey() const;
        const std::string   &getKey() const;
        void                setKey(const std::string &key);
        void                removeKey();
        int                 getUserLimit() const;
        void                setUserLimit(int limit);
        void                removeUserLimit();
        const std::string   &getTopic() const;
        void                setTopic(const std::string &topic);
        const std::string   &getName() const;
        size_t              getClientCount() const;
        bool                hasClient(Client *client) const;
        bool                isOperator(Client *client) const;
        void                addClient(Client *client);
        void                addOperator(Client *client);
        void                removeOp(Client *client);
        void                removeClient(Client *client);
        bool                isInvited(Client *client) const;
        void                addInvite(Client *client);
        void                removeInvite(Client *client);
        void                broadcast(Server &server, Client *sender, const std::string &msg);
};
