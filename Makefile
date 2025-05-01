CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = my_shell

SRCS = Cykor_week2_main.c Cykor_week2_parser.c Cykor_week2_executor.c Cykor_week2_external_command.c Cykor_week2_internal_command.c 
OBJS = $(SRCS:.c=.o)
HEADERS = Cykor_week2_parser.h Cykor_week2_executor.h Cykor_week2_external_command.h Cykor_week2_internal_command.h 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
