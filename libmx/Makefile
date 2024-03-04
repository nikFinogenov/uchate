NAME = libmx.a
FLAG = -std=c11 -Wall -Wextra -Werror -Wpedantic
SRCD = src
INCD = inc
OBJD = obj

INC	 = libmx.h
INCS = $(addprefix $(INCD)/, $(INC))

SRC  = $(wildcard $(SRCD)/*.c)
OBJS = $(patsubst $(SRCD)/%.c, $(OBJD)/%.o, $(SRC))

CLEAR = \r\33[2K
GREENB = \033[32;1m
YELLOWB = \033[33;1m
REDB = \033[31;1m
NORMAL = \033[0m

all: install
	
install: $(NAME)

$(NAME): $(OBJS)
	@ar rcs $@ $^
	@printf "${CLEAR}$@\t   ${GREENB}created${NORMAL}\n"

$(OBJD)/%.o: $(SRCD)/%.c $(INCS) | $(OBJD)
	@clang $(FLAG) -c $< -o $@ -I$(INCD)
	@printf "${CLEAR}$(NAME)\t   ${YELLOWB}compile ${NORMAL}$(<:$(SRCD)/%.c=%) "

$(OBJS): | $(OBJD)

$(OBJD):
	@mkdir -p $@
	
clean:
	@rm -rf $(OBJD)

uninstall: clean
	@rm -rf $(NAME)
	@printf "$(NAME)\t   ${REDB}uninstalled${NORMAL}\n"

reinstall: uninstall install

start: 
	clang $(FLAG) main.c libmx.a && ./a.out

start_i: install
	clang $(FLAG) main.c libmx.a && ./a.out
	