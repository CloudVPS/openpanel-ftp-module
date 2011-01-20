
# This file is part of OpenPanel - The Open Source Control Panel
# OpenPanel is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, using version 3 of the License.
#
# Please note that use of the OpenPanel trademark may be subject to additional 
# restrictions. For more information, please visit the Legal Information 
# section of the OpenPanel website on http://www.openpanel.com/

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
	/usr/lib/openpanel-core/libcoremodule.a

install:
	mkdir -p ${DESTDIR}/var/openpanel/modules/FTP.module
	mkdir -p ${DESTDIR}/var/openpanel/conf/staging/FTP
	cp -rf ./ftpmodule.app    ${DESTDIR}/var/openpanel/modules/FTP.module/
	ln -sf ftpmodule.app/exec ${DESTDIR}/var/openpanel/modules/FTP.module/action
	cp     module.xml          ${DESTDIR}/var/openpanel/modules/FTP.module/module.xml
	install -m 755 verify      ${DESTDIR}/var/openpanel/modules/FTP.module/verify
	cp *.html ${DESTDIR}/var/openpanel/modules/FTP.module

clean:
	rm -f *.o *.exe
	rm -rf ftpmodule.app
	rm -f ftpmodule

SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I/usr/include/opencore -c -g $<
