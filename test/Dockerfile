FROM debian
# Debian because alpine is not suited to compile in 32bit
# https://stackoverflow.com/questions/40539592/how-to-compile-a-32bit-hello-world-on-alpine

WORKDIR /data

RUN ["apt-get", "update"]
RUN ["apt-get", "upgrade"]
RUN ["apt-get", "install", "-y", "gcc", "g++", "make"]
# https://stackoverflow.com/questions/54082459/fatal-error-bits-libc-header-start-h-no-such-file-or-directory-while-compili
RUN ["apt-get", "install", "-y", "gcc-multilib", "g++-multilib"]


ENTRYPOINT ["make", "elf_linux"]