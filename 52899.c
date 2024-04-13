static void git_pack_config(void)
{
	int indexversion_value;
	unsigned long packsizelimit_value;

	if (!git_config_get_ulong("pack.depth", &max_depth)) {
		if (max_depth > MAX_DEPTH)
			max_depth = MAX_DEPTH;
	}
	if (!git_config_get_int("pack.compression", &pack_compression_level)) {
		if (pack_compression_level == -1)
			pack_compression_level = Z_DEFAULT_COMPRESSION;
		else if (pack_compression_level < 0 ||
			 pack_compression_level > Z_BEST_COMPRESSION)
			git_die_config("pack.compression",
					"bad pack compression level %d", pack_compression_level);
		pack_compression_seen = 1;
	}
	if (!git_config_get_int("pack.indexversion", &indexversion_value)) {
		pack_idx_opts.version = indexversion_value;
		if (pack_idx_opts.version > 2)
			git_die_config("pack.indexversion",
					"bad pack.indexversion=%"PRIu32, pack_idx_opts.version);
	}
	if (!git_config_get_ulong("pack.packsizelimit", &packsizelimit_value))
		max_packsize = packsizelimit_value;

	git_config(git_default_config, NULL);
}
