DEBFLAGS = -O2
RET_VAL_NUMBER = -1
RET_VAL_TIME = -1

ccflags-y := -DDEF_RET_VAL_TIME=$(RET_VAL_TIME) -DDEF_RET_VAL_NUMBER=$(RET_VAL_NUMBER)

EXTRA_CFLAGS = $(DEBFLAGS) 
EXTRA_CFLAGS += -I$(LDDINC) 


ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= chardev.o 

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 	

endif



clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

#depend .depend dep:
	#$(CC) $(CFLAGS) -M *.c > .depend


#ifeq (.depend,$(wildcard .depend))
#include .depend
#endif

