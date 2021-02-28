PWD := $(shell pwd)
KVERSION := $(shell uname -r)
BASEINCLUDE ?= /usr/src/linux-headers-$(KVERSION)/
#BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo-objs := simple_char.o 

obj-m	:=   mydemo.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


