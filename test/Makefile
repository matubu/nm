SRCS = $(wildcard srcs/*)
ELFS += $(patsubst srcs/%,elf/%.out,$(SRCS))
ELFS += $(patsubst srcs/%,elf/%.o,$(SRCS))
ELFS += $(patsubst srcs/%,elf/%.so,$(SRCS))

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
	g++ $^ -o $@
elf/%.c.out: srcs/%.c
	gcc $^ -o $@

elf/%.cpp.o: srcs/%.cpp
	g++ $^ -c -o $@
elf/%.c.o: srcs/%.c
	gcc $^ -c -o $@

elf/%.cpp.so: srcs/%.cpp
	g++ $^ -shared -fPIC -o $@
elf/%.c.so: srcs/%.c
	gcc $^ -shared -fPIC -o $@

fclean:
	@echo "🗑  $(Red)deleting$(Eoc)  elf"
	@rm -rf elf

re: fclean elf

NORMALIZE = tr '0' ' ' | sort

test: elf
	@mkdir -p logs/ft_nm logs/nm
	@echo "-ft_nm"
	@echo "+nm"
	@for elf in $(notdir $(ELFS)); \
	do \
		echo "\n\033[1;91m>>> diff $$elf <<<\033[0m\n"; \
		../ft_nm elf/$$elf | $(NORMALIZE) | sort > ./logs/ft_nm/$$elf; \
		nm elf/$$elf | $(NORMALIZE) > ./logs/nm/$$elf; \
		diff -U0 logs/ft_nm/$$elf logs/nm/$$elf; \
	done


.PHONY: elf fclean re test