SUBDIRS = src 

docs:
	@mkdirhier doc/HTML
	@echo "running doc++"
	@doc++ -d doc/HTML -u -A -H -a -f `ls -R src/*/*.h`
	@echo "entry point is doc/HTML/index.html"
	

