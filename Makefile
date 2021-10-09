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
	@cp -v include/* initramfs/usr/include
	@cp -v hello_world initramfs/usr/bin/	
	@cp -v bad_su initramfs/usr/bin/	

initramfs:
	@echo Packaging initramfs ...
	@cd initramfs && find . -not -path './home/*' -print0\
	 | cpio --null -ov --format=newc -R 0:0 -F ../initramfs-scc0140.cpio
	@cd initramfs && find . -path './home/amanda*' -print0\
	 | cpio --null -Aov --format=newc -R 1000:1000 -F ../initramfs-scc0140.cpio
	@cd initramfs && find . -path './home/irwaen*' -print0\
	 | cpio --null -Aov --format=newc -R 1001:1001 -F ../initramfs-scc0140.cpio
	@echo Compressing initramfs ...
	@cat initramfs-scc0140.cpio | gzip -9 > initramfs-scc0140.cpio.gz

run:
	@echo Running kernel with qemu ...
	@qemu-system-x86_64 -kernel linux/arch/x86_64/boot/bzImage -initrd initramfs-scc0140.cpio.gz -nographic --append "console=ttyS0 loglevel=4" -enable-kvm

clean:
	@echo Cleaning build files ...
	@rm -vf hello_world bad_su src/*.o\
		initramfs-scc0140.cpio.gz

