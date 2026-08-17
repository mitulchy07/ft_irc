/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:18 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/15 02:35:47 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"


Parser::Parser() {}

Parser::Parser(const Parser &other) { (void)other; }

Parser &Parser::operator=(const Parser &other)
{
	(void)other;
	return (*this);
}

std::string Parser::toUpper(const std::string &value)
{
	std::string	result;

	result = value;
	for (size_t i = 0; i < result.size(); ++i)
	{
		result[i] = static_cast<char>(std::toupper(
					static_cast<unsigned char>(result[i])));
	}
	return (result);
}

IrcMsg Parser::parse(const std::string &line)
{
	IrcMsg	msg;
	size_t		i;
	size_t		start;

	i = 0;

	while (i < line.size() && line[i] == ' ')
		++i;

	if (i < line.size() && line[i] == ':')
	{
		++i;
		start = i;
		while (i < line.size() && line[i] != ' ')
			++i;
		msg.prefix = line.substr(start, i - start);
		while (i < line.size() && line[i] == ' ')
			++i;
	}

	start = i;
	while (i < line.size() && line[i] != ' ')
		++i;
	msg.command = Parser::toUpper(line.substr(start, i - start));

	while (i < line.size())
	{
		while (i < line.size() && line[i] == ' ')
			++i;
		if (i >= line.size())
			break ;

		if (line[i] == ':')
		{
			msg.params.push_back(line.substr(i + 1));
			break ;
		}

		start = i;
		while (i < line.size() && line[i] != ' ')
			++i;
		msg.params.push_back(line.substr(start, i - start));
	}

	return (msg);
}