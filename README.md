# AMSCode (AMS) -- Automated Log Monitoring & Observation Platform

> A Domain-Specific Language and full-stack platform for writing, compiling, and deploying real-time log monitoring rules.

---

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
  - [Environment Setup](#environment-setup)
  - [Docker (Recommended)](#docker-recommended)
  - [Manual Setup](#manual-setup)
- [Part 1 -- AMS-Lang Compiler](#part-1--ams-lang-compiler)
  - [Install Dependencies](#install-dependencies)
  - [Build the Compiler](#build-the-compiler)
  - [Using the Compiler](#using-the-compiler)
- [Part 2 -- AMS Platform & IDE](#part-2--ams-platform--ide)
  - [Start the IDE & Platform](#start-the-ide--platform)
  - [Using the IDE](#using-the-ide)
  - [Platform Dashboard](#platform-dashboard)
- [AMS-Lang Syntax Guide](#ams-lang-syntax-guide)
- [Example Scripts](#example-scripts)
- [Tech Stack](#tech-stack)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

**AutomonScript (AMS-Lang)** is a purpose-built DSL for cybersecurity monitoring and automated threat response. It provides:

- **Human-readable syntax** -- Write monitoring rules in plain English-like constructs
- **Pattern-based detection** -- Match log patterns, regex, and field conditions
- **Time-windowed correlation** -- Detect events occurring N times within a time window
- **Signal-based async scheduling** -- Sources, Events, and Observers with tracked state
- **Log file integration** -- Monitor real log files with built-in parsing and filtering
- **Automated response** -- Trigger alerts, emails, API calls, IP blocking, and script execution
- **High-performance compilation** -- Transpiles `.ams` scripts to native C++ executables
- **Full-featured Platform & IDE** -- Professional web and Electron-based desktop IDE, real-time dashboard, log source integration, rule editors, alert routes, and team management

---

## Project Structure

```text
ams-lang/                       # Compiler & Language Engine
  grammar/AMS.g4                # ANTLR4 grammar definition
  include/
    core/builder/               # AST construction from parse tree
    core/compiler/              # C++ code generation
    core/supports/              # Compiler utilities
    runtime/                    # Runtime library headers
    stdlib/                     # Standard library (I/O, log sources, utils)
  src/driver/main.cpp           # CLI entry point
  examples/                     # Example .ams scripts & demo log files
  CMakeLists.txt                # CMake build configuration
  install_dependency.ps1        # Dependency installer (Windows)
  ams_install_windows.ps1       # Build script (Windows)

IDE/                            # Full-Stack Platform & IDE
  server/                       # Node.js/Express backend
    src/
      index.ts                  # Express server with WebSocket/SSE
      routes/                   # API endpoints (auth, fs, rules, alerts, compile)
      services/                 # Log parser, WebSocket, sessions
      middleware/               # JWT authentication
      lib/                      # Prisma client
    prisma/schema.prisma        # Database schema
    .env_example                # Environment variable template
  client/                       # React + Vite frontend
    electron/                   # Electron shell integration
    src/
      App.tsx                   # Platform layout & routing
      components/               # UI components (Editor, AST, Dashboard)
      pages/                    # Platform pages (Projects, LogSources, Alerts)
      hooks/                    # Custom React hooks
      languages/                # AMS syntax highlighting for Monaco
  docker-compose.yml            # Docker setup for full stack
  start-dev.bat                 # Web-based dev startup
  start-electron.bat            # Electron desktop launch
```

---

## Prerequisites

| Tool | Version | Purpose |
|------|---------|---------|
| **Node.js** | >= 18.x | IDE backend & frontend |
| **npm** | >= 9.x | Package management |
| **CMake** | >= 3.20 | Compiler build system |
| **MSVC / GCC** | C++17 | Compiler compilation |
| **Java JRE** | >= 11 | ANTLR4 parser generation |
| **PowerShell** | 5.1+ | Build scripts (Windows) |
| **PostgreSQL** | >= 14 | Backend database |
| **Docker** | (Optional) | Containerized deployment |

---

## Getting Started

### Environment Setup

Copy the example environment file and configure it:

```bash
cp IDE/server/.env_example IDE/server/.env
```

Edit `IDE/server/.env` with your values:

```env
# PostgreSQL connection (used by Docker Compose)
DB_USER=postgres
DB_PASSWORD=your_password_here
DB_NAME=ams_db

# Prisma database URL
DATABASE_URL="postgresql://postgres:your_password_here@localhost:5432/ams_db?schema=public"

# Prisma engine type (required for Docker)
PRISMA_CLIENT_ENGINE_TYPE=binary

# JWT secret (IMPORTANT: change this to a unique random string in production)
JWT_SECRET="change_this_to_a_random_secret_string"

# Server port
PORT=3001

# SMTP Configuration (optional -- for email alerts)
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_SECURE=false
SMTP_USER=your_email@gmail.com
SMTP_PASS=your_app_password
SMTP_FROM="AMS Alert Bot <your_email@gmail.com>"
```

> Make sure `DB_USER`, `DB_PASSWORD`, and `DB_NAME` match the credentials in your `DATABASE_URL`.

### Docker (Recommended)

The fastest way to get the full platform running:

```bash
cd IDE

# First time (builds images and starts containers)
docker-compose up --build

# Subsequent runs
docker-compose up
```

To stop:

```bash
# Stop containers (keep images)
docker-compose stop

# Stop and remove containers
docker-compose down
```

Database records are persisted in `IDE/postgres_data/` on the host.

### Manual Setup

If you prefer running without Docker:

```bash
# 1. Start PostgreSQL locally and create the database

# 2. Install backend dependencies
cd IDE/server
npm install

# 3. Push the Prisma schema to your database
npx prisma db push

# 4. Install frontend dependencies
cd ../client
npm install
```

---

## Part 1 -- AMS-Lang Compiler

### Install Dependencies

Open **PowerShell** in the `ams-lang/` directory:

```powershell
cd ams-lang
.\install_dependency.ps1
```

This installs ANTLR4, vcpkg packages, and other build dependencies.

### Build the Compiler

```powershell
.\ams_install_windows.ps1
```

This generates the ANTLR parser from `AMS.g4`, compiles the C++ source, and produces `build/ams.exe`.

### Using the Compiler

After building, the `ams` command is available:

```powershell
# View help and available commands
ams

# Run a script in interpreted mode
ams run .\examples\hello_world.ams

# Compile a script to a native executable
ams build .\examples\logfile_demo.ams

# Run the compiled executable
.\examples\logfile_demo.exe
```

#### Available Example Scripts

| Script | Description |
|--------|-------------|
| `hello_world.ams` | Basic syntax -- variables, printing, code structure |
| `test.ams` | Comprehensive feature test -- variables, operators, sources, events, observers |
| `runtime_demo.ams` | Signal-based async scheduling -- multiple events/observers, TRACK variables, data sharing |
| `logfile_demo.ams` | Production log monitoring -- reads real log files, pattern matching, error detection |
| `live_demo/dashboard_monitor.ams` | Live dashboard integration -- real-time monitoring with log generation |

---

## Part 2 -- AMS Platform & IDE

The IDE is a full-stack platform providing a professional interface for writing, compiling, monitoring, and managing AMS-Lang scripts, available as a web app or an Electron desktop application.

### Start the IDE & Platform

**Option A: Web Mode**

Use the `start-dev.bat` script, or run manually:

```powershell
# Terminal 1 -- Backend (Port 3001)
cd IDE\server
npm run dev

# Terminal 2 -- Frontend (Port 5173)
cd IDE\client
npx vite --port 5173
```

Then open your browser to: **http://localhost:5173**

**Option B: Electron Desktop Mode**

```powershell
cd IDE
.\start-electron.bat
```

This synchronizes the Prisma schema, starts the backend, runs Vite, and launches the Electron window.

### Using the IDE

#### Editor View

- **File Explorer** (Left Sidebar) -- Browse, create, and open `.ams` files. Fully resizable.
- **Monaco Editor** (Center) -- Full AMS syntax highlighting, autocomplete, and error diagnostics.
- **AST Panel** (Right Sidebar) -- Live Abstract Syntax Tree visualization that updates as you type.
- **Output Terminal** (Bottom) -- Real-time compilation output, progress, and errors.
- **Toolbar Menu** -- Compile the active script, save, or navigate the platform.

### Platform Dashboard

Beyond the code editor, the platform provides:

- **Projects & Team Management** -- Organize monitoring projects and manage team access.
- **Log Sources Management** -- Configure log streams (Elasticsearch, local files, Splunk endpoints).
- **Rules Editor** -- No-code/low-code interface alongside `.ams` scripts.
- **Alert Routes** -- Route triggers to Slack, PagerDuty, Email, or Webhooks.
- **Real-Time Dashboard** -- View active event streams, rule hit frequencies, and server metrics.

#### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl + S` | Save current file |
| `Ctrl + Shift + B` | Compile current file |
| `Ctrl + Space` | Trigger autocomplete |

---

## AMS-Lang Syntax Guide

### Basic Structure

```
## This is a block comment ##

GLOBAL:
    INT threshold = 5
    STRING status = "ACTIVE"
    PRINTLN("System initialized")

SOURCES:
    SOURCE SensorMonitor CHECK EVERY 1000 MS
        TRACK INT reading = 0
        reading = reading + 1
        PRINTLN("Reading: ", reading)

        EVENT HighReading WHEN reading > threshold
            SIGNAL HighReading
        END
    END

OBSERVERS:
    OBSERVER AlertHandler WATCHES HighReading
        PRINTLN("ALERT: High reading detected!")
    END
```

### Log File Monitoring

```
GLOBAL:
    LOG_SOURCE logs = OPEN LOG_SOURCE "app.log" READ MODE

SOURCES:
    SOURCE LogMonitor CHECK EVERY 3000 MS
        TRACK INT error_count = 0

        SEARCH logs FOR "ERROR" INTO results

        FOR EACH record IN results
            error_count = error_count + 1
            PRINTLN("Error found: ", record.raw)
        END
    END
```

### Keywords Reference

| Category | Keywords |
|----------|----------|
| **Structure** | `GLOBAL`, `SOURCES`, `OBSERVERS` |
| **Types** | `INT`, `FLOAT`, `STRING`, `BOOL` |
| **Sources** | `SOURCE`, `CHECK EVERY`, `MS`, `TRACK` |
| **Events** | `EVENT`, `WHEN`, `SIGNAL` |
| **Observers** | `OBSERVER`, `WATCHES` |
| **Log Sources** | `LOG_SOURCE`, `OPEN`, `READ MODE`, `SEARCH`, `FOR`, `INTO` |
| **Iteration** | `FOR EACH`, `IN` |
| **Output** | `PRINT`, `PRINTLN` |
| **Flow** | `IF`, `ELSE`, `END` |
| **Operators** | `==`, `!=`, `>`, `<`, `>=`, `<=`, `AND`, `OR`, `NOT` |

---

## Example Scripts

### Hello World

```
GLOBAL:
    BOOL trigger = true
    INT X = 11
    PRINT("Hello! ")
    PRINTLN("WORLD")
```

### Production Log Monitoring

```
GLOBAL:
    LOG_SOURCE APP_LOGS = OPEN LOG_SOURCE "app.log" READ MODE

SOURCES:
    SOURCE AppLogMonitor CHECK EVERY 3000 MS
        TRACK INT check_count = 0
        TRACK INT total_errors = 0

        check_count = check_count + 1
        SEARCH APP_LOGS FOR "ERROR" INTO error_results

        FOR EACH record IN error_results
            total_errors = total_errors + 1
            PRINTLN("[ALERT] Error detected: ", record.raw)
        END
    END
```

### Signal-Based Async Monitoring

```
GLOBAL:
    PRINTLN("=== Async Scheduling Demo ===")

SOURCES:
    SOURCE TempSensor CHECK EVERY 500 MS
        TRACK INT current_temp = 20
        current_temp = current_temp + 2

        EVENT OverheatWarning WHEN current_temp > 30
            SIGNAL OverheatWarning
        END
    END

OBSERVERS:
    OBSERVER TempAlert WATCHES OverheatWarning
        PRINTLN("WARNING: Temperature exceeded threshold!")
    END
```

---

## Tech Stack

| Component | Technology |
|-----------|------------|
| **DSL Grammar** | ANTLR4 |
| **Compiler** | C++17 (MSVC / GCC) |
| **Build System** | CMake |
| **Platform Backend** | Node.js, Express, Prisma, TypeScript |
| **Platform Frontend** | React 18, Vite, TypeScript |
| **Code Editor** | Monaco Editor |
| **Charts** | Recharts |
| **Icons** | Lucide React |
| **Real-time** | WebSockets |
| **Compile Streaming** | Server-Sent Events (SSE) |
| **Desktop** | Electron |
| **Database** | PostgreSQL (via Prisma) |
| **Containerization** | Docker, Docker Compose |

---

## Contributing

See [CONTRIBUTING.md](ams-lang/CONTRIBUTING.md) for guidelines on how to contribute to this project.

---

## License

See [LICENSE](ams-lang/LICENSE) for details.
