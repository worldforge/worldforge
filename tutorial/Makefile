CXX=g++
CXXFLAGS=-Wall -g -I/usr/local/include -D_REENTRANT -D_THREAD_SAFE

CLIENT_LFLAGS = -L/usr/local/lib -lAtlas -lAtlasNet -pthread -lccxx
CLIENT_OBJECTS = simple_client.o

SERVER_LFLAGS = -L/usr/local/lib -lAtlas -lAtlasNet -pthread -lccxx
SERVER_OBJECTS = simple_server.o

all: simple_client simple_server

simple_client: $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o simple_client $(CLIENT_OBJECTS) $(CLIENT_LFLAGS)

simple_server: $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o simple_server $(SERVER_OBJECTS) $(SERVER_LFLAGS)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o
	rm -f simple_client simple_server
