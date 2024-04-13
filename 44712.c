static void floppy_release_allocated_regions(int fdc, const struct io_region *p)
{
	while (p != io_regions) {
		p--;
		release_region(FDCS->address + p->offset, p->size);
	}
}
