# ft_irc server foundation

This package contains the five requested server implementation files plus the
minimum supporting files required because the repository headers and client
implementation are currently empty.

## Build

```bash
make
./ircserv 6667 changeme
```

## Smoke test

In another terminal:

```bash
nc -C 127.0.0.1 6667
PING :12345
```

Expected reply:

```text
PONG :12345
```

## Next integration point

`Server::processLine()` in `src/server/Server.cpp` currently handles only
`PING` and `QUIT`. Replace its body with your Parser/Command dispatcher when
those modules are implemented.
