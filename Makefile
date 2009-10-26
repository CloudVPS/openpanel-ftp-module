include makeinclude

OBJ	= main.o version.o

all: module.xml ftpmodule.exe
	grace mkapp ftpmodule 

module.xml: module.def
	mkmodulexml < module.def > module.xml

version.cpp:
	grace mkversion version.cpp

ftpmodule.exe: $(OBJ)
	$(LD) $(LDFLAGS) -o ftpmodule.exe $(OBJ) $(LIBS) \
	/usr/lib/opencore/libcoremodule.a -lz -lssl

clean:
	rm -f *.o *.exe
	rm -rf ftpmodule.app
	rm -f ftpmodule

SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I/usr/include/opencore -c -g $<
