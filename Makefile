
FREETYPE_CFLAGS = $(shell freetype-config --cflags)
FREETYPE_LIBS = $(shell freetype-config --libs)

#LIB3DS_CFLAGS = $(shell lib3ds-config --cflags)
#LIB3DS_LIBS = $(shell lib3ds-config --libs)

SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs) -lGLU -lGL
#SDL_LIBS = -lSDLmain -lSDL

OS = $(shell uname -s)

ifeq ($(OS), Darwin)
  OSLIBS = -framework OpenGL
endif


INCLUDES =  -I.

CPPFLAGS =  -Wall -g -fPIC -funroll-loops $(INCLUDES) $(FREETYPE_CFLAGS) $(LIB3DS_CFLAGS) $(SDL_CFLAGS) -DNO_AUDIO
CXXFLAGS = 
LIBS = -g -lm $(FREETYPE_LIBS) $(LIB3DS_LIBS) $(SDL_LIBS) $(OSLIBS) -lGL -lGLU # -lSDL_mixer
LDFLAGS = 

CXX = g++

HADOOPDROPPER_SRC = HadoopDropper.cpp Drops.cpp DropListener.cpp IPtoLocation.cpp IPandLoc.cpp SingleDrop.cpp SelectionDisplay.cpp

MATH_SRC = Math.cpp

CURSOR_SRC = Cursor.cpp

AUDIO_SRC = Audio.cpp

TEXTPRINTER_SRC = BitMapText.cpp

ABSTRACTSCENE_SRC = AbstractScene.cpp

CAMERA_SRC = Camera.cpp

FREETYPE_SRC = FreeType.cpp

MODELLOADER_SRC = CModel3DS.cpp

CTARGAIMAGE_SRC = CTargaImage.cpp

MAIN_SRC = main.cpp events.cpp display.cpp init.cpp

#	$(MODELLOADER_SRC:%=ModelLoader/%) \
#	$(AUDIO_SRC:%=Audio/%) \
#	$(TEXTPRINTER_SRC:%=TextPrinter/%) \
#	$(MODELLOADER_SRC:%.cpp=ModelLoader/%.o) \
#	$(AUDIO_SRC:%.cpp=Audio/%.o) \
#	$(TEXTPRINTER_SRC:%.cpp=TextPrinter/%.o) \

SOURCES = $(MAIN_SRC) $(CAMERA_SRC:%=Camera/%) \
	$(ABSTRACTSCENE_SRC:%=AbstractScene/%) $(FREETYPE_SRC:%=FreeType/%) \
	$(CTARGAIMAGE_SRC:%=TextureLoader/%) \
	$(MATH_SRC:%=Math/%) \
	$(CURSOR_SRC:%=Cursor/%) \
	$(HADOOPDROPPER_SRC:%=HadoopDropper/%) 
	
OBJS = $(MAIN_SRC:%.cpp=%.o) $(CAMERA_SRC:%.cpp=Camera/%.o) \
	$(ABSTRACTSCENE_SRC:%.cpp=AbstractScene/%.o) $(FREETYPE_SRC:%.cpp=FreeType/%.o) \
	$(CTARGAIMAGE_SRC:%.cpp=TextureLoader/%.o) \
	$(MATH_SRC:%.cpp=Math/%.o) \
	$(CURSOR_SRC:%.cpp=Cursor/%.o) \
	$(HADOOPDROPPER_SRC:%.cpp=HadoopDropper/%.o)
	

all: hadoopviz


hadoopviz: $(OBJS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDES) -o hadoopviz $(LIBS)

	

clean:
	rm -f $(OBJS)

install: hadoopviz
	install -m 755 -d $(DESTDIR)/$(BINDIR)
	install -m 755 hadoopviz.start $(DESTDIR)/$(BINDIR)/hadoopviz
	install -m 755 -d $(DESTDIR)/$(LIBEXECDIR)
	install -m 755 hadoopviz $(DESTDIR)/$(LIBEXECDIR)/hadoopviz
	install -m 755 -d $(DESTDIR)/$(DATADIR)/hadoopviz/textures/
	install -m 755 -d $(DESTDIR)/$(DATADIR)/hadoopviz/fonts/
	install -m 644 fonts/Test.ttf $(DESTDIR)/$(DATADIR)/hadoopviz/fonts/Test.ttf




