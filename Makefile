#!/usr/bin/make -f
# skeleton-libpurple - A skeleton for developing libpurple plugins
# Copyright (C) 2017  Gary Kramlich <grim@reaperworld.com>
# Copyright (C) 2016-2017  Eion Robb <eion@robbmob.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

PLUGIN_NAME = noircwho

ifeq ($(OS),Windows_NT)
PLUGIN_TARGET := $(PLUGIN_NAME).dll
else
PLUGIN_TARGET := $(PLUGIN_NAME).so
endif

PLUGIN_OBJECTS := $(PLUGIN_SOURCES:%.c=%.o)

CC ?= gcc
CFLAGS ?= -O2 -g -g3
PKG_CONFIG ?= pkg-config

LIBPURPLE_PC_MODULE ?= purple
LIBPURPLE_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(LIBPURPLE_PC_MODULE))
LIBPURPLE_LIBS := $(shell $(PKG_CONFIG) --libs $(LIBPURPLE_PC_MODULE))
LIBPURPLE_PLUGIN_DIR := $(shell $(PKG_CONFIG) --variable=plugindir $(LIBPURPLE_PC_MODULE))
LIBPURPLE_DATA_DIR := $(shell $(PKG_CONFIG) --variable=datadir $(LIBPURPLE_PC_MODULE))
LIBPURPLE_LIB_DIR := $(shell $(PKG_CONFIG) --variable=libdir $(LIBPURPLE_PC_MODULE))

ADDITIONAL_CFLAGS := \
	-DDATADIR=$(LIBPURPLE_DATA_DIR) \
	-DLIBDIR=$(LIBPURPLE_LIB_DIR) \
	-DPIXMAPSDIR=$(LIBPURPLE_DATA_DIR)/pixmaps/pidgin

.PHONY: all clean install uninstall

all: $(PLUGIN_TARGET)

clean:
	rm -f $(PLUGIN_OBJECTS) $(PLUGIN_TARGET)

install: $(PLUGIN_TARGET)
	install -D $(PLUGIN_TARGET) $(LIBPURPLE_PLUGIN_DIR)/$(PLUGIN_TARGET)

uninstall:
	rm -f $(LIBPURPLE_PLUGIN_DIR)/$(PLUGIN_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(ADDITIONAL_CFLAGS) $(LIBPURPLE_CFLAGS) $(PLUGIN_CFLAGS) -c -o $@ $<

$(PLUGIN_TARGET): $(PLUGIN_OBJECTS) $(PLUGIN_HEADERS) $(MAKEFILE_LIST)
	$(CC) -shared $(LIBS) $(LIBPURPLE_LIBS) $(PLUGIN_LIBS) -o $@ $(PLUGIN_OBJECTS)