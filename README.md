# Project for ICMC's SCC0140 Operating Systems class.


**Objective:** Add a new syscall to the linux kernel.

The syscalls were implemented on top of Arch Linux's fork of Linux 5.14.9,
as that was what I was running in my computer.

- Arch's kernel fork: https://github.com/archlinux/linux/tree/v5.14.9-arch2
- My fork, with extra syscalls: https://github.com/dmdemoura/linux/tree/SSC0140


The patched Kernel was tested on real hardware, and I was able to boot and use
my system without problems.

## Downloading the needed sources

```sh
git clone https://github.com/dmdemoura/scc0140-syscalls
cd scc0140-syscalls
git clone --depth 1 --branch SSC0140 https://github.com/dmdemoura/linux
```

## How to configure the Kernel

Before building the Linux kernel must be configured, and there are several options on how to this:

1. Use your [distribution's default config](https://github.com/archlinux/svntogit-packages/blob/packages/linux/trunk/config),
this avoids any trouble with your system not working because a kernel feature is missing.
However it takes ages to build as generic distro configs tend to enable a lot of features.
2. Use a handbuilt config specific to your system. This compiles much faster but requires more work.
	1. If trying to configure the kernel for a real desktop system, take a look at
	[Gentoo's kernel configuration guide](https://wiki.gentoo.org/wiki/Kernel/Gentoo_Kernel_Configuration_Guide).
	2. Configuring the kernel to run a virtual machine with a simple distro installed, can be 
	much easier then getting a fully fledged desktop system to boot.

I foolishly chose option 1, the first time and waited overnight for the kernel to build,
but it did boot.
The next try I copied an old Gentoo custom config, and after a few tweaks the build times were much productive.

I have included [my kernel config](iridium-config) as an curiosity but it won't be of any
use unless you happen to also have an 2012 Intel desktop with sata disks formatted as ext4.

### An simpler option (Recommended)

As all those manual steps and need to account differences in real computers complicates the
testing process too much, I tried to use a virtual machine instead, which simplifies the process a lot.

I've already configured the kernel for running in qemu (see [qemu-config](./qemu-config)),
so it's as simple as copying the config to the kernel folder:
```sh
cp qemu-config linux/.config
```

## How to build the kernel

The kernel is built using it's standard build tools. For quick reference:
```sh
num_cpus=$(nproc) # Get number of logical CPUs.
make -j${num_cpus} # Builds the kernel using num_cpus gcc instances so it's way faster.
make modules -j${num_cpus} # Build kernel modules (If needed)
```

## Installing the kernel

Kernel installation can be done in many forms, and it can be very distro dependant.
But for a quick testing install, this manual method works:
```sh
sudo make modules_install -j${num_cpus} # Install kernel modules and headers to /lib/modules/
sudo cp -v arch/x86_64/boot/bzImage /boot/vmLinuz-scc0140 # Copy kernel to boot partition. Make sure it's mounted.
```

Depending on how the kernel has been configured, you must also generate and install an
initramfs image. Usually this only needs to be done for system's that require module
loading to be available before being to finish booting, such as when root filesystem drivers
have been compiled as modules. Most distribution kernel configs require an initramfs.

On Arch Linux to generate the initramfs:
```sh
sudo mkinitcpio -k 5.14.9-arch2scc0140+ -g /boot/initramfs-scc0140.img
```
For other distributions, check the their documentation. There's usually a tool for automating this step. Or compile all the needed features as part of the kernel instead of modules.


## Syscalls

### sys_hello_world()

Prints a message to the kernel log.
```c
long sys_hello_world(char* msg)
```
msg - An ASCII encoded string to be output to dmesg. Must be null terminated. <br/>
return - Always 

### sys_bad_setuid()

Works like setuid(), but with no permission checking. <br/>
So any user can change into any other user, regardless of normal security measures.
```c
long sys_bad_setuid(uid_t uid)
```
msg - An ASCII encoded string to be output to dmesg. Must be null terminated. <br/>
return - Always 

## Userspace wrapper library

### hello_world()

```c
int hello_world(char* msg)
```

### bad_setuid()

```c
int bad_setuid(uid_t uid)
```


Kernel changes: https://github.com/dmdemoura/linux/commit/9f6031c3bc6072d6032979d7c2f7fccf0599661f



#### References

Adding a new system call: https://www.kernel.org/doc/html/latest/process/adding-syscalls.html
