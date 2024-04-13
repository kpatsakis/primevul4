static inline int __set_dabr(unsigned long dabr, unsigned long dabrx)
{
	mtspr(SPRN_DAC1, dabr);
#ifdef CONFIG_PPC_47x
	isync();
#endif
	return 0;
}
