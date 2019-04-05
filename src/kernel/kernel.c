#if defined(__linux__)
#error "No cross compiler detected you will probably run into trouble"
#endif

#include <rhino/kernel.h>
#include <rhino/common.h>

#include <libk/string.h>
#include <libk/stdio.h>
#include <libk/stdlib.h>

#include <rhino/mm/vmm.h>
#include <rhino/mm/pmm.h>
#include <rhino/mm/hmm.h>

#include <rhino/multitasking/task.h>
#include <rhino/multitasking/scheduler.h>

#include <rhino/acpi/acpi.h>

#include <rhino/Deer/deer.h>

#include <rhino/udi/udi.h>

#include <rhino/user/program.h>

#include <rhino/pwr/power.h>

#include <rhino/fs/fat32.h>

#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/pci.h>
#include <rhino/arch/x86/gdt.h>
#include <rhino/arch/x86/msr.h>
#include <rhino/arch/x86/cpuid.h>
#include <rhino/arch/x86/io.h>
#include <rhino/arch/x86/timer.h>

#include <rhino/arch/x86/drivers/rtc.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/arch/x86/drivers/keyboard.h>
#include <rhino/arch/x86/drivers/pcspkr.h>
#include <rhino/arch/x86/drivers/serial.h>
#include <rhino/arch/x86/drivers/fpu.h>
#include <rhino/arch/x86/drivers/sse.h>
#include <rhino/arch/x86/drivers/avx.h>
#include <rhino/arch/x86/drivers/tsc.h>

#include <rhino/kernel.h>
#include <rhino/panic.h>

#include <rhino/types/linked_list.h>

extern uint32_t placement_address;
extern uint32_t _kernel_start;

multiboot_info_t *multibootInfo;
uint32_t ramAmountMB = 0;

#ifdef DEBUG
void user_input(char *input);
#endif

multiboot_info_t *get_mbd()
{
    return multibootInfo;
}

void kernel_main(multiboot_info_t *mbd, unsigned int magic)
{
    char buf[25] = "";

#ifndef DEBUG
    clear_screen();
#else
    set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    uint32_t kern_base = (uint32_t)&_kernel_start;
    hex_to_ascii(kern_base, buf);
    kprint("KERNEL VIRTUAL BASE: ");
    kprint(buf);
    kprint("\n");
    buf[0] = '\0';
    kern_base = (kern_base - KERNEL_VBASE);
    hex_to_ascii(kern_base, buf);
    kprint("KERNEL PHYSICAL BASE: ");
    kprint(buf);
    kprint("\n\n");
#endif

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC_M("Kernel was not booted by a multiboot compliant bootloader!\n");
        return;
    }

    multibootInfo = mbd;

    if (BIT_IS_SET(mbd->flags, 1) && BIT_IS_SET(mbd->flags, 6))
    {
        ramAmountMB = ((((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024) / 1024) + 1; // Formula for converting from Kibibyte to Megabyte so: Megabyte = (Kibibyte * 1024) / 1024 / 1024 + 1
    }
    else
    {
        PANIC_M(": Bootloader did not supply memory info");
    }
    init_serial_early();
    set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    kprint("Starting Rhino 0.3.3, Copyright 2018 Thomas Woertman, The Netherlands\n");
    debug_log("[KERNEL]: Starting Rhino\n");
    int_to_ascii(ramAmountMB, buf);
    kprint("Detected Memory: ");
    kprint(buf);
    debug_log("[KERNEL]: Detected Memory: ");
    debug_log(buf);
    debug_log("MB\n");
    kprint("MB\n\n");

    kprint("Initializing x86 Protected Mode\n");
    gdt_install();
    isr_install();

    init_fpu();
    init_sse();
    init_avx();

    kprint("Initializing Memory Manager\n");

    ASSERT(mbd->mods_count >= 1);
    multiboot_module_t *initrd = (multiboot_module_t *)(mbd->mods_addr + KERNEL_VBASE);
    uint32_t initrd_location = (initrd->mod_start + KERNEL_VBASE);
    uint32_t initrd_end = (initrd->mod_end + KERNEL_VBASE);
    placement_address = initrd_end;

    init_pmm(mbd);
    init_vmm();
    init_heap();

    kprint("Initializing drivers\n");

    init_vfs();

    map_vfs_directory("/dev");

    initialise_initrd(initrd_location);

    if (!fs_root)
        map_vfs_directory("/");

    udi_initialize();

    init_acpi();
    irq_install();

    pci_check_all_buses();

    kprint("Initializing Multitasking\n");
    initTasking();

    fs_node_t *node = kopen("/dev/hda", O_RDWR);
    uint8_t bfuf[512] = "";
    read_fs(node, 0, 512, bfuf);
    uint32_t lba = 0;

    for (int i = 0x1BE; i < (0x1EE + 16); i += 16)
    {

        if (BIT_IS_SET(bfuf[i], 7))
            lba = *((uint32_t *)(bfuf + i + 8));
    }

    init_fat32(node, lba);

    time_t date = read_rtc_time();
    kprint("Date: ");

    int_to_ascii(date.day, buf);
    kprint(buf);
    kprint("/");
    int_to_ascii(date.month, buf);
    kprint(buf);
    kprint("/");
    int_to_ascii(date.year, buf);
    kprint(buf);
    kprint(" ");
    int_to_ascii(date.hour, buf);
    kprint(buf);
    kprint(":");
    if (date.minute < 9)
        kprint("0");
    int_to_ascii(date.minute, buf);
    kprint(buf);
    kprint("\n");

#ifndef DEBUG
    clear_screen();
    enable_scheduling();
    deer_start();
    set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    init("/dev/initrd/init");
    while (1)
        ;

#else

    kprint("Rhino Kernel Internal Shell version 0.0.5");
    set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprint("\n$");
    enable_scheduling();

    while (1)
    {
        char c[256] = "";
        getline(c, 256);

        user_input(c);
    }

#endif
}

#ifdef DEBUG

void user_input(char *input)
{
    if (strcmp(input, "exit") == 0)
    {
        clear_screen();
        kprint("Shutting down Rhino");
        shutdown();

        return;
    }
    if (strcmp(input, "help") == 0)
    {
        kprint("----------------------------------------------\n");
        kprint("run: Initialize the OS.\n");
        kprint("deer: Start the deer display manager.\n");
        kprint("exit: Exit the Kernel.\n");
        kprint("help: To show this Page.\n");
        kprint("clear: To clear the screen.\n");
        kprint("reboot: To reboot the machine.\n");
#ifdef DEBUG
        kprint("cpuid: Print CPUID info.\n");
        kprint("pid: To show the current PID.\n");
        kprint("panic: Panic the kernel.\n");
        kprint("mmap: Print the sections in the BIOS mmap.\n");
#endif
        kprint("----------------------------------------------\n");
        kprint("$");
        return;
    }
    if (strcmp(input, "clear") == 0)
    {
        clear_screen();
        kprint("$");
        return;
    }
    if (strcmp(input, "panic") == 0)
    {
        PANIC_M("Deliberate Kernel Panic");
    }
    if (strcmp(input, "pid") == 0)
    {
        char c[4] = "";
        int_to_ascii(get_current_pid(), c);
        kprint(c);
        kprint("\n$");
        return;
    }
    if (strcmp(input, "deer") == 0)
    {
        deer_start();
        kprint("$");
        return;
    }
    if (strcmp(input, "mmap") == 0)
    {
        set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
        uint8_t entryCount = 1;
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)(multibootInfo->mmap_addr + KERNEL_VBASE);
        uintptr_t *mmap_end = (uintptr_t *)((multibootInfo->mmap_addr + multibootInfo->mmap_length + KERNEL_VBASE));
        while ((uint32_t)mmap < (uint32_t)mmap_end)
        {
            char buf[128] = "";
            kprint("Entry: ");
            int_to_ascii(entryCount, buf);
            kprint(buf);
            for (uint32_t i = 0; i < 128; i++)
                buf[i] = '\0';
            kprint(" Base: ");
            hex_to_ascii(mmap->addr, buf);
            kprint(buf);
            for (uint32_t i = 0; i < 128; i++)
                buf[i] = '\0';
            kprint(" - Top: ");
            hex_to_ascii(mmap->addr + mmap->len, buf);
            kprint(buf);
            switch (mmap->type)
            {
            case MULTIBOOT_MEMORY_AVAILABLE:
                kprint(" Available\n");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                kprint(" Reserved\n");
                break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                kprint(" ACPI Reclaim\n");
                break;
            case MULTIBOOT_MEMORY_NVS:
                kprint(" Non Volatile Storage\n");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                kprint(" Badram\n");
                break;
            default:
                break;
            }
            for (uint32_t i = 0; i < 128; i++)
                buf[i] = '\0';
            mmap = (multiboot_memory_map_t *)((uintptr_t)mmap + mmap->size + sizeof(uintptr_t));
            entryCount++;
        }
        set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        kprint("$");
        return;
    }
    if (strcmp(input, "reboot") == 0)
        reboot();
    if (strcmp(input, "run") == 0)
    {
        init("/dev/initrd/init");
        kprint("\n$");
        return;
    }
    if (strcmp(input, "cpuid") == 0)
    {
        id_cpu();
        kprint("$");
        return;
    }
    if (strcmp(input, "") == 0)
    {
        kprint("$");
        return;
    }
    kprint(input);
    kprint(" is not an kernel command.");
    kprint("\n$");
}

#endif