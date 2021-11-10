noinst_PROGRAMS = Timer
AM_CXXFLAGS = @CPPFLAGS_MYSQL@ @CPPFLAGS_XML@

Timer_SOURCES= 	lbtimer.cpp \
			lbwnd.cpp.cpp \
			main.cpp

Timer_LDFLAGS = @LIBS_MYSQL@ @LIBS_XML@ $(top_builddir)/hwsdk/libhwsdk.la $(top_builddir)/encrypt/libencrypt.la $(top_builddir)/hwscript/libhwscript.la $(top_builddir)/json/libmyjson.a $(top_builddir)/protobuf/libmyprotobuf.a $(top_builddir)/protobuf/protobuf/lib/libprotobuf.a
