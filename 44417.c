static void prime_debug_regs(struct debug_reg *debug)
{
	/*
	 * We could have inherited MSR_DE from userspace, since
	 * it doesn't get cleared on exception entry.  Make sure
	 * MSR_DE is clear before we enable any debug events.
	 */
	mtmsr(mfmsr() & ~MSR_DE);

	mtspr(SPRN_IAC1, debug->iac1);
	mtspr(SPRN_IAC2, debug->iac2);
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
	mtspr(SPRN_IAC3, debug->iac3);
	mtspr(SPRN_IAC4, debug->iac4);
#endif
	mtspr(SPRN_DAC1, debug->dac1);
	mtspr(SPRN_DAC2, debug->dac2);
#if CONFIG_PPC_ADV_DEBUG_DVCS > 0
	mtspr(SPRN_DVC1, debug->dvc1);
	mtspr(SPRN_DVC2, debug->dvc2);
#endif
	mtspr(SPRN_DBCR0, debug->dbcr0);
	mtspr(SPRN_DBCR1, debug->dbcr1);
#ifdef CONFIG_BOOKE
	mtspr(SPRN_DBCR2, debug->dbcr2);
#endif
}
