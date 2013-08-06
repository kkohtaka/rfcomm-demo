CC=g++
CFLAGS=-I. -Wall -Werror -std=gnu++0x -O2
LIBS=-L/usr/lib/x86_64-linux-gnu -lstdc++ -lbluetooth

server: server.o Util.o HCI.o SDP.o RFCOMM.o
	${CC} -o $@ $< Util.o HCI.o SDP.o RFCOMM.o ${LIBS}

client: client.o Util.o HCI.o SDP.o RFCOMM.o
	${CC} -o $@ $< Util.o HCI.o SDP.o RFCOMM.o ${LIBS}

.cc.o:
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	${RM} -f *.o server client

.PHONY: clean run

