SUBDIRS = src 

docs:
	@mkdirhier doc/HTML
	@echo "running doc++"
	@doc++ -d doc/HTML -u -nd -A -H -a -f `find src/ -name '*.h'`
	@echo "entry point is doc/HTML/index.html"
	

