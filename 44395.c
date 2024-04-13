static inline int __set_dabr(unsigned long dabr, unsigned long dabrx)
{
	mtspr(SPRN_DABR, dabr);
	if (cpu_has_feature(CPU_FTR_DABRX))
		mtspr(SPRN_DABRX, dabrx);
	return 0;
}
