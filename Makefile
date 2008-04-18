include makeinclude

OBJ	= main.o version.o

all: module.xml ftpmodule.exe
	mkapp ftpmodule 

module.xml: module.def
	mkmodulexml < module.def > module.xml

version.cpp:
	mkversion version.cpp

ftpmodule.exe: $(OBJ)
	$(LD) $(LDFLAGS) -o ftpmodule.exe $(OBJ) $(LIBS) \
	../opencore/api/c++/lib/libcoremodule.a -lz -lssl

clean:
	rm -f *.o *.exe
	rm -rf ftpmodule.app
	rm -f ftpmodule

SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I../opencore/api/c++/include -c -g $<
