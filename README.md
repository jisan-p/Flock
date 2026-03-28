<p align="center">
  <img src="https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=cplusplus&logoColor=white" />
  <img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" />
  <img src="https://img.shields.io/badge/Networking-TCP%20Sockets-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Linux-lightgrey?style=for-the-badge" />
</p>

<h1 align="center">🐦 Flock</h1>

<p align="center">
  <b>A multi-threaded, terminal-based real-time chat application built entirely in C++17.</b><br/>
  <i>No frameworks. No dependencies. Just raw sockets, threads, and ANSI magic.</i>
</p>

---

## 📖 About

**Flock** is a client–server chat application that runs entirely inside the terminal. It features a rich, colorful Text User Interface (TUI) powered by ANSI escape codes, real-time message delivery over TCP sockets, and a file-based persistence layer—all built from scratch using only the C++ Standard Library and POSIX APIs.

The project follows **Object-Oriented Programming** principles with clean abstractions: an interface-based screen system for the UI, a dedicated networking layer with background listener threads, and a thread-safe database class for persistent storage.

---

## ✨ Features

| Feature | Status |
|---|---|
| 🔐 User Registration & Login | ✅ Implemented |
| 💬 1-to-1 Real-time Messaging | ✅ Implemented |
| 📜 Chat History (persistent) | ✅ Implemented |
| 👥 Online User Listing | ✅ Implemented |
| 🎨 Colorful TUI with ANSI codes | ✅ Implemented |
| 🖥️ Multi-screen Navigation | ✅ Implemented |
| 🧵 Multi-threaded Server (thread-per-client) | ✅ Implemented |
| 📡 Async Message Push (real-time receive) | ✅ Implemented |
| 🔄 Scrollable Chat History | ✅ Implemented |
| 🏠 Group Chat Rooms | 🔜 Planned |
| 🔒 End-to-End Encryption (OpenSSL) | 🔜 Planned |
| 📁 Room-based Chat Persistence | 🔜 Planned |

---

## 🏗️ Architecture

```
Flock/
├── Client/                          # Client application
│   ├── main.cpp                     # Entry point & screen flow controller
│   ├── network/
│   │   ├── TCPClient.hpp/cpp        # TCP connection, send/receive, listener thread
│   ├── ui/
│   │   ├── Screen.hpp               # Abstract base class (interface)
│   │   ├── LoadingScreen.hpp/cpp    # Animated splash screen
│   │   ├── MenuScreen.hpp/cpp       # Login / Register selection
│   │   ├── LoginScreen.hpp/cpp      # User authentication screen
│   │   ├── RegisterScreen.hpp/cpp   # Account creation screen
│   │   ├── HomeScreen.hpp/cpp       # User list & chat selection
│   │   ├── ChatRoomScreen.hpp/cpp   # Real-time 1-to-1 chat interface
│   │   ├── Terminal.hpp/cpp         # ANSI escape code utilities
│   │   └── color.hpp/cpp            # Terminal color formatting
│   └── CMakeLists.txt
│
├── Server/                          # Server application
│   ├── main.cpp                     # Entry point: creates DB & starts TCP server
│   ├── Network/
│   │   ├── TCPServer.hpp/cpp        # Multi-threaded TCP listener & command router
│   │   └── Protocol.hpp             # Shared pipe-delimited text protocol
│   ├── Database/
│   │   ├── Database.hpp/cpp         # Thread-safe file-based storage engine
│   │   ├── User.hpp                 # User model with serialize/deserialize
│   │   └── Room.hpp                 # Room model with serialize/deserialize
│   └── CMakeLists.txt
│
└── README.md
```

### Client–Server Communication

Flock uses a **custom text-based protocol** over TCP. Messages are **pipe-delimited** strings terminated by a newline:

```
COMMAND|arg1|arg2|...\n
```

| Direction | Command | Format | Description |
|---|---|---|---|
| `C → S` | `LOGIN` | `LOGIN\|username\|password` | Authenticate a user |
| `C → S` | `REGISTER` | `REGISTER\|username\|password` | Create a new account |
| `C → S` | `SEND` | `SEND\|targetUser\|message` | Send a message to a user |
| `C → S` | `HISTORY` | `HISTORY\|targetUser` | Request chat history |
| `C → S` | `LIST_USERS` | `LIST_USERS` | Get all registered users |
| `C → S` | `LOGOUT` | `LOGOUT` | Disconnect gracefully |
| `S → C` | `MSG` | `MSG\|senderUser\|message` | Push incoming message in real-time |

### Threading Model

```
┌─────────────────────────────────────────────┐
│                   SERVER                    │
│                                             │
│   Main Thread: accept() loop               │
│       │                                     │
│       ├── Client Thread 1 (handleClient)    │
│       ├── Client Thread 2 (handleClient)    │
│       └── Client Thread N (handleClient)    │
│                                             │
│   Shared: Database (mutex-protected)        │
│   Shared: onlineClients map (mutex-guarded) │
└─────────────────────────────────────────────┘

┌─────────────────────────────────────────────┐
│                   CLIENT                    │
│                                             │
│   Main Thread: UI rendering & input         │
│       │                                     │
│       └── Listener Thread (background)      │
│           Receives MSG pushes from server   │
│           Triggers messageCallback on UI    │
│                                             │
│   Shared: sendMutex (protects socket writes)│
└─────────────────────────────────────────────┘
```

### Database (File-Based)

All data is persisted using plain text files:

| File | Format | Purpose |
|---|---|---|
| `users.txt` | `userID\|username\|password` | Registered user credentials |
| `chats_1-1/<userA>_<userB>.txt` | `timestamp\|senderID\|message` | 1-to-1 chat history |
| `rooms.txt` *(planned)* | `roomID\|roomName\|member1,member2,...` | Group room definitions |

Filenames for 1-to-1 chats are generated by sorting usernames alphabetically, ensuring a consistent single file per conversation pair.

---

## 🛠️ Prerequisites

- **C++17** compatible compiler (`g++`, `clang++`)
- **CMake** ≥ 3.10
- **POSIX-compatible OS** (macOS or Linux)
- **pthreads** (included with your system)

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/jisan-p/Flock.git
cd Flock
```

### 2. Build the Server

```bash
cd Server
mkdir -p build && cd build
cmake ..
make
```

### 3. Build the Client

```bash
cd Client
mkdir -p build && cd build
cmake ..
make
```

### 4. Run

**Start the server first** (it listens on port `4444`):

```bash
./Server/build/FlockServer
```

**Then launch one or more clients** (in separate terminal windows):

```bash
./Client/build/flock
```

> **Tip:** Open multiple terminal windows and register different users to test real-time messaging between them!

---

## 🖥️ Screen Flow

```
┌────────────────┐
│ Loading Screen │  Animated splash with Flock logo
└───────┬────────┘
        │
┌───────▼────────┐
│   Main Menu    │  [1] Login  [2] Register  [3] Exit
└───┬────────┬───┘
    │        │
┌───▼───┐ ┌──▼──────────┐
│ Login │ │  Register   │  Enter username & password
└───┬───┘ └──┬──────────┘
    │        │
    └───┬────┘
        │  (on success)
┌───────▼────────┐
│  Home Screen   │  Lists online/registered users
└───────┬────────┘
        │  (select a user)
┌───────▼────────┐
│  Chat Room     │  Real-time 1-to-1 messaging
│                │  Scrollable history (m/n keys)
│                │  Press 'q' to return to Home
└────────────────┘
```

---

## 🎨 UI Highlights

- **ANSI Escape Codes** — The entire UI is rendered using raw ANSI sequences for cursor positioning, clearing, and styling. No external TUI library needed.
- **Color System** — A dedicated `Color` class provides methods like `Color::cyan()`, `Color::green()`, `Color::bold()` for consistent styling across all screens.
- **Terminal Utilities** — The `Terminal` namespace offers `clear()`, `moveCursor()`, `printAt()`, `hideCursor()`, `showCursor()`, and a custom `getch()` for raw keypress input.
- **Screen Abstraction** — Every screen inherits from the `Screen` interface (`render()`, `handleInput()`, `getNextScreen()`), making it trivial to add new screens.
- **Thread-Safe Rendering** — A shared `screenMutex` prevents race conditions when the listener thread and main thread both write to the terminal.

---

## 🗺️ Roadmap — Future Implementations

### 🔜 Near-Term

- [ ] **Group Chat Rooms** — Create, join, and chat in named rooms with multiple members. The `Room` model and serialization are already in place.
- [ ] **Room-based Chat Persistence** — Store group chat history in `room_chats/<roomName><roomID>.txt` files.
- [ ] **User Online/Offline Status** — Show which users are currently connected in real-time on the Home Screen.
- [ ] **Typing Indicators** — Notify the other user when someone is typing.
- [ ] **Message Timestamps in UI** — Display formatted timestamps alongside each message in the chat view.

### 🔒 Security

- [ ] **Password Hashing** — Hash passwords using SHA-256 (OpenSSL) before storing in `users.txt`.
- [ ] **End-to-End Encryption** — Encrypt messages using AES or RSA (OpenSSL) so that stored and transmitted messages are unreadable without the key.
- [ ] **Secure Transport (TLS)** — Wrap TCP sockets with OpenSSL TLS for encrypted communication between client and server.

### 🧰 Quality of Life

- [ ] **Message Notifications** — Visual/audio bell when a new message arrives while in a different screen.
- [ ] **Emoji Support** — Parse and display basic emoji shortcodes in messages.
- [ ] **Chat Search** — Search through chat history by keyword.
- [ ] **User Profiles** — Display user info (join date, bio) on selection.
- [ ] **File Sharing** — Send files between users over the existing TCP channel.

### 🏗️ Infrastructure

- [ ] **SQLite Database** — Replace file-based storage with SQLite for better concurrency and query support.
- [ ] **Config File** — Allow server IP, port, and other settings to be configured via a `config.txt` or command-line arguments.
- [ ] **Cross-Platform Windows Support** — Add Windows socket (`winsock2`) and terminal (`windows.h`) compatibility.
- [ ] **Logging System** — Add timestamped server logs for debugging and audit trails.
- [ ] **Unit Tests** — Add unit tests for Protocol parsing, Database operations, and serialization.

---

## 🧱 OOP Concepts Used

| Concept | Where |
|---|---|
| **Abstraction** | `Screen` interface with pure virtual functions |
| **Encapsulation** | Private members in `TCPClient`, `TCPServer`, `Database` |
| **Polymorphism** | All screens implement `Screen` and are used interchangeably |
| **Composition** | `TCPServer` holds a `Database&`; `ChatRoomScreen` holds a `TCPClient&` |
| **Serialization** | `User::serialize()` / `User::deserialize()` pattern |
| **RAII** | Destructors in `TCPClient` and `TCPServer` clean up sockets |
| **Thread Safety** | `std::mutex` guards in `Database`, `TCPServer`, `Terminal` |
| **Namespaces** | `Protocol`, `Terminal` namespaces for logical grouping |

---


<p align="center">
  Made with C++
</p>
