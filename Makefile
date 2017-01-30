obj-m := i210_mod.o

i210_mod-y += i210.o \
			i210_pci.o \

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

