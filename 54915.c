static Sdb *store_versioninfo(ELFOBJ *bin) {
	Sdb *sdb_versioninfo = NULL;
	int num_verdef = 0;
	int num_verneed = 0;
	int num_versym = 0;
	int i;

	if (!bin || !bin->shdr) {
		return NULL;
	}
	if (!(sdb_versioninfo = sdb_new0 ())) {
		return NULL;
	}

	for (i = 0; i < bin->ehdr.e_shnum; i++) {
		Sdb *sdb = NULL;
		char key[32] = {0};
		int size = bin->shdr[i].sh_size;

		if (size - (i*sizeof(Elf_(Shdr)) > bin->size)) {
			size = bin->size - (i*sizeof(Elf_(Shdr)));
		}
		int left = size - (i * sizeof (Elf_(Shdr)));
		left = R_MIN (left, bin->shdr[i].sh_size);
		if (left < 0) {
			break;
		}
		switch (bin->shdr[i].sh_type) {
		case SHT_GNU_verdef:
			sdb = store_versioninfo_gnu_verdef (bin, &bin->shdr[i], left);
			snprintf (key, sizeof (key), "verdef%d", num_verdef++);
			sdb_ns_set (sdb_versioninfo, key, sdb);
			break;
		case SHT_GNU_verneed:
			sdb = store_versioninfo_gnu_verneed (bin, &bin->shdr[i], left);
			snprintf (key, sizeof (key), "verneed%d", num_verneed++);
			sdb_ns_set (sdb_versioninfo, key, sdb);
			break;
		case SHT_GNU_versym:
			sdb = store_versioninfo_gnu_versym (bin, &bin->shdr[i], left);
			snprintf (key, sizeof (key), "versym%d", num_versym++);
			sdb_ns_set (sdb_versioninfo, key, sdb);
			break;
		}
	}

	return sdb_versioninfo;
}
