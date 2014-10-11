GCC = nspire-gcc
LD = nspire-ld-bflt
GCCFLAGS = -Os -Wall -Wextra -marm -nostdlib
LDFLAGS =
EXE = nTxt20.tns
OBJS = texteditor.o input.o output.o utils.o menu.o fileio.o navigation.o filebrowser.o actions.o editactions.o history.o undoredo.o
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $(LDFLAGS) $^ -o $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
