#include <rhino/panic.h>

__attribute__((noreturn)) void panic_int(char *file, char *func, uint32_t line)
{
    asm("cli");
    kprint_err("KERNEL PANIC!\n");
    kprint_err("PLEASE REBOOT SYSTEM!\n");
    kprint_err(file);
    kprint_err(" -> ");
    kprint_err(func);
    kprint_err("() : ");
    char c[5];
    int_to_ascii(line, c);
    kprint_err(c);
    while (1)
    {
    }
}

__attribute__((noreturn)) void panic_m(char *m, char *file, char *func, uint32_t line)
{
    asm("cli");
    kprint_err("KERNEL PANIC!\n");
    kprint_err("PLEASE REBOOT SYSTEM!\n");
    kprint_err(file);
    kprint_err(" -> ");
    kprint_err(func);
    kprint_err("() : ");
    char c[5];
    int_to_ascii(line, c);
    kprint_err(c);
    kprint("\n");
    kprint_err(m);
    while (1)
    {
    }
}

__attribute__((noreturn)) void panic_assert(char *file, char *func, uint32_t line, char *desc)
{
    asm("cli");

    kprint_err("ASSERTION-FAILED(");
    kprint_err(desc);
    kprint_err(") at ");
    kprint_err(file);
    kprint_err(" -> ");
    kprint_err(func);
    kprint_err("() : ");
    char c[5];
    int_to_ascii(line, c);
    kprint_err(c);
    while (1)
    {
    }
}
