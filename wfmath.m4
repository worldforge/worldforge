# Configure paths for wfmath
# Stolen by Dan Tomalesky
# Stolen by Michael Koch
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_WFMATH([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for wfmath, and define WFMATH_CFLAGS and WFMATH_LIBS
dnl
AC_DEFUN(AM_PATH_WFMATH,
[dnl 
dnl Get the cflags and libraries from the wfmath-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(wfmath-prefix,[  --with-wfmath-prefix=PREFIX
                          Prefix where wfmath is installed (optional)],
            wfmath_prefix="$withval", wfmath_prefix="")
AC_ARG_WITH(wfmath-exec-prefix,[  --with-wfmath-exec-prefix=PREFIX
                          Exec prefix where wfmath is installed (optional)],
            wfmath_exec_prefix="$withval", wfmath_exec_prefix="")
AC_ARG_ENABLE(wfmathtest, [  --disable-wfmathtest     Do not try to compile and run a test wfmath program],
		    , enable_wfmathtest=yes)

  if test x$wfmath_exec_prefix != x ; then
     wfmath_args="$wfmath_args --exec-prefix=$wfmath_exec_prefix"
     if test x${WFMATH_CONFIG+set} != xset ; then
        WFMATH_CONFIG=$wfmath_exec_prefix/bin/wfmath-config
     fi
  fi
  if test x$wfmath_prefix != x ; then
     wfmath_args="$wfmath_args --prefix=$wfmath_prefix"
     if test x${WFMATH_CONFIG+set} != xset ; then
        WFMATH_CONFIG=$wfmath_prefix/bin/wfmath-config
     fi
  fi

  AC_PATH_PROG(WFMATH_CONFIG, wfmath-config, no)
  min_wfmath_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for wfmath - version >= $min_wfmath_version)
  no_wfmath=""
  if test "$WFMATH_CONFIG" = "no" ; then
    no_wfmath=yes
  else
    WFMATH_CFLAGS=`$WFMATH_CONFIG $wfmathconf_args --cflags`
    WFMATH_LIBS=`$WFMATH_CONFIG $wfmathconf_args --libs`

    wfmath_major_version=`$WFMATH_CONFIG $wfmath_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    wfmath_minor_version=`$WFMATH_CONFIG $wfmath_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    wfmath_micro_version=`$WFMATH_CONFIG $wfmath_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_wfmathtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $WFMATH_CFLAGS"
      LIBS="$LIBS $WFMATH_LIBS"
dnl
dnl Now check if the installed wfmath is sufficiently new. (Also sanity
dnl checks the results of wfmath-config to some extent)
dnl
      rm -f conf.wfmathtest
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
  system ("touch conf.wfmathtest");
  */
  { FILE *fp = fopen("conf.wfmathtest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_wfmath_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_wfmath_version");
     exit(1);
   }

   if (($wfmath_major_version > major) ||
      (($wfmath_major_version == major) && ($wfmath_minor_version > minor)) ||
      (($wfmath_major_version == major) && ($wfmath_minor_version == minor) && ($wfmath_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'wfmath-config --version' returned %d.%d.%d, but the minimum version\n", $wfmath_major_version, $wfmath_minor_version, $wfmath_micro_version);
      printf("*** of wfmath required is %d.%d.%d. If wfmath-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If wfmath-config was wrong, set the environment variable WFMATH_CONFIG\n");
      printf("*** to point to the correct copy of wfmath-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_wfmath=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_wfmath" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$WFMATH_CONFIG" = "no" ; then
       echo "*** The wfmath-config script installed by wfmath could not be found"
       echo "*** If wfmath was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the WFMATH_CONFIG environment variable to the"
       echo "*** full path to wfmath-config."
     else
       if test -f conf.wfmathtest ; then
        :
       else
          echo "*** Could not run wfmath test program, checking why..."
          CFLAGS="$CFLAGS $WFMATH_CFLAGS"
          LIBS="$LIBS $WFMATH_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding wfmath or finding the wrong"
          echo "*** version of wfmath. If it is not finding wfmath, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means wfmath was incorrectly installed"
          echo "*** or that you have moved wfmath since it was installed. In the latter case, you"
          echo "*** may want to edit the wfmath-config script: $WFMATH_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     WFMATH_CFLAGS=""
     WFMATH_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(WFMATH_CFLAGS)
  AC_SUBST(WFMATH_LIBS)
  rm -f conf.wfmathtest
])
