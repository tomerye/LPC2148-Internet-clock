CFLAGS=$(shell pkg-config --cflags libgcal)
LDLIBS=$(shell pkg-config --libs libgcal)

all: Dispatcher

clean:
	rm -f *.o
	rm -f Dispatcher GCAccount GCEvent

Dispatcher: Dispatcher.o GCAccount.o GCEvent.o
	g++ -lpthread -o Dispatcher Dispatcher.o GCAccount.o GCEvent.o $(CFLAGS) $(LDLIBS)

Dispatcher.o: GoogleCalender2.cpp
	g++ -c GoogleCalender2.cpp -o Dispatcher.o $(CFLAGS) $(LDLIBS)

GCAccount.o: GCAccount.cpp
	g++ -lpthread -c GCAccount.cpp -o GCAccount.o $(CFLAGS) $(LDLIBS)

GCEvent.o: GCEvent.cpp
	g++ -c GCEvent.cpp -o GCEvent.o $(CFLAGS) $(LDLIBS)



