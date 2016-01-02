PEPPER=/Users/cstack/Development/nacl_sdk/pepper_46
PLATFORM=mac
BINDIR=bin
BUILD_VARIANT=Debug
TOOLCHAIN=$(PEPPER)/toolchain/$(PLATFORM)_pnacl/$(BINDIR)
CXX=$(TOOLCHAIN)/pnacl-clang++
LINK=$(CXX)
FINALIZE=$(TOOLCHAIN)/pnacl-finalize
PPAPI_INCLUDE=$(PEPPER)/include
PPAPI_LIBDIR=$(PEPPER)/lib/pnacl/$(BUILD_VARIANT)
LIBS=-lppapi_simple_cpp -lnacl_io -lsdk_util -lppapi_cpp -lppapi -lpthread

all: game.pexe

clean:
	rm *.pexe *.bc *.o

color.o: engine/color.cc
	$(CXX) -c -o color.o -pthread engine/color.cc

util.o: engine/util.cc
	$(CXX) -c -o util.o -pthread engine/util.cc

game.o: game.cc
	$(CXX) -c -o game.o -pthread game.cc

nacl.o: platform/nacl.cc
	$(CXX) -c -o nacl.o -pthread -I $(PPAPI_INCLUDE) platform/nacl.cc

minimal_unstripped.bc: game.o nacl.o color.o util.o
	$(LINK) -o minimal_unstripped.bc -pthread -L "$(PPAPI_LIBDIR)" game.o nacl.o color.o util.o $(LIBS)

game.pexe: minimal_unstripped.bc
	$(FINALIZE) -o game.pexe minimal_unstripped.bc
