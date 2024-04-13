static inline void inc(be128 *iv)
{
	be64_add_cpu(&iv->b, 1);
	if (!iv->b)
		be64_add_cpu(&iv->a, 1);
}
