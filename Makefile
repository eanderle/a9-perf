
obj-m := a9_perf.o

.PHONY: clean

clean:
	rm -rf *.ko *.o *.mod.* modules.order Module.symvers
