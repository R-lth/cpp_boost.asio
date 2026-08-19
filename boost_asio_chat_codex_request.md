# Boost.Asio Educational Chat Example Request

## Primary Teaching Goal

Create a Visual Studio 2022 solution for a simple C++17 Boost.Asio console TCP chat example.

This is **not** a feature-rich chat application. The main purpose is to teach
asynchronous object lifetime and ownership in C++.

The code should make it clear:

- who keeps a `ChatSession` alive while an async operation is pending
- why capturing only `[this]` in async callbacks can be unsafe
- why `std::enable_shared_from_this` and `shared_from_this()` are used
- why `std::shared_ptr<T>(this)` is dangerous
- why `async_write()` buffers must remain alive until completion
- how `shared_ptr`, non-owning references, and callbacks express ownership

## Required Visual Studio Structure

Create one Visual Studio solution with two C++ console projects.

```text
BoostAsioChat/
|
+-- BoostAsioChat.sln
|
+-- ChatServer/
|   +-- ChatServer.vcxproj
|   +-- main.cpp
|   +-- ChatServer.h
|   +-- ChatServer.cpp
|   +-- ChatSession.h
|   +-- ChatSession.cpp
|
+-- ChatClient/
|   +-- ChatClient.vcxproj
|   +-- main.cpp
|   +-- ChatClient.h
|   +-- ChatClient.cpp
|
+-- README.md
```

The solution must build two executables:

```text
ChatServer.exe
ChatClient.exe
```

Use Visual Studio 2022 friendly project settings:

- C++17 or later
- x64 Debug as the main test configuration
- Console subsystem
- Boost.Asio found through vcpkg integration

Do **not** use CMake for this version unless Visual Studio project files cannot be generated.
The preferred output is `.sln` + `.vcxproj`.

## Technical Constraints

- Use C++17 or later.
- Use Boost.Asio, not standalone Asio.
- Use TCP.
- Use lambda callbacks.
- Keep the code short and readable.
- Add comments only where lifetime or ownership matters.
- Prefer simple educational code over production-level infrastructure.

Use this style:

```cpp
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
```

## Do Not Use

- coroutine
- `co_await`
- strand
- thread pool
- JSON
- login
- authentication
- database
- nickname system
- chat rooms
- private messages
- packet header protocol
- encryption
- game logic
- complex framework code

## Server Requirements

- Listen on TCP port `7777`.
- Accept multiple clients.
- Create one `ChatSession` per connected client.
- Store active sessions using a `std::shared_ptr<ChatSession>` collection.
- Broadcast each received line message to all connected sessions.
- Sending the message back to the sender is allowed.
- Remove disconnected sessions.
- The server must continue running when one client disconnects.
- Keep server threading simple: one `asio::io_context` and one thread running it.

Expected server startup:

```text
Chat Server Started
Port : 7777
```

Expected connection log:

```text
Client Connected
Client Disconnected
```

## ChatSession Requirements

`ChatSession` represents one client connection.

It must inherit:

```cpp
std::enable_shared_from_this<ChatSession>
```

Required behavior:

- Own the client `tcp::socket`.
- Run an async read loop.
- Prefer newline-delimited messages.
- Use `async_read_until()` or `async_read_some()`; choose the simpler educational design.
- When an async callback needs the session object, capture:

```cpp
auto self = shared_from_this();
```

- Do not rely on `[this]` alone for async callback lifetime.
- Use `asio::async_write()` for sending.
- Ensure write buffer data remains alive until the write callback runs.
- Prefer a tiny write queue if needed to keep `async_write()` calls safe and ordered.

Important ownership intent:

```text
ChatServer
    owns shared_ptr<ChatSession>

Async Callback
    captures shared_ptr<ChatSession>

ChatSession
    has non-owning access back to ChatServer
```

Use the simplest non-owning relationship from `ChatSession` back to `ChatServer`,
such as `ChatServer&` or `ChatServer*`. Do not make `ChatServer` a `shared_ptr`
unless there is a clear need.

## Client Requirements

- Connect to `127.0.0.1:7777` by default.
- Print a short message after connecting.
- Read console input using `std::getline`.
- Send each entered line to the server.
- Receive server messages asynchronously and print them.
- Multiple client processes should be able to chat through the server.
- A single input thread is acceptable.
- Avoid any complex threading model.

Expected client flow:

```text
Connected to server.

> Hello
Hello
```

## Visual Studio Run Requirements

Explain in `README.md` how to run the program from Visual Studio.

At minimum, include these options:

### Option 1: Run separately

1. Build the solution.
2. Run `ChatServer.exe`.
3. Run `ChatClient.exe` in two separate terminals.
4. Type a message in one client.

### Option 2: Multiple startup projects

Explain that Visual Studio can run `ChatServer` and `ChatClient` together by using:

```text
Solution Properties > Common Properties > Startup Project > Multiple startup projects
```

Even with multiple startup projects, a second `ChatClient.exe` may still need to be launched manually for the two-client test.

## Minimal Runtime Flow

The implementation should follow this conceptual flow:

```text
Client Connect
      |
      v
ChatSession created
      |
      v
Async Read starts
      |
      v
Message received
      |
      v
Server Broadcast
      |
      v
Async Write to sessions
      |
      v
Async Read continues
```

## README.md Requirements

Create `README.md` focused on design explanation and Visual Studio usage.

It must include:

- solution/project structure
- how to build in Visual Studio 2022
- how to run `ChatServer.exe`
- how to run two `ChatClient.exe` instances
- Object lifetime in async operations
- why async operations can continue after the initiating function returns
- why `[this]` does not extend object lifetime
- why `shared_from_this()` is used
- why `std::shared_ptr<ChatSession>(this)` is wrong
- how `ChatServer`, `ChatSession`, and async callbacks share ownership
- why `async_write()` buffer lifetime matters

Include these Mermaid diagrams:

### Class Diagram

Show:

- `ChatServer`
- `ChatSession`
- `ChatSession` inheriting `enable_shared_from_this`
- `ChatServer` owning many sessions
- `ChatSession` having non-owning access back to `ChatServer`

### Ownership Diagram

Show this ownership idea:

```text
ChatServer
    |
    +-- shared_ptr
           |
           v
       ChatSession
           ^
           |
       shared_ptr
           |
      Async Callback
```

### Async Flow Diagram

Show:

- async read starts
- callback captures `self`
- message is received
- server broadcasts
- async write starts
- write buffer remains alive until completion

## Acceptance Test

Run three terminals.

Terminal 1:

```text
ChatServer.exe
```

Terminal 2:

```text
ChatClient.exe
> Hello
```

Terminal 3:

```text
ChatClient.exe
```

Expected result:

- Terminal 2 and Terminal 3 both receive `Hello`.
- Closing one client does not stop the server.
- Remaining clients can continue chatting.

## Most Important Question

The finished project should help students answer this question:

> While an asynchronous operation is still pending, who keeps the object and
> the buffer data alive?