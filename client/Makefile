export DISPLAY=:0

NAME = uchat
CC = clang
CFLAGS = -pthread `pkg-config --cflags gtk+-3.0` -Wno-deprecated-declarations# -rdynamic -lm
LIBS = -lsqlite3 `pkg-config --libs gtk+-3.0`
INC = inc
SRCDIR = src
OBJDIR = obj
LMXA = ../libmx/libmx.a
LMXI = ../libmx/inc


SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(LMXA) -o ../$(NAME) $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC) -I$(LMXI)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) ../$(NAME)

.PHONY: all clean

reinstall:
	make clean
	make
