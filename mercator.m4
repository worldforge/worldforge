# Configure paths for Mercator
# Adapted by Al Riddoch from Eris-C++
# Adapted by Michael Kocg from Eris-C++
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_MERCATOR([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Mercator, and define MERCATOR_CFLAGS and MERCATOR_LIBS
dnl
AC_DEFUN(AM_PATH_MERCATOR,
[dnl 
dnl Get the cflags and libraries from the mercator-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(mercator-prefix,[  --with-mercator-prefix=PREFIX
                          Prefix where Mercator is installed (optional)],
            mercator_prefix="$withval", mercator_prefix="")
AC_ARG_WITH(mercator-exec-prefix,[  --with-mercator-exec-prefix=PREFIX
                          Exec prefix where Mercator is installed (optional)],
            mercator_exec_prefix="$withval", mercator_exec_prefix="")
AC_ARG_ENABLE(mercatortest, [  --disable-mercatortest     Do not try to compile and run a test Mercator program],
		    , enable_mercatortest=yes)

  if test x$mercator_exec_prefix != x ; then
     mercator_args="$mercator_args --exec-prefix=$mercator_exec_prefix"
     if test x${MERCATOR_CONFIG+set} != xset ; then
        MERCATOR_CONFIG=$mercator_exec_prefix/bin/mercator-config
     fi
  fi
  if test x$mercator_prefix != x ; then
     mercator_args="$mercator_args --prefix=$mercator_prefix"
     if test x${MERCATOR_CONFIG+set} != xset ; then
        MERCATOR_CONFIG=$mercator_prefix/bin/mercator-config
     fi
  fi

  AC_PATH_PROG(MERCATOR_CONFIG, mercator-config, no)
  min_mercator_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for Mercator - version >= $min_mercator_version)
  no_mercator=""
  if test "$MERCATOR_CONFIG" = "no" ; then
    no_mercator=yes
  else
    MERCATOR_CFLAGS=`$MERCATOR_CONFIG $mercatorconf_args --cflags`
    MERCATOR_LIBS=`$MERCATOR_CONFIG $mercatorconf_args --libs`

    mercator_major_version=`$MERCATOR_CONFIG $mercator_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    mercator_minor_version=`$MERCATOR_CONFIG $mercator_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    mercator_micro_version=`$MERCATOR_CONFIG $mercator_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_mercatortest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $MERCATOR_CFLAGS"
      LIBS="$LIBS $MERCATOR_LIBS"
dnl
dnl Now check if the installed Mercator is sufficiently new. (Also sanity
dnl checks the results of mercator-config to some extent)
dnl
      rm -f conf.mercatortest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.mercatortest");
  */
  { FILE *fp = fopen("conf.mercatortest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_mercator_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_mercator_version");
     exit(1);
   }

   if (($mercator_major_version > major) ||
      (($mercator_major_version == major) && ($mercator_minor_version > minor)) ||
      (($mercator_major_version == major) && ($mercator_minor_version == minor) && ($mercator_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'mercator-config --version' returned %d.%d.%d, but the minimum version\n", $mercator_major_version, $mercator_minor_version, $mercator_micro_version);
      printf("*** of Mercator required is %d.%d.%d. If mercator-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If mercator-config was wrong, set the environment variable MERCATOR_CONFIG\n");
      printf("*** to point to the correct copy of mercator-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_mercator=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_mercator" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$MERCATOR_CONFIG" = "no" ; then
       echo "*** The mercator-config script installed by Mercator could not be found"
       echo "*** If Mercator was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the MERCATOR_CONFIG environment variable to the"
       echo "*** full path to mercator-config."
     else
       if test -f conf.mercatortest ; then
        :
       else
          echo "*** Could not run Mercator test program, checking why..."
          CFLAGS="$CFLAGS $MERCATOR_CFLAGS"
          LIBS="$LIBS $MERCATOR_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Mercator or finding the wrong"
          echo "*** version of Mercator. If it is not finding Mercator, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Mercator was incorrectly installed"
          echo "*** or that you have moved Mercator since it was installed. In the latter case, you"
          echo "*** may want to edit the mercator-config script: $MERCATOR_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     MERCATOR_CFLAGS=""
     MERCATOR_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(MERCATOR_CFLAGS)
  AC_SUBST(MERCATOR_LIBS)
  rm -f conf.mercatortest
])
