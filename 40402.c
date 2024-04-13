static int __init skcipher_module_init(void)
{
	skcipher_default_geniv = num_possible_cpus() > 1 ?
				 "eseqiv" : "chainiv";
	return 0;
}
