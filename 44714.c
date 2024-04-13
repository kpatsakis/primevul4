static void floppy_release_regions(int fdc)
{
	floppy_release_allocated_regions(fdc, ARRAY_END(io_regions));
}
