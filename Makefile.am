SUBDIRS = src 

docs:
	@mkdirhier doc/HTML
	@echo "running doc++"
	@doc++ -ddoc/HTML -u --no-define -A -H -a -f `ls -R src/*/*.h`
	@echo "entry point is doc/HTML/index.html"
	

