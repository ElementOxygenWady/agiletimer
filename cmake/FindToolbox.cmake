#* Author: Wady <iswady@163.com>
#* Date:   Wed May 15 12:00:00 2019

set(TOOLBOX_INCLUDE_DIRS )

find_path(TOOLBOX_INCLUDE_DIRS
    NAMES toolbox/toolbox_logging.h
    PATH_SUFFIXES include
)
if(TOOLBOX_INCLUDE_DIRS)
    set(HAVE_TOOLBOX_H 1)
endif()

find_library(TOOLBOX_LIBRARIES
    NAMES toolbox
)
if(TOOLBOX_LIBRARIES)
    set(TOOLBOX_FOUND 1)
endif()
