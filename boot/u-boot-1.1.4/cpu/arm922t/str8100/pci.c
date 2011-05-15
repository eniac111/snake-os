#include <common.h>

#ifdef CONFIG_PCI

#include <asm/processor.h>
#include <asm/io.h>
#include <pci.h>
#include <asm/arch/star_pci_bridge.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_powermgt.h>

#define CONFIG_CMD(bus, device, function, where) \
	(0x80000000 | ((bus) << 16) | ((device) << 11) | ((function) << 8) | ((where) & ~3))

static u32 pci_config_addr;
static u32 pci_config_data;
static u32 str8100_pci_irq_swizzle[4] = {0, INTC_PCI_INTA_BIT_INDEX, INTC_PCI_INTB_BIT_INDEX, 0};
static pci_dev_t pci_bridge;
static struct pci_controller str8100_pci_hose;

#if 0
static void str8100_pci_config_bridge(struct pci_controller *hose,
	pci_dev_t dev, struct pci_config_table *entry)
{
	u32 cmdstat = 0;

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0x0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, 0x0);

	pci_read_config_dword(dev, PCI_COMMAND, &cmdstat);
        cmdstat = cmdstat | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config_dword(dev, PCI_COMMAND, cmdstat);
	pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE, 0x08);
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0x80);
}

static struct pci_config_table str8100_pci_config_table[] = {
	{ PCIB_VENDOR_ID, PCIB_DEVICE_ID, PCI_ANY_ID,
	  PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
	  str8100_pci_config_bridge },
	{}
};
#endif

// config resources for bridge
// the bridge is located at 0:0:0
static void str8100_pci_config_bridge(void)
{
	u32 cmdstat = 0;

	pci_write_config_dword(0, PCI_BASE_ADDRESS_0, 0x0);
	pci_write_config_dword(0, PCI_BASE_ADDRESS_1, 0x0);

	pci_read_config_dword(0, PCI_COMMAND, &cmdstat);
	cmdstat = cmdstat | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config_dword(0, PCI_COMMAND, cmdstat);
	pci_write_config_byte(0, PCI_CACHE_LINE_SIZE, 0x08);
	pci_write_config_byte(0, PCI_LATENCY_TIMER, 0x80);
}

static void str8100_pci_fixup_irq(struct pci_controller *hose, pci_dev_t dev)
{
	unsigned char irq;
	unsigned char slot;

	// PCI Bridge, no need to assign irq
	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) == 0 && PCI_FUNC(dev) == 0)
		return;

	//printf("BUS:%02x DEVICE:%02x FUNCTION:%02x\n", PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));

	slot = 2;

	irq = str8100_pci_irq_swizzle[slot];

	pci_write_config_byte(dev, PCI_INTERRUPT_LINE, irq);
}

static inline u32 str8100_pci_read_config(pci_dev_t dev, int where, int size)
{
	u32 v;
	u32 shift;

	__raw_writel(CONFIG_CMD(PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where), pci_config_addr);
	v = __raw_readl(pci_config_data);

	switch (size) {
	case 1:
		shift = (where & 0x3) << 3;
		v = (v >> shift) & 0xff;
		break;
	case 2:
		shift = (where & 0x3) << 3;
		v = (v >> shift) & 0xffff;
		break;
	case 4:
		break;
	default:
		v = 0xffffffff;
		break;
	}

	return v;
}

static inline void str8100_pci_write_config(pci_dev_t dev, int where, u32 val, int size)
{
	u32 shift;
	u32 orig_val;

	switch (size) {
	case 1:
		__raw_writel(CONFIG_CMD(PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where), pci_config_addr);
		orig_val = __raw_readl(pci_config_data);
		shift = (where & 0x3) << 3;
		orig_val = (orig_val & ~(0xff << shift)) | (val << shift);
		__raw_writel(orig_val, pci_config_data);
		break;
	case 2:
		__raw_writel(CONFIG_CMD(PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where), pci_config_addr);
		orig_val = __raw_readl(pci_config_data);
		shift = (where & 0x3) << 3;
		orig_val = (orig_val & ~(0xffff << shift)) | (val << shift);
		__raw_writel(orig_val, pci_config_data);
		break;
	case 4:
		__raw_writel(CONFIG_CMD(PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where), pci_config_addr);
		__raw_writel(val, pci_config_data);
		break;
	default:
		break;
	}
}

int str8100_pci_hose_read_config_byte(struct pci_controller *hose, pci_dev_t dev, int where, u8 *val)
{
	*val = (u8)str8100_pci_read_config(dev, where, 1);
	return 0;
}

int str8100_pci_hose_read_config_word(struct pci_controller *hose, pci_dev_t dev, int where, u16 *val)
{
	*val = (u16)str8100_pci_read_config(dev, where, 2);
	return 0;
}

int str8100_pci_hose_read_config_dword(struct pci_controller *hose, pci_dev_t dev, int where, u32 *val)
{
	*val = (u32)str8100_pci_read_config(dev, where, 4);
	return 0;
}

int str8100_pci_hose_write_config_byte(struct pci_controller *hose, pci_dev_t dev, int where, u8 val)
{
	str8100_pci_write_config(dev, where, val, 1);
	return 0;
}

int str8100_pci_hose_write_config_word(struct pci_controller *hose, pci_dev_t dev, int where, u16 val)
{
	str8100_pci_write_config(dev, where, val, 2);
	return 0;
}

int str8100_pci_hose_write_config_dword(struct pci_controller *hose, pci_dev_t dev, int where, u32 val)
{
	str8100_pci_write_config(dev, where, val, 4);
	return 0;
}

static void str8100_pci_init(void)
{
	struct pci_controller *hose = &str8100_pci_hose;

	HAL_MISC_ENABLE_PCI_PINS();
	HAL_PWRMGT_ENABLE_PCI_BRIDGE_33M_CLOCK();

	pci_config_addr = SYSPA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR + PCI_BRIDGE_CONFIG_ADDR_REG_OFFSET;
	pci_config_data = SYSPA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR + PCI_BRIDGE_CONFIG_DATA_REG_OFFSET;

	hose->first_busno = 0;
	hose->last_busno = 0;

	/* pci memory space */
	pci_set_region(hose->regions + 0,
		PCI_MEMORY_SPACE_BASE_ADDR,
		PCI_MEMORY_SPACE_BASE_ADDR,
		PCI_MEMORY_SPACE_SIZE,
		PCI_REGION_MEM);

	/* pci I/O space */
	pci_set_region(hose->regions + 1,
		PCI_IO_SPACE_BASE_ADDR,
		PCI_IO_SPACE_BASE_ADDR,
		PCI_IO_SPACE_SIZE,
		PCI_REGION_IO);

	hose->region_count = 2;
	pci_set_ops(hose,
		str8100_pci_hose_read_config_byte,
		str8100_pci_hose_read_config_word,
		str8100_pci_hose_read_config_dword,
		str8100_pci_hose_write_config_byte,
		str8100_pci_hose_write_config_word,
		str8100_pci_hose_write_config_dword);
#if 0
	hose->config_table = str8100_pci_config_table;
#endif
	hose->fixup_irq = str8100_pci_fixup_irq;
	pci_register_hose(hose);
	hose->last_busno = pci_hose_scan(hose);
	str8100_pci_config_bridge();
}

void pci_init_board(void)   
{
	str8100_pci_init();
}

#endif	/* CONFIG_PCI */
