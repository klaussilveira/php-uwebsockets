# Compiler and flags
CPP          = g++
CC           = cc
AR           = ar rcs
CFLAGS       = -O3 -fPIC -g
CPPFLAGS     = -fPIC -flto=auto -march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++2b -Isrc
LDFLAGS      = -shared

# PHP configuration
PHP_CONFIG   = $(shell which php-config)
PHP_INCLUDE  = $(shell $(PHP_CONFIG) --includes)
PHP_EXT_DIR  = $(shell $(PHP_CONFIG) --extension-dir)

# Extension configuration
EXT_NAME     = uwebsockets
EXT_SO       = $(EXT_NAME).so

# Vendor
USOCKETS_SRC = vendor/uSockets/src
UWS_SRC      = vendor/uWebSockets/src

# Sources
CPP_SOURCES  = uwebsockets.cpp
CPP_OBJECTS  = $(CPP_SOURCES:.cpp=.o)

# uSockets sources
USOCKETS_C_SOURCES = \
	$(USOCKETS_SRC)/bsd.c \
	$(USOCKETS_SRC)/context.c \
	$(USOCKETS_SRC)/loop.c \
	$(USOCKETS_SRC)/quic.c \
	$(USOCKETS_SRC)/socket.c \
	$(USOCKETS_SRC)/udp.c \
	$(USOCKETS_SRC)/eventing/epoll_kqueue.c \
	$(USOCKETS_SRC)/eventing/gcd.c \
	$(USOCKETS_SRC)/eventing/libuv.c \
	$(USOCKETS_SRC)/crypto/openssl.c

USOCKETS_OBJECTS = $(USOCKETS_C_SOURCES:.c=.o)

.PHONY: all clean install uninstall test

all: $(EXT_SO)

$(EXT_SO): $(CPP_OBJECTS) libusockets.a
	$(CPP) $(LDFLAGS) -o $@ $(CPP_OBJECTS) libusockets.a -lphpcpp -lssl -lcrypto -lz -lstdc++

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -I$(UWS_SRC) -I$(USOCKETS_SRC) $(PHP_INCLUDE) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I$(USOCKETS_SRC) -I$(USOCKETS_SRC)/internal -I$(USOCKETS_SRC)/eventing -I$(USOCKETS_SRC)/internal/networking -c $< -o $@

libusockets.a: $(USOCKETS_OBJECTS)
	$(AR) libusockets.a $(USOCKETS_OBJECTS)

clean:
	rm -rf $(CPP_OBJECTS) $(USOCKETS_OBJECTS) libusockets.a $(EXT_SO)

install:
	cp -f $(EXT_SO) $(PHP_EXT_DIR)/
	@echo "Installed $(EXT_SO) to $(PHP_EXT_DIR)"

uninstall:
	rm -rf $(PHP_EXT_DIR)/$(EXT_SO)
	@echo "Removed $(EXT_SO) from $(PHP_EXT_DIR)"

test:
	php -c 30-uwebsockets.ini server.php
