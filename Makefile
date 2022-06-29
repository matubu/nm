Name = ft_nm
Srcs = $(wildcard srcs/*.c)
Deps = $(wildcard incs/*.h) Makefile
Objs = $(Srcs:srcs/%.c=bin/%.o)

Incs = -I incs
Flag = -Wall -Wextra -Werror $(Incs) -O3

Red = \033[0;1;91m
Green = \033[0;1;92m
Eoc = \033[0m

Args = elf

all: $(Name)

run: all elf
	@echo "🖥  $(Green)Launching$(Eoc) ./$(Name) $(Args)"
	@./$(Name) $(Args)

bin/%.o: srcs/%.c $(Deps)
	@echo "🔧 $(Green)Compiling$(Eoc) $(notdir $<)"
	@mkdir -p bin
	@gcc $(Flag) -c $< -o $@

$(Name): $(Objs)
	@echo "🎯 $(Green)Compiling$(Eoc) $@"
	@gcc $(Flag) $^ -o $@

clean:
	@echo "🗑  $(Red)Deleting$(Eoc)  binaries"
	@rm -rf bin

fclean: clean
	@echo "🗑  $(Red)Deleting$(Eoc)  $(Name)"
	@rm -rf $(Name)

re: fclean all

.PHONY: all run clean fclean re

elf_linux:
	@echo 🐧 Linux mode
	@gcc test/elf.c -o elf
	@echo Successfully created elf
	
elf_docker:
	@echo 🐳 Docker mode
	@docker compose up --build

elf:
	@echo "🗜  $(Green)Creating$(Eoc) elf file"
	@if [ $$(uname -s) = 'Linux' ]; \
		then make elf_linux; \
		else make elf_docker; \
	fi

.PHONY: elf_docker elf_linux