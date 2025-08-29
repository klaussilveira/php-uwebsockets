# PHP uWebSockets Extension

This project is a native PHP extension that provides a fast and lightweight HTTP
server using [uWebSockets](https://github.com/uNetworking/uWebSockets). It
allows PHP developers to define HTTP routes and handle requests directly in PHP
using a high-performance C++ server backend.

The main goal of this extension is to combine the performance of uWebSockets
with the simplicity of PHP for request handling. Developers can register routes
and write request/response logic in PHP, while leveraging a compiled
event-driven HTTP server underneath.

## Features

- Route-based HTTP handling
- Access to HTTP request information from PHP
- Ability to write headers, status, and body to the response from PHP
- Built by default with io_uring support

## Dependencies

- [PHP-CPP](https://www.php-cpp.com/)
- PHP development headers (`php-dev`, `phpize`, etc.)

The `uWebSockets` and `uSockets` libraries are vendored.

## Building

To build the extension:

    $ make

This will produce a shared object file (`.so`) that can be loaded into PHP.

Ensure that the PHP-CPP headers and libraries are available to your compiler.
You may need to set `PHP_INCLUDE` or modify the `Makefile` to match
your environment.

## Example

```php
<?php

$server = new UServer();

$server->get('/', function ($req, $res) {
    $res->setHeader('Content-Type', 'application/json');

    return json_encode(['hello' => 'world']);
});

$server->listen(3000)->run();
```

## Classes

### UServer

* `get(string $path, callable $handler)`: Register a GET route
* `listen(int $port)`: Start listening on the given port
* `run()`: Start the event loop

### HttpRequest

* `getUrl()`
* `getMethod()`
* `getQuery()`
* `getHeader(string $name)`

### HttpResponse

* `write(string $chunk)`
* `setStatus(string $statusLine)`
* `setHeader(string $key, string $value)`
