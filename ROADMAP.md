# Building an HTTPS Server From Scratch in C

## Why This Project Exists

Most developers learn how to use web servers.

Few developers learn how web servers actually work.

This project is my attempt to bridge that gap.

Instead of relying on existing frameworks and servers, I want to understand the layers beneath them by building an HTTPS server from scratch in C. My goal is not merely to create a working server, but to gain a practical understanding of:

* Computer networks
* Operating systems
* Socket programming
* HTTP
* TLS/SSL
* Concurrency
* Memory management
* Performance analysis
* Systems design

The project will be built incrementally. Every phase introduces new concepts and challenges while preserving what was learned previously.

After completing the C implementation, I intend to rebuild the entire project in Python to compare low-level and high-level approaches to systems programming.

---

# Project Philosophy

This project prioritizes:

* Understanding over speed
* Engineering over copy-pasting
* Fundamentals over frameworks

At every milestone, I should be able to explain:

* What I built
* Why it works
* Which operating system concepts are involved
* Which networking concepts are involved
* Which tradeoffs were made

---

# Timeline Assumptions

I am a university student.

Available study time:

* During semester: 8–12 hours/week
* During breaks: 15–20 hours/week

The timeline below assumes regular classes, assignments, and exams.

Expected total duration:

**8–12 months**

This is intentionally realistic.

---

# Phase 0 – Development Environment

**Duration:** 1 week

## Goal

Prepare a Linux systems programming environment.

## Learn

* GCC
* Makefiles
* Git
* gdb
* valgrind
* Linux man pages
* File descriptors

## Deliverable

A project repository with:

* Build system
* Debugging workflow
* Version control workflow

---

# Phase 1 – TCP Fundamentals

**Duration:** 2 weeks

## Goal

Build a TCP echo server and client.

## Learn

* TCP/IP
* Ports
* IP addresses
* Client-server architecture
* Sockets

## System Calls

* socket()
* bind()
* listen()
* accept()
* connect()
* recv()
* send()
* close()

## Deliverable

A client sends data.

The server receives it and sends it back.

---

# Phase 2 – Understanding the Operating System

**Duration:** 2 weeks

## Goal

Understand what the operating system does while the server runs.

## Learn

* Processes
* Threads
* Context switching
* Kernel space vs user space
* Blocking I/O
* File descriptors

## Deliverable

A written explanation of how the operating system handles a connection from a client.

---

# Phase 3 – Multi-Client TCP Server

**Duration:** 3 weeks

## Goal

Support multiple clients.

## Learn

* select()
* poll()
* Event loops
* Concurrent connections

## Deliverable

Several clients connected simultaneously.

---

# Phase 4 – HTTP Fundamentals

**Duration:** 2 weeks

## Goal

Understand HTTP as a protocol.

## Learn

* HTTP request structure
* HTTP response structure
* Status codes
* Headers
* Methods

## Deliverable

Browser successfully receives a valid HTTP response.

---

# Phase 5 – HTTP Parsing Engine

**Duration:** 3 weeks

## Goal

Convert raw HTTP text into structured data.

## Learn

* Parsing
* Buffer management
* String manipulation
* Memory allocation

## Deliverable

The server extracts:

* Method
* Path
* Headers
* Body

---

# Phase 6 – Routing Engine

**Duration:** 2 weeks

## Goal

Support multiple endpoints.

## Examples

```
/
```

```
/health
```

```
/users
```

## Learn

* Function pointers
* Dispatch tables
* Server architecture

## Deliverable

Each route produces a unique response.

---

# Phase 7 – Static File Server

**Duration:** 2 weeks

## Goal

Serve files.

## Learn

* Filesystems
* MIME types
* Path traversal attacks
* Secure file handling

## Deliverable

Serve:

* HTML
* CSS
* JavaScript
* Images

---

# Phase 8 – Threaded Server

**Duration:** 4 weeks

## Goal

Handle many users simultaneously.

## Learn

* pthreads
* Mutexes
* Race conditions
* Synchronization

## Deliverable

Thread-per-connection server.

---

# Phase 9 – Logging and Configuration

**Duration:** 2 weeks

## Goal

Make the server usable.

## Learn

* Logging systems
* Configuration files
* Error reporting

## Deliverable

Server logs requests and reads configuration from file.

---

# Phase 10 – HTTPS Using OpenSSL

**Duration:** 5 weeks

## Goal

Secure communication.

## Learn

* Cryptography basics
* Certificates
* Public/private keys
* TLS handshake
* Certificate Authorities

## OpenSSL Topics

* SSL_CTX
* SSL_new
* SSL_accept
* SSL_read
* SSL_write

## Deliverable

Browser connects through HTTPS.

---

# Phase 11 – Hardening the Server

**Duration:** 3 weeks

## Goal

Improve reliability.

## Learn

* Timeouts
* Connection limits
* Error recovery
* Defensive programming

## Deliverable

More robust server.

---

# Phase 12 – Performance Analysis

**Duration:** 3 weeks

## Goal

Measure performance.

## Learn

* Benchmarking
* Profiling
* Bottleneck analysis

## Tools

* perf
* valgrind
* gprof

## Deliverable

Performance report.

---

# Phase 13 – Python Rewrite

**Duration:** 4–6 weeks

## Goal

Rebuild everything in Python.

## Compare

### C

* Manual memory management
* Lower-level APIs
* More control

### Python

* Faster development
* Simpler APIs
* Automatic memory management

## Deliverable

Working Python implementation and comparison report.

---

# Recommended Core Reading

### Networking

📖 *Beej's Guide to Network Programming*

### Operating Systems

📖 *Operating Systems: Three Easy Pieces*

### Linux Internals

📖 *The Linux Programming Interface* — Michael Kerrisk

### HTTP

📖 RFC 9110 (HTTP Semantics)

### TLS

📖 OpenSSL Documentation

---

# Success Criteria

At the end of this project I should be able to explain:

1. How a browser reaches a server.
2. How TCP connections are established.
3. How HTTP messages are structured.
4. How operating systems manage sockets.
5. How concurrent servers work.
6. How TLS secures communication.
7. How production servers are engineered.
8. The differences between implementing networking software in C and Python.