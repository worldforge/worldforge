# Configure paths for libsic++
# by Karsten Laux, 12/07/2000
# stolen from Sam Lantinga 
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor
 
 
dnl AM_PATH_SIGC([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SIGC, and define SIGC_CFLAGS and SIGC_LIBS
dnl
AC_DEFUN(AM_PATH_SIGC,
[dnl
dnl Get the cflags and libraries from the sigc-config script
dnl
AC_ARG_WITH(sigc-prefix,[  --with-sigc-prefix=PFX   Prefix where libsigc++ is installed (optional)],
            sigc_prefix="$withval", sigc_prefix="")
AC_ARG_WITH(sigc-exec-prefix,[  --with-sigc-exec-prefix=PFX Exec prefix where libsigc++ is installed (optional)],
            sigc_exec_prefix="$withval", sigc_exec_prefix="")          
  if test x$sigc_exec_prefix != x ; then
     sigc_args="$sigc_args --exec-prefix=$sigc_exec_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_exec_prefix/bin/sigc-config
     fi
  fi
  if test x$sigc_prefix != x ; then
     sigc_args="$sigc_args --prefix=$sigc_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_prefix/bin/sigc-config
     fi
  fi
 
  AC_PATH_PROG(SIGC_CONFIG, sigc-config, no)
  min_sigc_version=ifelse([$1], ,0.8.5,$1)
  AC_MSG_CHECKING(for libsigc++ - version >= $min_sigc_version)
  no_sigc=""
  if test "$SIGC_CONFIG" = "no" ; then
    no_sigc=yes
    AC_MSG_RESULT(no)
  else
    SIGC_CFLAGS=`$SIGC_CONFIG $sigcconf_args --cflags`
    SIGC_LIBS=`$SIGC_CONFIG $sigcconf_args --libs`
 
    sigc_major_version=`$SIGC_CONFIG $sigc_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sigc_minor_version=`$SIGC_CONFIG $sigc_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sigc_micro_version=`$SIGC_CONFIG $sigc_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $SIGC_CFLAGS"
    LIBS="$LIBS $SIGC_LIBS"
 
    rm -f conf.sigctest
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
      new_str = malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
 
  return new_str;
}    
int main ()
{
  int major, minor, micro;
  char *tmp_version;
 
  system ("touch conf.sigctest");
 
  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sigc_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sigc_version");
     exit(1);
   }
 
   if (($sigc_major_version > major) ||
      (($sigc_major_version == major) && ($sigc_minor_version > minor)) ||
      (($sigc_major_version == major) && ($sigc_minor_version == minor) && ($sigc_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sigc-config --version' returned %d.%d.%d, but the minimum version\n", $sigc_major_version, $sigc_minor_version, $sigc_micro_version);
      printf("*** of libsigc required is %d.%d.%d.\n", major, minor, micro);
      printf("*** If sigc-config was wrong, set the environment variable SIGC_CONFIG\n");
      printf("*** to point to the correct copy of sigc-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}
 
],,no_sigc=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
 
    CFLAGS="$ac_save_CFLAGS"
    LIBS="$ac_save_LIBS"
    if test "x$no_sigc" = x ; then
      AC_MSG_RESULT(yes)
      ifelse([$2], , :, [$2])
    else
      AC_MSG_RESULT(no)
      ifelse([$3], , :, [$3])
    fi
   fi
  AC_SUBST(SIGC_CFLAGS)
  AC_SUBST(SIGC_LIBS)
 
]) 