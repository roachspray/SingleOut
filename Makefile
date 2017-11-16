LLBIN=/usr/lib/llvm-4.0/bin
LLVM_CONFIG=$(LLBIN)/llvm-config
LDIS=$(LLBIN)/llvm-dis
CPP=$(LLBIN)/clang++
CC=$(LLBIN)/clang
LLC=$(LLBIN)/llc
OPTB=$(LLBIN)/opt

#QUIET:=@
QUIET:=

SRC_DIR?=$(PWD)/src
BOD=build/obj

LDFLAGS+=$(shell $(LLVM_CONFIG) --ldflags) 
COMMON_FLAGS=-Wall -Wextra -g
CXXFLAGS+=$(COMMON_FLAGS) $(shell $(LLVM_CONFIG) --cxxflags)
CPPFLAGS+=$(shell $(LLVM_CONFIG) --cppflags) -std=c++11 -I$(SRC_DIR) 
LOADABLE_MODULE_OPTIONS=-shared
LIBS=$(shell $(LLVM_CONFIG) --libs) 

PASS=libSingleOut.so
PASS_OBJECTS=SingleOutPass.o  \
  NoCallFunctionSieve.o  \
  PossiblyCheck.o  \
  SeahornBodyRock.o

default: prep $(PASS)

prep:
	$(QUIET)mkdir -p build/obj
	$(QUIET)mkdir -p build/lib

define builditdood
$(QUIET)$(CPP) -o $(BOD)/$(1)/$(@F) -c $(CPPFLAGS) $(CXXFLAGS) $<
endef

%.o: %.cpp
	@echo "Compiling $*.cpp"
	$(call builditdood,.)

$(PASS) : $(PASS_OBJECTS)
	@echo "Linking $@"
	$(QUIET)$(CPP) -o build/lib/$@ $(LOADABLE_MODULE_OPTIONS) $(CXXFLAGS) $(LDFLAGS) ${addprefix $(BOD)/,$^}

clean:
	$(QUIET)rm -rf build 

