static void set_debug_reg_defaults(struct thread_struct *thread)
{
	thread->debug.iac1 = thread->debug.iac2 = 0;
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
	thread->debug.iac3 = thread->debug.iac4 = 0;
#endif
	thread->debug.dac1 = thread->debug.dac2 = 0;
#if CONFIG_PPC_ADV_DEBUG_DVCS > 0
	thread->debug.dvc1 = thread->debug.dvc2 = 0;
#endif
	thread->debug.dbcr0 = 0;
#ifdef CONFIG_BOOKE
	/*
	 * Force User/Supervisor bits to b11 (user-only MSR[PR]=1)
	 */
	thread->debug.dbcr1 = DBCR1_IAC1US | DBCR1_IAC2US |
			DBCR1_IAC3US | DBCR1_IAC4US;
	/*
	 * Force Data Address Compare User/Supervisor bits to be User-only
	 * (0b11 MSR[PR]=1) and set all other bits in DBCR2 register to be 0.
	 */
	thread->debug.dbcr2 = DBCR2_DAC1US | DBCR2_DAC2US;
#else
	thread->debug.dbcr1 = 0;
#endif
}
