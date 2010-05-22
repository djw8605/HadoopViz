#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.61)
AC_INIT(HadoopViz, 1.0, dweitzel@cse.unl.edu)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PATH_SDL([1.2.10])
AC_CHECK_PROGS([sdl], [sdl-config], [:])
if test "$sdl" = :; then
  AC_MSG_ERROR([HadoopViz needs sdl-config in the path])
fi
AC_CHECK_FT2
AC_CHECK_PROGS([freetype], [freetype-config], [:])
if test "$sdl" = :; then
  AC_MSG_ERROR([HadoopViz needs freetype-config in the path])
fi

MAC_LIBS=""
if test "$target_vendor" = "apple"; then
  AC_DEFINE([IS_MAC], [], [Whether operating system is an Apple])
  MAC_LIBS="-framework OpenGL"
fi
AC_SUBST(MAC_LIBS)

AC_CHECK_HEADER(SDL/SDL_opengl.h, ,
	AC_MSG_ERROR([SDL OpenGL header not found. Please install SDL with OpenGL support.]))
AC_CHECK_LIB(GL, main,
	OPENGL_LIBS="-lGL",
	AC_CHECK_LIB(gl, main,
		OPENGL_LIBS="-lgl",
		AC_MSG_WARN([OpenGL library not found.])))
		

AC_CHECK_LIB(GLU, main,
	OPENGL_LIBS="${OPENGL_LIBS} -lGLU",
	AC_CHECK_LIB(glu32, main,
		OPENGL_LIBS="${OPENGL_LIBS} -lglu32",
		AC_MSG_WARN([GLU library not found.])))
AC_SUBST([OPENGL_LIBS], [${OPENGL_LIBS}])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gethostbyname gettimeofday memset pow select socket sqrt strchr strstr])

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
