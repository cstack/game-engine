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
LIBS=-lppapi_simple -lppapi -lppapi_cpp -lnacl_io

game.o: game.cc
	$(CXX) -c -o game.o -pthread -I $(PPAPI_INCLUDE) game.cc

minimal_unstripped.bc: game.o
	$(LINK) -o minimal_unstripped.bc -pthread -L "$(PPAPI_LIBDIR)" game.o $(LIBS)

game.pexe: minimal_unstripped.bc
	$(FINALIZE) -o game.pexe minimal_unstripped.bc

all: game.pexe

clean:
	rm *.pexe *.bc *.o
