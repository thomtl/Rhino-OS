#include <rhino/arch/x86/drivers/dma.h>

void dma_set_address(uint8_t channel, uint8_t low, uint8_t high){
  if(channel > 8) return;

  uint16_t port = 0;
  switch (channel) {
    case 0:
      port = DMA0_CHAN0_ADDR_REG;
      break;
    case 1:
      port = DMA0_CHAN1_ADDR_REG;
      break;
    case 2:
      port = DMA0_CHAN2_ADDR_REG;
      break;
    case 3:
      port = DMA0_CHAN3_ADDR_REG;
      break;
    case 4:
      port = DMA1_CHAN4_ADDR_REG;
      break;
    case 5:
      port = DMA1_CHAN5_ADDR_REG;
      break;
    case 6:
      port = DMA1_CHAN6_ADDR_REG;
      break;
    case 7:
      port = DMA1_CHAN7_ADDR_REG;
      break;
    default:
      return;
  }

  outb(port, low);
  outb(port, high);
}


void dma_set_count(uint8_t channel, uint8_t low, uint8_t high){
  if(channel > 8) return;

  uint16_t port = 0;
  switch (channel) {
    case 0:
      port = DMA0_CHAN0_COUNT_REG;
      break;
    case 1:
      port = DMA0_CHAN1_COUNT_REG;
      break;
    case 2:
      port = DMA0_CHAN2_COUNT_REG;
      break;
    case 3:
      port = DMA0_CHAN3_COUNT_REG;
      break;
    case 4:
      port = DMA1_CHAN4_COUNT_REG;
      break;
    case 5:
      port = DMA1_CHAN5_COUNT_REG;
      break;
    case 6:
      port = DMA1_CHAN6_COUNT_REG;
      break;
    case 7:
      port = DMA1_CHAN7_COUNT_REG;
      break;
    default:
      return;
  }

  outb(port, low);
  outb(port, high);
}

void dma_set_external_page_register(uint8_t reg, uint8_t val){
  if(reg > 14) return;

  uint16_t port = 0;
  switch (reg) {
    case 1:
      port = DMA_PAGE_CHAN1_ADDRBYTE2;
      break;
    case 2:
      port = DMA_PAGE_CHAN2_ADDRBYTE2;
      break;
    case 3:
      port = DMA_PAGE_CHAN3_ADDRBYTE2;
      break;
    case 4:
      return;
    case 5:
      port = DMA_PAGE_CHAN5_ADDRBYTE2;
      break;
    case 6:
      port = DMA_PAGE_CHAN6_ADDRBYTE2;
      break;
    case 7:
      port = DMA_PAGE_CHAN7_ADDRBYTE2;
      break;
    default:
      return;
  }

  outb(port, val);
}

void dma_set_mode(uint8_t channel, uint8_t mode){
    int dma = (channel < 4) ? 0 : 1;
    int chan = (dma == 0) ? channel : channel - 4;

    dma_mask_channel(channel);
    outb((channel < 4) ? (DMA0_MODE_REG) : DMA1_MODE_REG, chan | (mode));
    dma_unmask_all(dma);
}

void dma_set_read(uint8_t channel){
  dma_set_mode(channel, DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_set_write(uint8_t channel){
  dma_set_mode(channel, DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_mask_channel(uint8_t channel){
  if (channel <= 4) outb(DMA0_CHANMASK_REG, (1 << (channel - 1)));
  else outb(DMA1_CHANMASK_REG, (1 << (channel - 5)));
}

void dma_unmask_channel(uint8_t channel){
  if (channel <= 4) outb(DMA0_CHANMASK_REG, channel);
  else outb(DMA1_CHANMASK_REG, channel);
}

void dma_reset_flipflop(uint8_t dma){
  if(dma > 2) return;
  outb((dma == 0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xFF);
}

void dma_reset(uint8_t dma){
  if(dma > 2) return;
  outb((dma == 0) ? 0x0D : 0xD8, 0xFF);
}

void dma_unmask_all(uint8_t dma){
  if(dma > 2) return;
  outb((dma == 0) ? 0x0E : 0xDC, 0xFF);
}

bool dma_initialize_floppy(uint8_t* buffer, uint32_t length){
  union {
    uint8_t byte[4];
    uint32_t l;
  } a, c;
  a.l = (uint32_t)buffer;
  c.l = length-1;
  if ((a.l >> 24) || (c.l >> 16) || (((a.l & 0xffff)+c.l) >> 16)){
    return false;
  }

  dma_reset(1);
  dma_mask_channel(2);
  dma_reset_flipflop(1);
  dma_set_address(2, a.byte[0], a.byte[1]);
  dma_reset_flipflop(1);
  dma_set_count(2, c.byte[0], c.byte[1]);
  dma_set_read(2);
  dma_unmask_all(1);
  return true;
}
