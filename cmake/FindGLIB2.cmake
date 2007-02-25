# - Try to find the GLIB2 libraries
# Once done this will define
#
#  GLIB2_FOUND - system has glib2
#  GLIB2_INCLUDE_DIR - the glib2 include directory
#  GLIB2_LIBRARIES - glib2 library

if(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)
    # Already in cache, be silent
    set(GLIB2_FIND_QUIETLY TRUE)
endif(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)

INCLUDE(UsePkgConfig)
PKGCONFIG(${GLIB2_REQ} _LibGLIB2IncDir _LibGLIB2LinkDir _LibGLIB2LinkFlags _LibGLIB2Cflags)

FIND_PATH(GLIBCONFIG_INCLUDE_DIR
	    NAMES
	    glibconfig.h
	    PATHS
	    ${_LibGLIB2IncDir}
	    /opt/gnome/lib64/glib-2.0/include
	    /opt/gnome/lib/glib-2.0/include
	    /opt/lib/glib-2.0/include
	    /usr/lib64/glib-2.0/include
	    /usr/lib/glib-2.0/include
	    /sw/lib/glib-2.0/include
)

FIND_PATH(GLIB2_INCLUDE_DIR glib.h
   ${_LibGLIB2IncDir}
   ${_LibGLIB2IncDir}/glib-2.0
   /usr/include/glib-2.0
   /usr/include
   /usr/local/include/glib-2.0
   /usr/local/include
)

FIND_LIBRARY(GLIB2_LIBRARIES NAMES glib-2.0
    PATHS
    ${_LibGLIB2LinkDir}
    /usr/lib
    /usr/local/lib
)

if(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES AND GLIBCONFIG_INCLUDE_DIR)
   set(GLIB2_FOUND TRUE)
endif(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES AND GLIBCONFIG_INCLUDE_DIR)

if(GLIB2_FOUND)
   if(NOT GLIB2_FIND_QUIETLY)
      message(STATUS "Found glib2: ${GLIB2_LIBRARIES} ${GLIB2_LIBRARIES}")
   endif(NOT GLIB2_FIND_QUIETLY)
endif(GLIB2_FOUND)

MARK_AS_ADVANCED(GLIB2_INCLUDE_DIR GLIB2_LIBRARIES)
