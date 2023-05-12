CC = gcc
CFLAGS = -Wall -Werror -I./header
LDFLAGS = -lm

SOURCES = ./source/cJSON.c ./source/config.c ./source/select_questions.c ./source/server.c ./source/testCode.c ./source/grading.c
OBJECTS = $(SOURCES:.c=.o)

TARGET = qbserver

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@  -lz
	rm -f $(OBJECTS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
