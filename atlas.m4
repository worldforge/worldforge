# Configure paths for Atlas
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_ATLAS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Atlas, and define ATLAS_CFLAGS and ATLAS_LIBS
dnl
AC_DEFUN(AM_PATH_ATLAS,
[dnl 
dnl Get the cflags and libraries from the atlas-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(atlas-prefix,[  --with-atlas-prefix=PFX   Prefix where Atlas is installed (optional)],
            atlas_prefix="$withval", atlas_prefix="")
AC_ARG_WITH(atlas-exec-prefix,[  --with-atlas-exec-prefix=PFX Exec prefix where Atlas is installed (optional)],
            atlas_exec_prefix="$withval", atlas_exec_prefix="")
AC_ARG_ENABLE(atlastest, [  --disable-atlastest       Do not try to compile and run a test Atlas program],
		    , enable_atlastest=yes)

  if test x$atlas_exec_prefix != x ; then
     atlas_args="$atlas_args --exec-prefix=$atlas_exec_prefix"
     if test x${ATLAS_CONFIG+set} != xset ; then
        ATLAS_CONFIG=$atlas_exec_prefix/bin/atlas-config
     fi
  fi
  if test x$atlas_prefix != x ; then
     atlas_args="$atlas_args --prefix=$atlas_prefix"
     if test x${ATLAS_CONFIG+set} != xset ; then
        ATLAS_CONFIG=$atlas_prefix/bin/atlas-config
     fi
  fi

  AC_PATH_PROG(ATLAS_CONFIG, atlas-config, no)
  min_atlas_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for Atlas - version >= $min_atlas_version)
  no_atlas=""
  if test "$ATLAS_CONFIG" = "no" ; then
    no_atlas=yes
  else
    ATLAS_CFLAGS=`$ATLAS_CONFIG $atlasconf_args --cflags`
    ATLAS_LIBS=`$ATLAS_CONFIG $atlasconf_args --libs`

    atlas_major_version=`$ATLAS_CONFIG $atlas_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    atlas_minor_version=`$ATLAS_CONFIG $atlas_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    atlas_micro_version=`$ATLAS_CONFIG $atlas_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_atlastest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $ATLAS_CFLAGS"
      LIBS="$LIBS $ATLAS_LIBS"
dnl
dnl Now check if the installed Atlas is sufficiently new. (Also sanity
dnl checks the results of atlas-config to some extent)
dnl
      rm -f conf.atlastest
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
  system ("touch conf.atlastest");
  */
  { FILE *fp = fopen("conf.atlastest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_atlas_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_atlas_version");
     exit(1);
   }

   if (($atlas_major_version > major) ||
      (($atlas_major_version == major) && ($atlas_minor_version > minor)) ||
      (($atlas_major_version == major) && ($atlas_minor_version == minor) && ($atlas_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'atlas-config --version' returned %d.%d.%d, but the minimum version\n", $atlas_major_version, $atlas_minor_version, $atlas_micro_version);
      printf("*** of Atlas required is %d.%d.%d. If atlas-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If atlas-config was wrong, set the environment variable ATLAS_CONFIG\n");
      printf("*** to point to the correct copy of atlas-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_atlas=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_atlas" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ATLAS_CONFIG" = "no" ; then
       echo "*** The atlas-config script installed by Atlas could not be found"
       echo "*** If Atlas was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ATLAS_CONFIG environment variable to the"
       echo "*** full path to atlas-config."
     else
       if test -f conf.atlastest ; then
        :
       else
          echo "*** Could not run Atlas test program, checking why..."
          CFLAGS="$CFLAGS $ATLAS_CFLAGS"
          LIBS="$LIBS $ATLAS_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Atlas or finding the wrong"
          echo "*** version of Atlas. If it is not finding Atlas, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Atlas was incorrectly installed"
          echo "*** or that you have moved Atlas since it was installed. In the latter case, you"
          echo "*** may want to edit the atlas-config script: $ATLAS_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ATLAS_CFLAGS=""
     ATLAS_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(ATLAS_CFLAGS)
  AC_SUBST(ATLAS_LIBS)
  rm -f conf.atlastest
])
