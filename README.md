*This project has been created as part of the 42 curriculum by mshariar, hchowdhu.*

# ft_irc

## Description

`ft_irc` is a C++98 implementation of a small IRC server.

The goal of the project is to understand how an IRC server works internally: socket creation, client authentication, command parsing, channel management, message broadcasting, channel operators, and non-blocking network I/O.

This server accepts multiple TCP clients at the same time and manages them through a single `poll()` loop. It implements the required subset of IRC commands from the 42 subject, including registration commands, channel commands, private messages, and channel modes.

The project is not intended to be a production-ready IRC daemon. It is an educational implementation focused on protocol handling, networking, and clean server architecture.

---

## Instructions

### Compilation

Compile the project with:

```bash
make
```

Clean object files:

```bash
make clean
```

Clean object files and the executable:

```bash
make fclean
```

Rebuild from scratch:

```bash
make re
```

The project is compiled with C++98 and strict flags:

```bash
-Wall -Wextra -Werror -std=c++98
```

### Execution

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 123
```

Arguments:

- `<port>`: port number where the IRC server listens.
- `<password>`: password required by clients before registration.

### Connecting with netcat

In another terminal:

```bash
nc -C 127.0.0.1 6667
```

Example IRC session:

```irc
PASS 123
NICK abrar
USER abrar 0 * :Mohammed Abrar
JOIN #test
PRIVMSG #test :hello everyone
```

### Connecting with Irssi

You can also test the server with an IRC client such as `irssi`:

```bash
irssi
```

Inside Irssi:

```irc
/connect 127.0.0.1 6667 123
/nick abrar
/join #test
/msg #test hello
```

---

## Supported Commands

### Registration and connection

| Command | Description |
|--------|-------------|
| `PASS` | Sends the server password |
| `NICK` | Sets or changes the nickname |
| `USER` | Sets the username and real name |
| `PING` | Server replies with `PONG` |
| `QUIT` | Disconnects the client |
| Unknown command | Server replies with numeric `421` |

### Channel and messaging commands

| Command | Description |
|--------|-------------|
| `JOIN` | Joins or creates a channel |
| `PRIVMSG` | Sends a message to a user or channel |
| `KICK` | Removes a user from a channel |
| `INVITE` | Invites a user to a channel |
| `TOPIC` | Views or changes a channel topic |
| `MODE` | Manages channel modes |

---

## Supported Channel Modes

| Mode | Description |
|------|-------------|
| `+i` / `-i` | Enable or disable invite-only mode |
| `+t` / `-t` | Restrict topic changes to channel operators |
| `+k <key>` / `-k` | Set or remove a channel password |
| `+o <nick>` / `-o <nick>` | Give or remove channel operator privilege |
| `+l <limit>` / `-l` | Set or remove a user limit |

---

## Usage Examples

### Join a channel

```irc
JOIN #test
```

### Send a channel message

```irc
PRIVMSG #test :hello channel
```

### Send a private message

```irc
PRIVMSG bob :hello bob
```

### Set a topic

```irc
TOPIC #test :project discussion
```

### Read the current topic

```irc
TOPIC #test
```

### Invite a user

```irc
INVITE bob #test
```

### Kick a user

```irc
KICK #test bob :reason
```

### Enable invite-only mode

```irc
MODE #test +i
```

### Disable invite-only mode

```irc
MODE #test -i
```

### Restrict topic changes to operators

```irc
MODE #test +t
```

### Set a channel key

```irc
MODE #test +k secret
```

### Join a channel using a key

```irc
JOIN #test secret
```

### Give operator privilege

```irc
MODE #test +o bob
```

### Remove operator privilege

```irc
MODE #test -o bob
```

### Set a user limit

```irc
MODE #test +l 5
```

---

## Technical Choices

### C++98

The project follows the 42 subject requirement and is written in C++98.

### One poll loop

The server uses a single `poll()` loop to monitor:

- the listening socket;
- connected client sockets;
- readable events with `POLLIN`;
- writable events with `POLLOUT`;
- socket errors and disconnections.

### Non-blocking sockets

Client sockets are configured as non-blocking.  
This prevents one slow or inactive client from blocking the whole server.

### Input buffering

Client input is stored in a buffer until a full IRC line is received.  
This allows the server to handle partial TCP messages correctly.

### Output buffering

Server replies are queued in each client output buffer.  
The server sends queued messages when the client socket is ready for writing.

### Channel cleanup

When a client disconnects, the server removes that client from every channel.  
Empty channels are deleted.

---

## Project Structure

Typical project layout:

```text
.
├── Makefile
├── README.md
├── include/
│   ├── Channel.hpp
│   ├── Client.hpp
│   ├── Command.hpp
│   ├── Parser.hpp
│   └── Server.hpp
└── src/
    ├── channel/
    │   └── Channel.cpp
    ├── client/
    │   └── Client.cpp
    ├── commands/
    │   └── Command.cpp
    ├── parser/
    │   └── Parser.cpp
    ├── server/
    │   ├── Server.cpp
    │   ├── ServerClient.cpp
    │   ├── ServerInit.cpp
    │   ├── ServerLoop.cpp
    │   └── ServerSocket.cpp
    └── main.cpp
```

---

## Testing

During development, the server was tested with:

- `nc -C`;
- custom Python scripts;
- manual multi-client tests;
- command regression tests;
- channel mode tests;
- disconnect and cleanup checks.

Tested behavior includes:

- registration with `PASS`, `NICK`, and `USER`;
- duplicate nicknames;
- invalid nicknames;
- missing parameters;
- `JOIN`;
- duplicate joins;
- `PRIVMSG` to users and channels;
- `KICK`;
- `INVITE`;
- `TOPIC`;
- `MODE +i`;
- `MODE +t`;
- `MODE +k`;
- `MODE +o`;
- `MODE +l`;
- channel cleanup after disconnection.

Example local test command:

```bash
make fclean && make
./test.py
```

The local test script is used only for development and is ignored by Git.

---

## Resources

### IRC protocol references

- RFC 1459 — Internet Relay Chat Protocol  
  https://www.rfc-editor.org/info/rfc1459/

- RFC 2810 — Internet Relay Chat: Architecture  
  https://www.rfc-editor.org/info/rfc2810/

- RFC 2811 — Internet Relay Chat: Channel Management  
  https://www.rfc-editor.org/info/rfc2811/

- RFC 2812 — Internet Relay Chat: Client Protocol  
  https://www.rfc-editor.org/info/rfc2812/

### Network programming references

- Beej’s Guide to Network Programming  
  https://beej.us/guide/bgnet/

- Linux manual page for `poll`  
  https://man7.org/linux/man-pages/man2/poll.2.html

- Linux manual page for sockets  
  https://man7.org/linux/man-pages/man7/socket.7.html

- Linux manual page for `recv`  
  https://man7.org/linux/man-pages/man2/recv.2.html

- Linux manual page for `send`  
  https://man7.org/linux/man-pages/man2/send.2.html

### IRC client references

- Irssi documentation  
  https://irssi.org/documentation/

- Irssi user manual  
  https://irssi.org/documentation/manual/

---

## Use of AI

AI assistance was used during the development of this project as a support tool.

It was used for:

- understanding the IRC protocol and command behavior;
- planning the implementation order;
- explaining socket and `poll()`-based server design;
- reviewing error replies and numeric responses;
- debugging compile errors;
- designing local test scenarios;
- generating temporary local Python test scripts;
- improving the README wording and structure.

AI was not used as a replacement for understanding the project.  
The implementation was manually reviewed, compiled, tested, and adjusted by the project members.

The final source code decisions, integration, testing, and validation were performed by the authors.

---

## Notes

This project is part of the 42 curriculum.  
It focuses on learning network programming, IRC protocol basics, C++98 design, and multi-client server architecture.