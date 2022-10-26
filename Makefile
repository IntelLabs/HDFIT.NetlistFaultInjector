
CC = gcc
CXX = g++
RM = rm -f

CPPFLAGS = -pedantic \
		-Wall \
		-W \
		-Wno-sign-compare \
		-Wno-unused-parameter \
		-Werror \
		-march=native \
		-std=c++17

EXE = netlistFaultInjector

SRCS = main.cpp RtlFile.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean :
	${RM} ${EXE} *.o

include .depend
