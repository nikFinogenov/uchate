NAME  = uchat_server
FLAGS = -std=c99 -Wall -Wextra -Werror -Wpedantic
DFLAGS= #-Wno-unused-command-line-argument -Wno-unused-variable \
    -Wno-unused-function -Wno-unused-parameter \
    -fno-omit-frame-pointer
SRCD = src
INCD = inc
OBJD = obj
LMXA = ../libmx/libmx.a
LMXI = ../libmx/inc

SRCS = $(wildcard $(SRCD)/*.c)
INCS = $(addprefix $(INCD)/, $(INC))
OBJS = $(patsubst $(SRCD)/%.c, $(OBJD)/%.o, $(SRCS))

CLEAR = \r\33[2K
GREENB = \033[32;1m
YELLOWB = \033[33;1m
REDB = \033[31;1m
NORMAL = \033[0m

all: install

install: $(NAME)

$(NAME): $(OBJS)
	@clang -lcrypto -lssl -lsqlite3 -lpthread $(FLAGS) $(DFLAGS) $(OBJS) $(LMXA) -o ../$(NAME)
	@printf "${CLEAR}$@\t   ${GREENB}created${NORMAL}\n"

$(OBJD)/%.o: $(SRCD)/%.c $(INCS)
	@clang $(FLAGS) $(DFLAGS) -c $< -o $@ -I$(INCD) -I$(LMXI)
	@printf "${CLEAR}$(NAME)\t   ${YELLOWB}compile ${NORMAL}$(<:$(SRCD)/%.c=%) "

$(OBJS): | $(OBJD)

$(OBJD):
	@mkdir -p $@

clean:
	@rm -rf $(OBJD)

uninstall: clean
	@rm -rf ../$(NAME)
	@printf "$(NAME)\t   ${REDB}uninstalled${NORMAL}\n"

reinstall: uninstall install
