CC		:= musl-gcc
CFLAGS	:= -I include/ -I initramfs/usr/include/
LDFLAGS := -static

.PHONY: all clean install initramfs run

%.o: %.c
	@echo Compiling $< ...
	$(CC) -c $(CFLAGS) $< -o $@

all: hello_world bad_su

hello_world: src/hello_world.o
	@echo Linking program ...
	$(CC) $(LDFLAGS) $< -o $@

bad_su: src/bad_su.o
	@echo Linking program ...
	$(CC) $(LDFLAGS) $< -o $@

install: all
	@echo Copying headers and binaries to initramfs ...
	@cp -v hello_world initramfs/bin/	
	@cp -v bad_su initramfs/bin/	

initramfs:
	@echo Packaging initramfs ...
	@cd initramfs && find . -print0\
	 | cpio --null -ov --format=newc -R 0:0\
	 | gzip -9 > ../initramfs-scc0140.cpio.gz

run:
	@echo Running kernel with qemu ...
	@qemu-system-x86_64 -kernel linux/arch/x86_64/boot/bzImage -initrd initramfs-scc0140.cpio.gz -nographic --append "console=ttyS0 loglevel=4" -enable-kvm

clean:
	@echo Cleaning build files ...
	@rm -vf hello_world bad_su src/*.o\
		initramfs-scc0140.cpio.gz

