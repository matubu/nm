SRCS = $(wildcard srcs/*)
ELFS = $(patsubst srcs/%,elf/%.out,$(SRCS))

elf: $(SRCS)
	@mkdir -p elf
	@if [ $$(uname -s) = 'Linux' ]; \
		then \
			echo 🐧 Linux mode; \
			make elf_linux; \
		else \
			echo 🐳 Docker mode; \
			make elf_docker; \
	fi

elf_linux: $(ELFS)

elf_docker:
	docker compose up --build

elf/%.cpp.out: srcs/%.cpp
	@echo "🗜  $(Green)Creating$(Eoc) $@"
	@g++ $^ -o $@
elf/%.c.out: srcs/%.c
	@echo "🗜  $(Green)Creating$(Eoc) $@"
	@gcc $^ -o $@

fclean:
	@echo "🗑  $(Red)Deleting$(Eoc)  elf"
	@rm -rf elf

re: fclean all

test: elf
	@mkdir -p logs/ft_nm logs/nm
	@echo "< ft_nm"
	@echo "> nm"
	@for elf in $(notdir $(ELFS)); \
	do \
		echo "\n\033[1;91m>>> diff $$elf <<<\033[0m\n"; \
		../ft_nm elf/$$elf | sort > ./logs/ft_nm/$$elf; \
		nm elf/$$elf | sort > ./logs/nm/$$elf; \
		diff --color=always -u0 logs/ft_nm/$$elf logs/nm/$$elf; \
	done


.PHONY: all fclean re test