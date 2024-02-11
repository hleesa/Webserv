# WebServer Project

## Introduction
This project involves creating a web server similar to NGINX, using C++ 98 standards. The server, named 'webserv', is designed to handle HTTP requests and serve content over the web.

## Features
- Fully functional HTTP server in C++ 98.
- Handles HTTP requests including GET, POST, and DELETE methods.
- Supports file uploads.
- Non-blocking I/O operations using `poll()` or equivalent.
- Compatible with various web browsers.
- Configuration file for server setup.
- Default error pages.
- Stress-tested for robust performance.

## Requirements
- C++ 98 compliance.
- No external libraries allowed.
- Must include a Makefile with specific rules.
- Server must handle configuration files and serve static websites.

## Configuration
- Configuration options include setting port and host for each server, server_names, error pages, client body size limits, and route-specific settings.
- Supports CGI execution for file extensions like .php.

## Installation
- Compile the source using the provided Makefile.
- Run `./webserv [configuration file]` to start the server.

## Testing
- Ensure compatibility with selected web browsers.
- Use the provided tester or create custom tests in convenient languages like Python or Golang.
