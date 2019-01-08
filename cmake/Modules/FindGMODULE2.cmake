#
# - Try to find GModule2
# Find GModule headers, libraries and the answer to all questions.
#
#  GMODULE2_FOUND               True if GMODULE2 got found
#  GMODULE2_INCLUDE_DIR         Location of GMODULE2 headers
#  GMODULE2_LIBRARIES           List of libraries to use GMODULE2
#


include( FindPkgConfig )

if( GMODULE2_FIND_REQUIRED )
	set( _pkgconfig_REQUIRED "REQUIRED" )
else()
	set( _pkgconfig_REQUIRED "" )
endif()

if( GMODULE2_MIN_VERSION )
	pkg_search_module( GMODULE2 ${_pkgconfig_REQUIRED} gmodule-2.0>=${GMODULE2_MIN_VERSION} )
else()
	pkg_search_module( GMODULE2 ${_pkgconfig_REQUIRED} gmodule-2.0 )
endif()

if( GMODULE2_FOUND  )
	if( GMODULE2_LIBRARY_DIRS )
		LINK_DIRECTORIES( ${GMODULE2_LIBRARY_DIRS} )
	endif()
	set(GMODULE2_INCLUDE_DIR ${GMODULE2_INCLUDE_DIRS})
	# Darwin does not work properly with -lgmodule-2.0;-lglib-2.0;-lintl;
	# Therefore, we need to explicitly set the libraries
	# Here we assume and enforce that users are using HomeBrew
	# Therefore, we give a hint at /usr/local/lib/. 
	# For gettext (libintl) we set /usr/local/opt/gettext/lib/ because it is keg-only
	# A more general approach to catch macports is expected in the future
	if(APPLE)
                #find_library( GMODULE2_LIBRARIES glib )
                #string(REGEX REPLACE "-lgmodule-2.0;-lglib-2.0;-lintl;" " " GMODULE2_STATIC_LDFLAGS_A "${GMODULE2_STATIC_LDFLAGS}")
                #string(REGEX REPLACE "-lgmodule-2.0;-lglib-2.0;-lintl;" " " GMODULE2_LDFLAGS_A "${GMODULE2_LDFLAGS}")
                set(GMODULE2_HINTS "/usr/local/lib")
                set(GMODULE2_LIBRARIES "${GMODULE2_HINTS}/libgmodule-2.0.dylib;${GMODULE2_HINTS}/libglib-2.0.dylib;/usr/local/opt/gettext/lib/libintl.dylib" CACHE PATH "Paths to GMODULE2 libs" FORCE)
                #message(STATUS "${GMODULE2_LIBRARIES}")
        else()
                find_library( GMODULE2_LIBRARIES NAMES gmodule-2.0 gmodule HINTS "${GMODULE2_HINTS}/lib" )
        endif()	
else()
	find_path( GMODULE2_INCLUDE_DIR NAMES gmodule.h	PATH_SUFFIXES glib-2.0 HINTS "${GMODULE2_HINTS}/include")
	if(APPLE)
                find_library( GMODULE2_LIBRARIES glib )
        else()
                find_library( GMODULE2_LIBRARIES NAMES gmodule-2.0 gmodule HINTS "${GMODULE2_HINTS}/lib" )
	endif()
	
	include( FindPackageHandleStandardArgs )
	find_package_handle_standard_args( GMODULE2 DEFAULT_MSG GMODULE2_LIBRARIES GMODULE2_INCLUDE_DIR )
endif()
mark_as_advanced( GMODULE2_LIBRARIES GMODULE2_INCLUDE_DIR )