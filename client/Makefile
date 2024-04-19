NAME = uchat
FLAGS = `pkg-config --cflags gtk+-3.0`
DFLAGS = -Wno-deprecated-declarations -Wno-unused-command-line-argument \
-Wno-unused-variable -Wno-unused-function -Wno-unused-parameter \
-fno-omit-frame-pointer -Wno-unused-comparison -Wno-return-type
SRCD = src
INCD = inc
OBJD = obj
LMXA = ../libmx/libmx.a
LMXI = ../libmx/inc
#LIB_JSON_DIR = ../cJSON
#LIB_JSON = ../cJSON/libcjson.a

SRCS = $(wildcard $(SRCD)/*.c)
INCS = $(wildcard $(INCD)/*.h)
OBJS = $(SRCS:$(SRCD)/%.c=$(OBJD)/%.o)

CLEAR = \r\33[2K
GREENB = \033[32;1m
YELLOWB = \033[33;1m
REDB = \033[31;1m
NORMAL = \033[0m

.PHONY: all install clean uninstall reinstall

all: install

install: $(NAME)

$(NAME): $(OBJS)
	@clang -lpthread `pkg-config --libs gtk+-3.0` $(FLAGS) $(DFLAGS) $(OBJS) $(LMXA) -o ../$(NAME)
	@printf "$(CLEAR)$@\t   $(GREENB)created$(NORMAL)\n"

$(OBJD)/%.o: $(SRCD)/%.c $(INCS) | $(OBJD)
	@clang $(FLAGS) $(DFLAGS) -c $< -o $@ -I$(INCD) -I$(LMXI)
	@printf "$(CLEAR)$(NAME)\t   $(YELLOWB)compile $(NORMAL)$(<:$(SRCD)/%.c=%) "

$(OBJD):
	@mkdir -p $@

clean:
	@rm -rf $(OBJD)

uninstall: clean

ifeq ($(wildcard ../$(NAME)),)
	@printf "$(NAME)\t   $(REDB)not installed$(NORMAL)\n"
else
	@rm -rf ../$(NAME)
	@printf "$(NAME)\t   $(REDB)uninstalled$(NORMAL)\n"
endif

reinstall: uninstall install
