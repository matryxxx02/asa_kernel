first kernel
------------

The first kernel project is a skeleton for low level direct programming of x86 hardware. 
It builds a simple bootable .iso file using grub and running your main.c file. 

## files description
 
Here is the initial file structure of the project : 

```
	.
	├── boot		The directory structure used by grub-mkrescue to build a bootable .iso
	│   └── grub		
	│       ├── grub.cfg	The grub configuration file
	│       └── unicode.pf2	The unicode font used by grub
	├── CONTRIBUTORS.md	The contributors list of this skeleton
	├── include		The directory used to store .h file
	    ├── ioport.h	C wrapper to in and out intel assembly instructions 
	    ├── gdt.h		Global Descriptor Table interface providing a default setup
	    ├── idt.h		Interrupt Descriptor Table configuration and interface
	├── LICENSE		The GNU General Public Licence used by this project
	├── link.ld		The link script used by gcc to build the project
	├── Makefile		The makefile building mykernel.iso from the source 
	├── README.md		This file (using markdown)
	└── src			The directory used to store the assembly and C sources
	    ├── 0boot.s		The actual entrypoint called by multiboot
	    ├── idt0.s		Interrupt Descriptor Table assembly routines
	    ├── idt.c		Interrupt Descriptor Table configuration and interface
	    ├── gdt.c		Global Descriptor Table configuration and interface
	    └── main.c		The main.c file where the main() is implemented.
```

## get started

The source of your kernel is splitted in five files : 
1. 0boot.s 
2. main.c
4. gdt.c
5. idt0.s
6. idt.c

0boot.s is the real entry point of your kernel. It just stops hardware interrupt, sets a call-stack and
jumps to the C main with the multiboot structure address as argument of main.

IDT and GDT-related files should be working out of the box, and configure the CPU on boot. You can play
with the IDT configuration to handle various interrupts.

main.c is the C code that will run : your kernel ! You can change it in order to print "Hello My World".

	$ vi src/main.c   

In order to build you first kernel, just run the Makefile :

	$ make

"make" will assembly the assembly code (using nasm) and compile the C code (using gcc). 

It will use a specific link script "./link.ld" to build the binary image (using ld). 

The binary image (namely mykernel.bin) will be copied in ./grub/ in order to build the .iso file 
(using grub-mkrescue). 

If nothing goes wrong you will get a "mykernel.iso" file in "./" of the project. 
You can burn it to run your software. In this case your code will be run on a bare metal environment. 
This is the goal of the project. Nevertheless, developping software on bare metal environment is boring.
You don't have your usual "operating system facility" to run and test your software. 
Most of the time, a bug will conclude by freezing or rebooting the machine. So we invite you to debug your
kernel on an emulator, namely qemu.

In order to run your .iso file on qemu just try :

	$ make run

qemu will emulate an x86 hardware environment running your iso file. That's it ! 


You could also clean your project using :

	$ make clean

## Build the kernel on OSX

First, you need to set up yours. Using [MacPorts](https://www.macports.org/install.php) you will be able to install needed tools.

	$ port install i386-elf-binutils nasm


After that, test if and are installed, for example with the following command:

	$ which i386-elf-gcc nasm
	/opt/local/bin/i386-elf-gcc
	/opt/local/bin/nasm

Now, you need to install GRUB2. Source: [os-dev](http://wiki.osdev.org/GRUB_2#Installing_GRUB_2_on_OS_X)

1. Clone the developer version of the sources:

```
$ git clone git://git.savannah.gnu.org/grub.git
```
2. A tool named [objconv](https://github.com/vertis/objconv) is required, get it from:
```
$ git clone https://github.com/vertis/objconv.git
```
Download sources, compile (see website for details) and make available in your PATH.

3. Run "autogen.sh" in the GRUB sources folder 

4. Create a seperate build directory, switch to it, and run GRUB's configure script (insert your target-specific tools!): 
```
$ ../grub/configure --disable-werror TARGET_CC=i386-elf-gcc TARGET_OBJCOPY=i386-elf-objcopy \ TARGET_STRIP=i386-elf-strip TARGET_NM=i386-elf-nm TARGET_RANLIB=i386-elf-ranlib --target=i386-elf
```
5. Run "make" and "make install" 
Now you have a working GRUB 2 that has the required files to build an image that boots on i386 platforms. 

Finaly, you will need a version of xorriso above 1.2.9, using [brew](https://brew.sh/):

	$ brew install xorriso

PS: If you miss some dependencies, fix it and let me know.

Now, you will need to tell, where are your installed package. In order to do that, add the prefix i386-elf to gcc and ld. Then change targeted directory by the one you will found with the next command:

	$ sudo find / -type f -name modinfo.sh

In my case: /usr/local/lib/grub/i386-pc/

/!\ Don't forget to update the Makefile with the right tools /!\
