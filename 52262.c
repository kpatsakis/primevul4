folder_uncompressed_size(struct _7z_folder *f)
{
	int n = (int)f->numOutStreams;
	unsigned pairs = (unsigned)f->numBindPairs;

	while (--n >= 0) {
		unsigned i;
		for (i = 0; i < pairs; i++) {
			if (f->bindPairs[i].outIndex == (uint64_t)n)
				break;
		}
		if (i >= pairs)
			return (f->unPackSize[n]);
	}
	return (0);
}
