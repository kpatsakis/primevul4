static int __init floppy_setup(char *str)
{
	int i;
	int param;
	int ints[11];

	str = get_options(str, ARRAY_SIZE(ints), ints);
	if (str) {
		for (i = 0; i < ARRAY_SIZE(config_params); i++) {
			if (strcmp(str, config_params[i].name) == 0) {
				if (ints[0])
					param = ints[1];
				else
					param = config_params[i].def_param;
				if (config_params[i].fn)
					config_params[i].fn(ints, param,
							    config_params[i].
							    param2);
				if (config_params[i].var) {
					DPRINT("%s=%d\n", str, param);
					*config_params[i].var = param;
				}
				return 1;
			}
		}
	}
	if (str) {
		DPRINT("unknown floppy option [%s]\n", str);

		DPRINT("allowed options are:");
		for (i = 0; i < ARRAY_SIZE(config_params); i++)
			pr_cont(" %s", config_params[i].name);
		pr_cont("\n");
	} else
		DPRINT("botched floppy option\n");
	DPRINT("Read Documentation/blockdev/floppy.txt\n");
	return 0;
}
