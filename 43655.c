static void __set_spte(u64 *sptep, u64 spte)
{
	*sptep = spte;
}
