SUBDIRS = src tests tutorial benchmark

bin_SCRIPTS = atlas-config

EXTRA_DIST = ROADMAP Doxyfile footer.html index.dox

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
