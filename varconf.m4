# Configure paths for varconf
# Stolen by Michael Koch from Atlas
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_VARCONF([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Varconf, and define VARCONF_CFLAGS and VARCONF_LIBS
dnl
AC_DEFUN(AM_PATH_VARCONF,
[dnl 
dnl Get the cflags and libraries from the varconf-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(varconf-prefix,[  --with-varconf-prefix=PFX   Prefix where Varconf is installed (optional)],
            varconf_prefix="$withval", varconf_prefix="")
AC_ARG_WITH(varconf-exec-prefix,[  --with-varconf-exec-prefix=PFX Exec prefix where Varconf is installed (optional)],
            varconf_exec_prefix="$withval", varconf_exec_prefix="")
AC_ARG_ENABLE(varconftest, [  --disable-varconftest       Do not try to compile and run a test Varconf program],
		    , enable_varconftest=yes)

  if test x$varconf_exec_prefix != x ; then
     varconf_args="$varconf_args --exec-prefix=$varconf_exec_prefix"
     if test x${VARCONF_CONFIG+set} != xset ; then
        VARCONF_CONFIG=$varconf_exec_prefix/bin/varconf-config
     fi
  fi
  if test x$varconf_prefix != x ; then
     varconf_args="$varconf_args --prefix=$varconf_prefix"
     if test x${VARCONF_CONFIG+set} != xset ; then
        VARCONF_CONFIG=$varconf_prefix/bin/varconf-config
     fi
  fi

  AC_PATH_PROG(VARCONF_CONFIG, varconf-config, no)
  min_varconf_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for Varconf - version >= $min_varconf_version)
  no_varconf=""
  if test "$VARCONF_CONFIG" = "no" ; then
    no_varconf=yes
  else
    VARCONF_CFLAGS=`$VARCONF_CONFIG $varconfconf_args --cflags`
    VARCONF_LIBS=`$VARCONF_CONFIG $varconfconf_args --libs`

    varconf_major_version=`$VARCONF_CONFIG $varconf_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    varconf_minor_version=`$VARCONF_CONFIG $varconf_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    varconf_micro_version=`$VARCONF_CONFIG $varconf_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_varconftest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $VARCONF_CFLAGS"
      LIBS="$LIBS $VARCONF_LIBS"
dnl
dnl Now check if the installed Varconf is sufficiently new. (Also sanity
dnl checks the results of varconf-config to some extent)
dnl
      rm -f conf.varconftest
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
  system ("touch conf.varconftest");
  */
  { FILE *fp = fopen("conf.varconftest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_varconf_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_varconf_version");
     exit(1);
   }

   if (($varconf_major_version > major) ||
      (($varconf_major_version == major) && ($varconf_minor_version > minor)) ||
      (($varconf_major_version == major) && ($varconf_minor_version == minor) && ($varconf_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'varconf-config --version' returned %d.%d.%d, but the minimum version\n", $varconf_major_version, $varconf_minor_version, $varconf_micro_version);
      printf("*** of Varconf required is %d.%d.%d. If varconf-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If varconf-config was wrong, set the environment variable VARCONF_CONFIG\n");
      printf("*** to point to the correct copy of varconf-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_varconf=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_varconf" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$VARCONF_CONFIG" = "no" ; then
       echo "*** The varconf-config script installed by Varconf could not be found"
       echo "*** If Varconf was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the VARCONF_CONFIG environment variable to the"
       echo "*** full path to varconf-config."
     else
       if test -f conf.varconftest ; then
        :
       else
          echo "*** Could not run Varconf test program, checking why..."
          CFLAGS="$CFLAGS $VARCONF_CFLAGS"
          LIBS="$LIBS $VARCONF_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Varconf or finding the wrong"
          echo "*** version of Varconf. If it is not finding Varconf, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Varconf was incorrectly installed"
          echo "*** or that you have moved Varconf since it was installed. In the latter case, you"
          echo "*** may want to edit the varconf-config script: $VARCONF_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     VARCONF_CFLAGS=""
     VARCONF_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(VARCONF_CFLAGS)
  AC_SUBST(VARCONF_LIBS)
  rm -f conf.varconftest
])
