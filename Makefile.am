
AM_CPPFLAGS = $(SDL_CFLAGS) $(FT2_CFLAGS) -DNO_AUDIO -g

AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = hadoopviz

hadoopviz_LDFLAGS = $(SDL_LIBS) $(FT2_LIBS) $(MAC_LIBS) $(OPENGL_LIBS) 

hadoopviz_SOURCES = \
display.cpp \
events.cpp \
init.cpp \
main.cpp \
display.h \
events.h \
init.h \
AbstractScene/AbstractScene.cpp \
Camera/Camera.cpp \
Cursor/Cursor.cpp \
FreeType/FreeType.cpp \
HadoopDropper/DropListener.cpp \
HadoopDropper/Drops.cpp \
HadoopDropper/HadoopDropper.cpp \
HadoopDropper/IPandLoc.cpp \
HadoopDropper/IPtoLocation.cpp \
HadoopDropper/SelectionDisplay.cpp \
HadoopDropper/SingleDrop.cpp \
HadoopDropper/NodeStatistics.cpp \
HadoopDropper/GlobalStats.cpp \
HadoopDropper/UI.cpp \
Math/Math.cpp \
TextureLoader/CTargaImage.cpp \
AbstractScene/AbstractScene.h \
Camera/Camera.h \
Cursor/Cursor.h \
FreeType/FreeType.h \
HadoopDropper/DropListener.h \
HadoopDropper/Drops.h \
HadoopDropper/HadoopDropper.h \
HadoopDropper/IPandLoc.h \
HadoopDropper/IPtoLocation.h \
HadoopDropper/SelectionDisplay.h \
HadoopDropper/SingleDrop.h \
HadoopDropper/NodeStatistics.h \
HadoopDropper/GlobalStats.h \
HadoopDropper/UI.h \
Math/Math.h \
TextureLoader/CTargaImage.h \
defaults.h


