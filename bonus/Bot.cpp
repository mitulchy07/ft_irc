/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 05:04:29 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/21 01:07:28 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

static std::string intToString(int n)
{
	std::stringstream ss;
	ss << n;
	return (ss.str());
}

static void sendLine(int fd, const std::string &line)
{
	std::string msg = line + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

static int connectBot(const std::string &host, int port)
{
	int fd;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return (-1);

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
	{
		close(fd);
		return (-1);
	}

	if (connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
	{
		close(fd);
		return (-1);
	}

	return (fd);
}

static std::string getNick(const std::string &line)
{
	size_t end;

	if (line.empty() || line[0] != ':')
		return ("unknown");

	end = line.find('!');
	if (end == std::string::npos)
		end = line.find(' ');
	if (end == std::string::npos)
		return ("unknown");

	return (line.substr(1, end - 1));
}

static std::string getTimeString(void)
{
	time_t now;
	struct tm *info;
	char buffer[64];

	now = time(NULL);
	info = localtime(&now);
	if (info == NULL)
		return ("time unavailable");

	std::strftime(buffer, sizeof(buffer), "%H:%M:%S", info);
	return (std::string(buffer));
}

static void handleCommand(int fd, const std::string &sender,
	const std::string &target, const std::string &text)
{
	int roll;

	if (text.empty() || text[0] != '!')
		return ;

	if (text == "!help")
		sendLine(fd, "PRIVMSG " + target + " :Commands: !help !ping !hello !time !roll !echo <text>");
	else if (text == "!ping")
		sendLine(fd, "PRIVMSG " + target + " :pong");
	else if (text == "!hello")
		sendLine(fd, "PRIVMSG " + target + " :hello " + sender);
	else if (text == "!time")
		sendLine(fd, "PRIVMSG " + target + " :current time is " + getTimeString());
	else if (text == "!roll")
	{
		roll = (std::rand() % 6) + 1;
		sendLine(fd, "PRIVMSG " + target + " :" + sender + " rolled " + intToString(roll));
	}
	else if (text.substr(0, 6) == "!echo ")
		sendLine(fd, "PRIVMSG " + target + " :" + text.substr(6));
	else
		sendLine(fd, "PRIVMSG " + target + " :unknown command. Try !help");
}

static void handleLine(int fd, const std::string &botNick, const std::string &line)
{
	size_t msgPos;
	size_t targetStart;
	size_t textStart;
	std::string sender;
	std::string target;
	std::string text;

	std::cout << line << std::endl;

	if (line.substr(0, 4) == "PING")
	{
		sendLine(fd, "PONG " + line.substr(5));
		return ;
	}

	msgPos = line.find(" PRIVMSG ");
	if (msgPos == std::string::npos)
		return ;

	targetStart = msgPos + 9;
	textStart = line.find(" :", targetStart);
	if (textStart == std::string::npos)
		return ;

	sender = getNick(line);
	target = line.substr(targetStart, textStart - targetStart);
	text = line.substr(textStart + 2);

	if (target == botNick)
		target = sender;

	handleCommand(fd, sender, target, text);
}

static void readServer(int fd, const std::string &botNick)
{
	char buffer[512];
	ssize_t bytes;
	std::string data;
	std::string line;
	size_t pos;

	while (true)
	{
		bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes <= 0)
			break ;

		buffer[bytes] = '\0';
		data += buffer;

		while ((pos = data.find('\n')) != std::string::npos)
		{
			line = data.substr(0, pos);
			data.erase(0, pos + 1);

			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);

			if (!line.empty())
				handleLine(fd, botNick, line);
		}
	}
}

int main(int argc, char **argv)
{
	int fd;
	int port;
	std::string botNick;

	if (argc != 5)
	{
		std::cerr << "Usage: ./ircbot <host> <port> <password> <channel>" << std::endl;
		return (1);
	}

	signal(SIGPIPE, SIG_IGN);
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	port = std::atoi(argv[2]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Bot: invalid port" << std::endl;
		return (1);
	}

	botNick = "ftbot";
	fd = connectBot(argv[1], port);
	if (fd == -1)
	{
		std::cerr << "Bot: connection failed" << std::endl;
		return (1);
	}

	sendLine(fd, "PASS " + std::string(argv[3]));
	sendLine(fd, "NICK " + botNick);
	sendLine(fd, "USER ftbot 0 * :ftbot");
	sendLine(fd, "JOIN " + std::string(argv[4]));
	sendLine(fd, "PRIVMSG " + std::string(argv[4]) + " :ftbot online. Try !help");

	readServer(fd, botNick);
	close(fd);
	return (0);
}