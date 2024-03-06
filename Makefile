main: shell.o command.o tokens.o shell.h
	cc -o main shell.o command.o tokens.o
shell.o: shell.c
	cc -c shell.c
command.o: command.c
	cc -c command.c
tokens.o: tokens.c
	cc -c tokens.c
clean:
	rm main shell.o command.o tokens.o
