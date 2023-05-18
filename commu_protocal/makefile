CC = gcc
CFLAGS = -Wall -Werror -I./header -I/usr/local/openssl_1_1_1/include -L/usr/local/openssl_1_1_1/lib 
LDFLAGS = -lm

SOURCES = ./source/CP.c
OBJECTS = $(SOURCES:.c=.o)

TARGET = communication_protocol

$(TARGET): $(OBJECTS)
	export PATH="/usr/local/openssl_1_1_1j/bin:$PATH" LD_LIBRARY_PATH="/usr/local/openssl_1_1_1j/lib:$LD_LIBRARY_PATH"
	export PKG_CONFIG_PATH=/usr/local/openssl_1_1_1j/lib/pkgconfig
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@  -lz -lssl -lcrypto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lz -lssl -lcrypto

clean:
	rm -f $(OBJECTS) $(TARGET)
