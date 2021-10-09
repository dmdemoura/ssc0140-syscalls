# Project for ICMC's SCC0140 Operating Systems class.

**Developed by**: Amanda de Moura Peres - 10734522

**Objective:** Add a new syscall to the linux kernel.

The syscalls were implemented on top of Arch Linux's fork of Linux 5.14.9,
as that was what I was running in my computer.

- Arch's kernel fork: https://github.com/archlinux/linux/tree/v5.14.9-arch2
- My fork, with extra syscalls: https://github.com/dmdemoura/linux/tree/SSC0140

The patched Kernel was tested on real hardware, and I was able to boot and use
my system without problems. Later, I also got it running on QEMU so it's easier
to test.

1. [Syscalls](#Syscalls)
	1. [sys_hello_world()](#sys-hello-world())
	2. [sys_bad_setuid()](#sys-bad-setuid())
2. [Userspace Wrapper Library](#Userspace-Wrapper-Library)
	1. [hello_world()](#hello-world())
	2. [bad_setuid()](#bad-setuid())
3. [Applications](#Applications)
4. [Download the sources](#Download-the-sources)
5. [Configure the Kernel](#Configure-the-Kernel)
6. [Build the Kernel](#Build-the-Kernel)
7. [Install the Kernel](#Install-the-Kernel)
	1. [Run in a virtual machine](#Run-in-a-virtual-machine)
	1. [Run on real hardware](#Run-on-real-hardware)
8. [Build the Applications](#Build-the-Applications)
9. [Test the Applications](#Test-the-Applications)

## Syscalls

### sys_hello_world()

Prints a message to the kernel log.
```c
long sys_hello_world(char* msg)
```
msg - An ASCII encoded string to be output to dmesg. Must be null terminated. <br/>
return - Always returns 1.

### sys_bad_setuid()

Works like setuid(), but with no permission checking. <br/>
So any user can change into any other user, regardless of normal security measures.
```c
long sys_bad_setuid(uid_t uid)
```
uid - The new effective, saved and filesystem uid to assume. <br/>
return - Returns greater than 0 on success, returns a negative errno compatible erro value on failure. 

## Userspace Wrapper Library

### hello_world()

Prints a message to the kernel log.
```c
int hello_world(char* msg)
```
msg - An ASCII encoded string to be output to dmesg. Must be null terminated. <br/>
return - Returns 0 on success, -1 on failure (Also sets errno).

### bad_setuid()

Works like setuid(), but with no permission checking. <br/>
So any user can change into any other user, regardless of normal security measures.
```c
int bad_setuid(uid_t uid)
```
uid - The new effective, saved and filesystem uid to assume. <br/>
return - Returns 0 on success, -1 on failure (Also sets errno).

### Applications

### hello_world

*hello_world* asks for you to input a string, checks if it's only printable ASCII,
then the syscall prints it to the kernel log.

### bad_su

*bad_su* works like *su*, but instead of using the *setuid()* syscall, it uses *bad_setuid()*.
As *bad_setuid* is implemented to ignore the normal security verifications, *bad_su* basically
let you change into any user, without any kind of authentication.

No amount of security measures can survive someone recompiling your kernel with extra 
security holes!

## Download the sources

```sh
git clone https://github.com/dmdemoura/scc0140-syscalls
cd scc0140-syscalls
git clone --depth 1 --branch SSC0140 https://github.com/dmdemoura/linux
```

## Configure the Kernel

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
testing process too much, I tried to use a virtual machine instead, which simplifies testing a lot.

I've already configured the kernel for running in qemu (see [qemu-config](./qemu-config)),
so it's as simple as copying the config to the kernel folder:
```sh
cp qemu-config linux/.config
```

## Build the kernel

The kernel is built using it's standard build tools. For quick reference:
```sh
cd linux/
num_cpus=$(nproc) # Get number of logical CPUs.
make -j${num_cpus} # Builds the kernel using num_cpus gcc instances so it's way faster.
make modules -j${num_cpus} # Build kernel modules (If needed)
cd ..
```

## Install the Kernel

### Run in a virtual machine

For this we will build a minimal initramfs, with everything needed to
test the syscalls and applications developed, by using busybox.

First, we will install kernel userspace headers into the initramfs:
```sh
cd linux/
make headers_install ARCH=x86_64 INSTALL_HDR_PATH=../initramfs/usr -j4
cd ..
```

Then, we need to download, build, and install busybox to the initramfs:
```sh
wget https://www.busybox.net/downloads/busybox-1.34.1.tar.bz2 # Download busybox
tar -jxf busybox-1.34.1.tar.bz2	# Extract it

cp busybox-config busybox-1.34.1/.config # Copy the included configuration
cd busybox-1.34.1/
make -j${num_cpus} # Build busybox
make install # Install busybox to ./_install
cp -a ./_install/* ../initramfs/ # Copy files to initramfs

cd ..
```

### Run on real hardware

Kernel installation can be done in many forms, and it can be very distro dependant.
But for a quick testing install, this manual method works:
```sh
cd linux/
sudo make modules_install -j${num_cpus} # Install kernel modules and headers to /lib/modules/
sudo cp -v arch/x86_64/boot/bzImage /boot/vmLinuz-scc0140 # Copy kernel to boot partition. Make sure it's mounted.
cd ..
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


## Build the Applications

```sh
make # Builds hello_world and bad_su
make install # Install to initramfs/
make initramfs # Package and compress initramfs in a way the kernel can read.
```

### Run with QEMU

Just use:
```sh
make run
```

To exit qemu: Press `Ctrl-a` then press `c`. Finally type `quit` at QEMU console and press enter.

## Test the Applications

Login with user `amanda` and password `so`.

```sh
hello_world # Type a string in standard input
dmesg | grep Hello # Then verify that it print to the kernel log.
```

There are three users configured, `root`, `amanda` and `irwaen`. 
But only `amanda` has a password, so normally there would be no way to log in as the other two.Fortunately, we have a giant security hole in the form of `bad_su`, so let test it:
```sh
ls /home/irwaen # Will fail with permission denied.
cat /home/irwaen/secret # Will also fail.

su irwaen # Can't do that, it will ask for password and fail since there's none.

bad_su irwaen # If su won't help, then bad_su should!

whoami # Now you should be irwaen.

ls /home/irwaen # There are indeed secrets here!
cat /home/irwaen/secret # Not so secret now

cat /etc/shadow # Permission denied. Again!

su # su won't help. Again.

bad_su # Normal users files might be secret but are very boring, let's try switching to root.

whoami # Unlimited power!

cat /etc/shadow # Even root's file's aren't safe.
```

## Resources

Adding a new system call: https://www.kernel.org/doc/html/latest/process/adding-syscalls.html
<br/>
Build and run minimal Linux / Busybox systems in Qemu: https://gist.github.com/chrisdone/02e165a0004be33734ac2334f215380e
<br/>
Prepare the environment for developing Linux kernel with qemu: https://medium.com/@daeseok.youn/prepare-the-environment-for-developing-linux-kernel-with-qemu-c55e37ba8ade
