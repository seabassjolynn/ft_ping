# ft_ping

**Note:** Project is developed under Debian 12, so it must be run under this OS.

## To Compile the Project:

1. `cd` to the project folder.
2. `sudo make`.

## Description:

`ft_ping` is a custom reimplementation of the standard `ping` utility, developed as a network programming practice project. The purpose is to understand the **ICMP protocol** and gain experience working with **raw sockets**.

`ft_ping` supports typical use cases such as `ft_ping example.com` and includes the following flags:

- **`-c N`**: Stop after sending N packets.
- **`--ttl=N`**: Set the Time to Live (TTL) for packets.
- **`-w N`**: Set a timeout in seconds for the entire ping session.
- **`-W N`**: Set a timeout in seconds to wait for each reply.
- **`-d`**: Enable debug mode.
- **`-v`**: Enable verbose output.
- **`-?`**: Display help and usage information.

This project enhances understanding of low-level network communication while mimicking essential `ping` functionality.
