# Wedit Makefile for project test_tmalloc
SRCDIR=d:\lcc\projects\a_projects5\tmalloc
CFLAGS=-Id:\lcc\include 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	tcheck.obj \
	test_tmalloc.obj \
	tmalloc.obj

LIBS=
EXE=test_tmalloc.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\test_tmalloc.exe $(OBJS) $(LIBS)

# Build tcheck.c
TCHECK_C=\
	$(SRCDIR)\tmalloc.h\

tcheck.obj: $(TCHECK_C) $(SRCDIR)\tcheck.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tcheck.c

# Build test_tmalloc.c
TEST_TMALLOC_C=\
	$(SRCDIR)\tmalloc.h\
	$(SRCDIR)\tcheck.h\

test_tmalloc.obj: $(TEST_TMALLOC_C) $(SRCDIR)\test_tmalloc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\test_tmalloc.c

# Build tmalloc.c
TMALLOC_C=\

tmalloc.obj: $(TMALLOC_C) $(SRCDIR)\tmalloc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tmalloc.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\test_tmalloc.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects5\tmalloc\lcc\test_tmalloc.exe
