
VERILATOR_TOP ?= /usr/share/verilator

CXX = g++
CPPFLAGS = \
		-Wall \
		-W \
		-Wno-sign-compare \
		-Wno-unused-parameter \
		-Werror \
		-Ofast \
		-march=native \
		-std=c++17

CPPFLAGS_VERILATED = -Ofast \
		-march=native \
		-std=c++17
	
VERILATOR_OPTIONS= --x-assign fast --x-initial fast --noassert --clk gcreudpasclk # TODO: including -O3 makes it slower
MAKE_OPTIONS='OPT_FAST=-Ofast -march=native'

SV2V_OPT=-E=Always -E=Assert -E=Interface -E=Logic -E=UnbasedUnsized

.PHONY: all

all : clean test

fma.v: fma.sv globals.sv
	sv2v --write=$@ $(SV2V_OPT) $^

JmsFlipFlop.v: JmsFlipFlop.sv
	sv2v --write=$@ $(SV2V_OPT) $^
	
fma_netlist.v: fma.v JmsFlipFlop.v ../netlistFaultInjector
	yosys -s yosys.script
	../netlistFaultInjector fma_netlist.v fma

fmaFiSignals.cpp: fma_netlist.v

obj_dir/Vfma_netlist.mk : fma_netlist.v
	verilator $(VERILATOR_OPTIONS) -CFLAGS -fPIC -Wall -Wno-fatal -cc fma_netlist.v

fma_netlist.a : obj_dir/Vfma_netlist.mk
	cd obj_dir && make $(MAKE_OPTIONS) -f Vfma_netlist.mk && mv Vfma_netlist__ALL.a fma_netlist.a && mv Vfma_netlist.h fma_netlist.h && cd ..

verilated.o : $(VERILATOR_TOP)/include/verilated.cpp
	$(CXX) $(CPPFLAGS_VERILATED) -fPIC $^ -c $<

netlistFaultInjector.o : ../netlistFaultInjector.cpp ../netlistFaultInjector.hpp 
	$(CXX) $(CPPFLAGS) -fPIC -I ../ -c $<

fmaFiSignals.o : fmaFiSignals.cpp ../netlistFaultInjector.hpp
	$(CXX) $(CPPFLAGS) -fPIC -I ../ -c fmaFiSignals.cpp

test : main.cpp netlistFaultInjector.o fmaFiSignals.o fma_netlist.a verilated.o
	$(CXX) $(CPPFLAGS) -I $(VERILATOR_TOP)/include main.cpp -o test fmaFiSignals.o netlistFaultInjector.o obj_dir/fma_netlist.a verilated.o

clean :
	rm -f fmaFiSignals.cpp && rm -f *.a && rm -f *.v && rm -f *.o && rm -f -r obj_dir && rm -f test
