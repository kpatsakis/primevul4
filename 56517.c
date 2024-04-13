static u32 get_ccsidr(u32 csselr)
{
	u32 ccsidr;

	/* Make sure noone else changes CSSELR during this! */
	local_irq_disable();
	write_sysreg(csselr, csselr_el1);
	isb();
	ccsidr = read_sysreg(ccsidr_el1);
	local_irq_enable();

	return ccsidr;
}
