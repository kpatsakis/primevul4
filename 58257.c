void __ref free_initmem(void)
{
	e820_reallocate_tables();

	free_init_pages("unused kernel",
			(unsigned long)(&__init_begin),
			(unsigned long)(&__init_end));
}
