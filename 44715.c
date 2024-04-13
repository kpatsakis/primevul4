static int floppy_request_regions(int fdc)
{
	const struct io_region *p;

	for (p = io_regions; p < ARRAY_END(io_regions); p++) {
		if (!request_region(FDCS->address + p->offset,
				    p->size, "floppy")) {
			DPRINT("Floppy io-port 0x%04lx in use\n",
			       FDCS->address + p->offset);
			floppy_release_allocated_regions(fdc, p);
			return -EBUSY;
		}
	}
	return 0;
}
