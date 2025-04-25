VERSION := $(shell cat VERSION)
CC = cc

SOURCEVIEW = gtksourceview-4
SOURCEVIEWARG =

ifeq ($(WITHOUTSOURCEVIEW),1)
	SOURCEVIEW =
	SOURCEVIEWARG = -DWITHOUTSOURCEVIEW
endif

CFLAGS = `pkg-config --cflags gtk+-3.0 $(SOURCEVIEW)` -Dpver=\"$(VERSION)\" $(SOURCEVIEWARG)
LIBS = `pkg-config --libs gtk+-3.0 $(SOURCEVIEW)`
SRC = events.c fmanip.c img.c settings.c cfgmgr.c editor.c mainwindow.c sgnotes.c 
OBJ = $(SRC:.c=.o)
EXE = sgnotes

PREFIX = /usr
BIN_DIR = $(PREFIX)/bin
APP_DIR = $(PREFIX)/share/applications
ICON_DIR = $(PREFIX)/share/icons/hicolor/64x64/apps/

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

debug:
	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)

test:
	./$(EXE)

install:
	install -Dm755 $(EXE) $(BIN_DIR)/$(EXE)
	install -Dm644 $(EXE).desktop $(APP_DIR)/$(EXE).desktop
	install -Dm644 icons/$(EXE).svg $(ICON_DIR)/$(EXE).svg

uninstall:
	rm -f $(BIN_DIR)/$(EXE)
	rm -f $(APP_DIR)/$(EXE).desktop
	rm -f $(ICON_DIR)/$(EXE).svg

clean:
	rm -f $(OBJ) $(EXE) debug
