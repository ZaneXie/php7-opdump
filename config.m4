dnl $Id$
dnl config.m4 for extension opdump

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(opdump, for opdump support,
dnl Make sure that the comment is aligned:
dnl [  --with-opdump             Include opdump support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(opdump, whether to enable opdump support,
    Make sure that the comment is aligned:
[  --enable-opdump           Enable opdump support])

if test "$PHP_OPDUMP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-opdump -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/opdump.h"  # you most likely want to change this
  dnl if test -r $PHP_OPDUMP/$SEARCH_FOR; then # path given as parameter
  dnl   OPDUMP_DIR=$PHP_OPDUMP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for opdump files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       OPDUMP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$OPDUMP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the opdump distribution])
  dnl fi

  dnl # --with-opdump -> add include path
  dnl PHP_ADD_INCLUDE($OPDUMP_DIR/include)

  dnl # --with-opdump -> check for lib and symbol presence
  dnl LIBNAME=opdump # you may want to change this
  dnl LIBSYMBOL=opdump # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $OPDUMP_DIR/$PHP_LIBDIR, OPDUMP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_OPDUMPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong opdump lib version or lib not found])
  dnl ],[
  dnl   -L$OPDUMP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(OPDUMP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(opdump, opdump.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
