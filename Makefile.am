SUBDIRS = src

EXTRA_DIST = ROADMAP Doxyfile footer.html index.dox \
             tutorial/DebugBridge.h tutorial/Makefile tutorial/index.dox \
             tutorial/message.dox tutorial/objects.dox tutorial/preamble.dox \
             tutorial/simple.dox tutorial/simple_client.cc \
             tutorial/simple_server.cc

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
