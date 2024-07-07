TARGET = mirror
SRC_DIR = ./src/
BIN_DIR = ./bin/
SRCS = $(SRC_DIR)mir.c
#SRCS += connect.c
OBJS = $(SRCS:.c=.o)
LDFLAGS = -lsqlite3 
#LDFLAGS += -lcurl
#LDFLAGS += -ljson-c
 CFLAGS =-fPIC -pie -fstack-protector-strong -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard
 CFLAGS+=-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard
 CFLAGS+= -I./lib/

all: $(TARGET) install clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	cp -af $(TARGET) $(BIN_DIR)

clean:
	rm -f $(OBJS) $(TARGET)

