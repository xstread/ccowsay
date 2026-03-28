CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = ccowsay
SOURCE = main.c
PREFIX = /usr/local
BINDIR = /usr/bin
SHAREDIR = $(PREFIX)/share/ccowsay

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	ln -sf $(TARGET) $(BINDIR)/ccowthink
	install -d $(SHAREDIR)/cows
	cp -r cows/* $(SHAREDIR)/cows

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(BINDIR)/ccowthink
	rm -rf $(SHAREDIR)

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
