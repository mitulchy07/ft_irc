/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:12:51 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/25 23:33:55 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0]
				  << " <port> <password>" << std::endl;
		return (1);
	}

	try
	{
		Server server(argv[1], argv[2]);

		server.init();
		server.run();
	}
	catch (const std::exception &error)
	{
		std::cerr << "Error: " << error.what() << std::endl;
		return (1);
	}
	catch (...)
	{
		std::cerr << "Error: unknown server error" << std::endl;
		return (1);
	}

	return (0);
}