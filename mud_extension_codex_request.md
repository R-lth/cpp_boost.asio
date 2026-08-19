# Boost.Asio Chat to MUD Extension Request

## Goal

Extend the existing Boost.Asio chat server/client project into a simple educational MUD game.

This is **not** a full game engine. The goal is to reuse the async server/session
structure from the chat project and add a small server-owned game world.

The extension should help students understand:

- how a chat server can become a game server
- how network sessions differ from game objects
- how server-side game state is owned and updated
- how async callback lifetime rules still apply
- how broadcast rules change when players are in different rooms

## Starting Point

Start from the project generated from:

```text
boost_asio_chat_codex_request.md
```

Keep these ideas from the chat project:

- Visual Studio 2022 solution
- Boost.Asio
- TCP server/client
- console programs
- async accept/read/write
- one session object per client
- `std::enable_shared_from_this`
- async callbacks capturing `self = shared_from_this()`
- safe `async_write()` buffer lifetime
- simple README with ownership/lifetime explanation

## Output Structure

Prefer creating new projects so the chat version remains easy to compare.

```text
BoostAsioChat/
|
+-- BoostAsioChat.sln
|
+-- ChatServer/
+-- ChatClient/
|
+-- MudServer/
|   +-- MudServer.vcxproj
|   +-- main.cpp
|   +-- MudServer.h
|   +-- MudServer.cpp
|   +-- MudSession.h
|   +-- MudSession.cpp
|   +-- GameWorld.h
|   +-- GameWorld.cpp
|   +-- Room.h
|   +-- Player.h
|
+-- MudClient/
|   +-- MudClient.vcxproj
|   +-- main.cpp
|   +-- MudClient.h
|   +-- MudClient.cpp
|
+-- README.md
```

If creating new Visual Studio projects is difficult, extending `ChatServer` and
`ChatClient` is allowed. However, prefer `MudServer` and `MudClient` for teaching clarity.

The solution should build:

```text
ChatServer.exe
ChatClient.exe
MudServer.exe
MudClient.exe
```

## Technical Constraints

- Use C++17 or later.
- Use Boost.Asio.
- Use TCP port `7777` for the MUD server.
- Use console input/output.
- Use lambda callbacks.
- Keep code educational and readable.
- Avoid production-level architecture.
- Avoid coroutine, `co_await`, strand, thread pool, database, login, JSON, encryption, or complex protocol headers.

## Core Architecture

Recommended classes:

- `MudServer`
- `MudSession`
- `GameWorld`
- `Room`
- `Player`

Responsibilities:

```text
MudServer
    accepts TCP clients
    owns active MudSession objects
    owns GameWorld
    routes session messages to GameWorld

MudSession
    owns one tcp::socket
    handles async read/write
    sends text to one client
    has non-owning access to MudServer

GameWorld
    owns rooms
    owns players
    parses and executes player commands
    decides who receives room messages

Room
    stores room id, name, description, and exits

Player
    stores player id, display name, and current room id
```

## Ownership Rules

Use this ownership model:

```text
MudServer
    owns shared_ptr<MudSession> collection

MudServer
    owns GameWorld directly

GameWorld
    owns Room and Player state

MudSession
    has non-owning access back to MudServer

Async Callback
    captures shared_ptr<MudSession> self
```

Do not make everything a `shared_ptr`.

Recommended:

- `MudServer` owns sessions with `std::set<std::shared_ptr<MudSession>>`
- `MudServer` owns `GameWorld` as a normal member variable
- `MudSession` refers to `MudServer` using `MudServer&`
- `GameWorld` stores players by a simple id
- `GameWorld` stores rooms in a simple map

## Lifetime Requirements

`MudSession` must inherit:

```cpp
std::enable_shared_from_this<MudSession>
```

In async callbacks, use:

```cpp
auto self = shared_from_this();
```

Then capture `self` in the lambda.

Prefer:

```cpp
[self](boost::system::error_code ec, std::size_t length)
{
    self->HandleRead(ec, length);
}
```

This makes it clear that `self`, not `this`, keeps the session alive.

`async_write()` buffer lifetime must be safe. A small write queue is recommended.

## Game World Requirements

Create at least 3 rooms.

Example rooms:

```text
Old Gate
Broken Hall
Torch Room
```

Each room should have:

- id
- name
- description
- exits

Example exits:

```text
Old Gate north -> Broken Hall
Old Gate east  -> Torch Room
Broken Hall south -> Old Gate
Torch Room west -> Old Gate
```

## Player Requirements

Each connected client becomes one player.

Minimum player data:

- player id
- display name
- current room id

Simple names are acceptable:

```text
Player1
Player2
Player3
```

When a player joins:

- add the player to `GameWorld`
- place the player in the starting room
- send a welcome message
- send the result of `look`
- notify players in the same room

When a player disconnects:

- remove the player from `GameWorld`
- notify players in the same room
- remove the `MudSession` from `MudServer`

## Commands

Support these commands:

```text
look
north
south
east
west
say <message>
quit
help
```

Command behavior:

```text
look
    show current room name, description, exits, and visible players

north/south/east/west
    move if an exit exists
    show the new room
    notify old room and new room
    show an error if no exit exists

say <message>
    broadcast only to players in the same room

help
    show available commands

quit
    disconnect the client cleanly
```

Unknown commands should return a simple error message.

## Broadcast Rules

Do not broadcast every message to every connected client.

MUD broadcast should depend on room membership.

Examples:

```text
Player1 says: hello
```

Only players in the same room as `Player1` should receive this message.

Movement messages should also be room-scoped:

```text
Player1 leaves north.
Player1 enters from south.
```

## Client Requirements

`MudClient` can be very similar to `ChatClient`.

Requirements:

- connect to `127.0.0.1:7777`
- read console input using `std::getline`
- send entered commands to the server
- receive server messages asynchronously
- print server messages
- `/quit` or `quit` should close the client

No client-side game state is required.

The server owns the truth.

## Ollama

Do **not** implement Ollama yet.

Add TODO comments showing where Ollama could be added later.

Good TODO locations:

- generating richer room descriptions for `look`
- generating NPC dialogue
- rewriting room descriptions with atmosphere

Important design rule:

```text
C++ owns the game rules and game state.
Ollama may generate text later, but it should not own the rules.
```

## README.md Requirements

Update `README.md` with:

- how the chat server structure was extended into a MUD server
- how to build the Visual Studio solution
- how to run `MudServer.exe`
- how to run two `MudClient.exe` instances
- command list
- class responsibility table
- ownership/lifetime explanation
- room-scoped broadcast explanation
- where Ollama could be added later

Include Mermaid diagrams:

### Class Responsibility Diagram

Show:

- `MudServer`
- `MudSession`
- `GameWorld`
- `Room`
- `Player`

### Ownership Diagram

Show:

```text
MudServer
    owns sessions
    owns GameWorld

GameWorld
    owns rooms
    owns players

Async Callback
    owns MudSession temporarily through shared_ptr self
```

### Command Flow Diagram

Show:

```text
MudClient input
    -> MudSession async read
    -> MudServer
    -> GameWorld command handling
    -> MudSession async write
```

## Acceptance Test

Run three terminals.

Terminal 1:

```text
MudServer.exe
```

Terminal 2:

```text
MudClient.exe
```

Terminal 3:

```text
MudClient.exe
```

Expected:

- both clients connect successfully
- each client receives a welcome message
- `look` shows the current room
- `north`, `south`, `east`, `west` move between rooms when exits exist
- invalid movement shows an error
- `say hello` is visible only to players in the same room
- disconnecting one client does not stop the server
- remaining clients can continue playing

## Drill Me Targets

After implementation, students should be able to answer:

- Which class owns the sockets?
- Which class owns the players?
- Which class owns the rooms?
- Which code keeps `MudSession` alive during async callbacks?
- Where is the `async_write()` buffer lifetime protected?
- Why is `MudSession` not the same thing as `Player`?
- How does the server decide who receives a `say` message?
- What state changes when a player moves rooms?

## Most Important Question

The finished project should help students answer this question:

> How does a networking session become a player inside a server-owned game world,
> while still preserving async object lifetime and buffer lifetime safety?