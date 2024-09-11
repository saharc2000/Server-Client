# STOMP Server-Client for World Cup Updates

Stay updated with World Cup games using this STOMP-based server-client system. The server, implemented in Java, supports both Thread-Per-Client (TPC) and Reactor modes, while the client, written in C++, enables interaction with the server.

## Features

- **STOMP Protocol**: Communication follows the STOMP (Simple Text Oriented Messaging Protocol) standard.
- **Server**: Handles topics (queues) and forwards messages to clients.
- **Client**: Allows users to log in, join game channels, report game events, and summarize game updates.

## Components

### Server

- Implemented in Java
- Supports Thread-Per-Client (TPC) and Reactor modes
- Handles STOMP frames (CONNECT, CONNECTED, MESSAGE, RECEIPT, ERROR)

### Client

- Implemented in C++
- Commands include login, join, exit, report, summarize, and logout
- Interacts with the server using STOMP frames

## STOMP Frames

- **CONNECT**: Initiates a connection
- **CONNECTED**: Acknowledges successful connection
- **SEND**: Sends a message to a topic
- **SUBSCRIBE**: Subscribes to a topic
- **UNSUBSCRIBE**: Unsubscribes from a topic
- **DISCONNECT**: Ends the connection
- **MESSAGE**: Received message from a topic
- **RECEIPT**: Acknowledges receipt of a frame
- **ERROR**: Indicates an error occurred

## Client Commands

- `login {host:port} {username} {password}`: Connects or logs in a user
- `join {game_name}`: Subscribes to a game channel
- `exit {game_name}`: Unsubscribes from a game channel
- `report {file}`: Sends game events from a file to the server
- `summary {game_name} {user} {file}`: Saves game updates to a file
- `logout`: Logs out and disconnects from the server
