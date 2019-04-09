#include <rhino/arch/x86/drivers/apic.h>

uint64_t *lapic, ioapic;
MADT *madt;

bool detect_apic()
{
    uint32_t a, c, d;
    cpuid(CPUID_GET_FEATURES, &a, &c, &d);
    if (BIT_IS_SET(d, CPUID_FEAT_EDX_APIC))
    {
        return true;
    }
    return false;
}

static uint8_t apic_isa_pin_polarity(uint8_t IRQ)
{
    for (uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s)
    {
        uint8_t typ = mmio_read8((uint32_t *)ptr);
        if (typ == 0)
        {
            s = 8; // LAPIC
        }
        else if (typ == 1)
        {
            s = 12; // IOAPIC
        }
        else if (typ == 2)
        {
            s = 10;
            if (mmio_read8((uint32_t *)(((uint8_t *)ptr) + 3)) == IRQ)
            {
                uint16_t dat = (mmio_read16((uint32_t *)(((uint16_t *)ptr) + 4)) >> 2) & 3;
                if (dat == 00)
                {
                    return 0;
                }
                else if (dat == 1)
                {
                    return 0;
                }
                else if (dat == 2)
                {
                    return 0;
                }
                else if (dat == 3)
                {
                    return 1;
                }
            }

            // interrupt redirect
        }
        else if (typ == 4)
        {
            s = 10;
            // IOAPIC NMI
        }
    }
    return 0;
}

static uint8_t apic_isa_trigger_mode(uint8_t IRQ)
{
    for (uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s)
    {
        uint8_t typ = mmio_read8((uint32_t *)ptr);
        if (typ == 0)
        {
            s = 8; // LAPIC
        }
        else if (typ == 1)
        {
            s = 12; // IOAPIC
        }
        else if (typ == 2)
        {
            s = 10;
            if (mmio_read8((uint32_t *)(((uint8_t *)ptr) + 3)) == IRQ)
            {
                uint16_t dat = mmio_read16((uint32_t *)(((uint16_t *)ptr) + 4)) & 3;
                if (dat == 00)
                {
                    return 0;
                }
                else if (dat == 1)
                {
                    return 0;
                }
                else if (dat == 2)
                {
                    return 0;
                }
                else if (dat == 3)
                {
                    return 1;
                }
            }

            // interrupt redirect
        }
        else if (typ == 4)
        {
            s = 10;
            // IOAPIC NMI
        }
    }
    return 0;
}

bool init_apic()
{
    if (!detect_apic())
        return false;
    debug_log("[APIC]: Initializing APIC\n");
    uint32_t hi, lo;
    msr_read(APIC_BASE, &lo, &hi);
    lapic = (uint64_t *)(lo & 0xfffff000);
    BIT_SET(lo, 11); // Set APIC enable bit if it wasn't already set
    msr_write(APIC_BASE, lo, hi);

    pic_remap(0, 8); // Reinterpret PIC Spurious Interrupts as exceptions
    pic_disable();
    debug_log("[APIC]: Initializing LAPIC\n");
    init_lapic((uint32_t)lapic);
    uint8_t lapic_id = lapic_get_id();
    debug_log("[APIC]: LAPIC Initialized\n");

    debug_log("[APIC]: Searching for MADT\n");
    madt = find_table(MADT_SIGNATURE);
    if (madt == 0)
    {
        debug_log("[APIC]: MADT Not found\n");
        debug_log("[APIC]: APIC Initializiation failed\n");
        return false;
    }

    for (uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s)
    {
        uint8_t typ = mmio_read8((uint32_t *)ptr);
        if (typ == 0)
        {
            s = 8; // LAPIC
        }
        else if (typ == 1)
        {
            s = 12;
            ioapic = mmio_read32(((uint32_t *)ptr + 1)); // IOAPIC
            continue;
        }
        else if (typ == 2)
        {
            s = 10;
            // interrupt redirect
        }
        else if (typ == 4)
        {
            s = 10;
            // IOAPIC NMI
        }
    }
    debug_log("[APIC]: Initializing IOAPIC\n");
    init_ioapic((uint32_t)ioapic);
    debug_log("[APIC]: IOAPIC Initialized\n");
    debug_log("[APIC]: Setting IOAPIC Redirection Entries\n");
    for (uint8_t i = 0; i < 16; i++)
    { // Enable Legacy PIC Interrupts
        if (i == 2)
            break; // Skip PIC2 Cascade IRQ, it seems to break things
        ioapic_set_entry(apic_irq_redirect(i), (IRQ_BASE + i) | (apic_isa_pin_polarity(i) << 13) | (apic_isa_trigger_mode(i) << 15) | ((uint64_t)lapic_id << 56));
    }
    debug_log("[APIC]: APIC Initialized\n");
    return true;
}

uint32_t apic_irq_redirect(uint8_t IRQ)
{
    for (uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s)
    {
        uint8_t typ = mmio_read8((uint32_t *)ptr);
        if (typ == 0)
        {
            s = 8; // LAPIC
        }
        else if (typ == 1)
        {
            s = 12; // IOAPIC
        }
        else if (typ == 2)
        {
            s = 10;
            if (mmio_read8((uint32_t *)(((uint8_t *)ptr) + 3)) == IRQ)
                return mmio_read32(((uint32_t *)ptr + 1));
            // interrupt redirect
        }
        else if (typ == 4)
        {
            s = 10;
            // IOAPIC NMI
        }
    }
    return IRQ;
}