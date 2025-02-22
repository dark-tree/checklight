if (NOT TARGET freetype)
		include(FetchContent)
		FetchContent_Declare(
				freetype
				GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
				GIT_TAG VER-2-13-2
		)
		FetchContent_MakeAvailable(freetype)

		set(FREETYPE_LIBRARY freetype)
		set(FREETYPE_INCLUDE_DIRS ${freetype_SOURCE_DIR}/include)
endif()

if (NOT TARGET Freetype::Freetype)
		add_library(Freetype::Freetype ALIAS freetype)
endif()

set(Freetype_FOUND TRUE)
