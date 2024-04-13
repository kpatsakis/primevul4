free_Folder(struct _7z_folder *f)
{
	unsigned i;

	if (f->coders) {
		for (i = 0; i< f->numCoders; i++) {
			free(f->coders[i].properties);
		}
		free(f->coders);
	}
	free(f->bindPairs);
	free(f->packedStreams);
	free(f->unPackSize);
}
