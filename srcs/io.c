#include "io.h"

void help()
{
	PUTS(2, "Usage: ./ft_nm [option(s)] [file(s)]");
	PUTS(2, " List symbols in [file(s)] (a.out by default).");
	PUTS(2, " The options are:");
	PUTS(2, "  -a     Display debugger-only symbols");
	PUTS(2, "  -g     Display only external symbols");
	PUTS(2, "  -p     Do not sort the symbols");
	PUTS(2, "  -r     Reverse the sense of the sort");
	PUTS(2, "  -u     Display only undefined symbols");
	PUTS(2, "nm: supported targets: x86_32, x64, object files, .so");
}

void	fd_puts(int fd, char *s) {
	if (s == NULL)
	{
		PUTS(fd, "NULL");
		return ;
	}
	ignore(write(fd, s, len(s)));
}

void	fd_pad_hex(int fd, u64 n, u32 size) {
	char	buf[size];
	u32		i = size;

	while (i--) {
		buf[i] = "0123456789abcdef"[n % 16];
		n /= 16;
	}
	ignore(write(fd, buf, size));
}

void	putc(int fd, char c)
{
	ignore(write(fd, &c, 1));
}

void	fd_fmt(int fd, char *fmt, ...)
{
	va_list			args;
	va_start(args, fmt);

	while (*fmt)
	{
		if (*fmt == '%')
		{
			switch(*++fmt)
			{
				case '\0':
					goto end;
					break ;

				case 's':
					fd_puts(fd, va_arg(args, char *));
					break ;

				case 'c':
					putc(fd, va_arg(args, int));
					break ;
			}
		}
		else
			ignore(write(fd, fmt, 1));
		++fmt;
	}

end:
	va_end(args);
}