VERSION := $(shell cat VERSION)
CC = cc

SOURCEVIEW = gtksourceview-4
SOURCEVIEWARG =

ifeq ($(WITHOUTSOURCEVIEW),1)
	SOURCEVIEW =
	SOURCEVIEWARG = -DWITHOUTSOURCEVIEW
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	CFLAGS = `pkg-config --cflags gtk+-3.0 $(SOURCEVIEW)` -Dpver=\"$(VERSION)\" $(SOURCEVIEWARG)
	PICON =
else
	CFLAGS = `pkg-config --cflags gtk+-3.0 $(SOURCEVIEW)` -Dpver=\"$(VERSION)\" $(SOURCEVIEWARG) -mwindows -O2
	PICON = programicon.o
endif

LIBS = `pkg-config --libs gtk+-3.0 $(SOURCEVIEW)`
SRC = events.c fmanip.c img.c settings.c cfgmgr.c editor.c mainwindow.c sgnotes.c $(PICON)
OBJ = $(SRC:.c=.o)
EXE = sgnotes

PREFIX = /usr
BIN_DIR = $(PREFIX)/bin
APP_DIR = $(PREFIX)/share/applications
ICON_DIR = $(PREFIX)/share/icons/hicolor/64x64/apps/

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

ifeq ($(OS),Windows_NT)
$(EXE): programicon.o
programicon.o: programicon.rc
	windres programicon.rc -O coff -o programicon.o
endif

debug:
	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)

test:
	./$(EXE)

install:
	install -Dm755 $(EXE) $(BIN_DIR)/$(EXE)
	install -Dm644 $(EXE).desktop $(APP_DIR)/$(EXE).desktop
	install -Dm644 share/icons/hicolor/scalable/apps/$(EXE).svg $(ICON_DIR)/$(EXE).svg
	install -Dm644 share/gtksourceview-4/styles/zarium.xml $(PREFIX)/share/gtksourceview-4/styles/zarium.xml

uninstall:
	rm -f $(BIN_DIR)/$(EXE)
	rm -f $(APP_DIR)/$(EXE).desktop
	rm -f $(ICON_DIR)/$(EXE).svg
	rm -f $(PREFIX)/share/gtksourceview-4/styles/zarium.xml

clean:
	rm -f $(OBJ) $(EXE) debug
