export DISPLAY=:0

NAME = uchat
CC = clang
CFLAGS = -pthread `pkg-config --cflags gtk+-3.0` -Wno-deprecated-declarations# -rdynamic -lm
LIBS = -lsqlite3 `pkg-config --libs gtk+-3.0`
INC = inc
SRCDIR = src
OBJDIR = obj

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(NAME)

.PHONY: all clean

reinstall:
	make clean
	make
