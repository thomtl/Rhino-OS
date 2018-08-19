/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_kernel_early)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */

   SECTIONS {
      /* The kernel will live at 3GB + 1MB in the virtual
         address space, which will be mapped to 1MB in the
         physical address space. */
      . = 0xC0100000;
      _kernel_start = .;
      .text : AT(ADDR(.text) - 0xC0000000) {
          *(.multiboot)
          *(.text)
          *(.rodata*)
      }

      .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) {
          *(.data)
      }

      .bss : AT(ADDR(.bss) - 0xC0000000) {
          _sbss = .;
          *(COMMON)
          *(.bss)
          _ebss = .;
      }
      _kernel_end = .;
   }
