/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:05 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/18 03:03:30 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	private:
		int _fd;
		std::string	_hostname;
		std::string	_inputBuffer;
		std::string	_outputBuffer;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		bool _passwordAccepted;
		bool _registered;

	public:
		Client();
		Client(int fd, const std::string &hostname);

		int getFd() const;
		const std::string &getHostname() const;
		const std::string &getNickname() const;
		const std::string &getUsername() const;
		const std::string &getRealname() const;
		bool isPasswordAccepted() const;
		bool isRegistered() const;

		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void setRealname(const std::string &realname);
		void setPasswordAccepted(bool accepted);
		void setRegistered(bool registered);

		std::string	&inputBuffer();
		std::string	&outputBuffer();
};

#endif