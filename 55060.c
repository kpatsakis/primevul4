int main () {
	RFlagItem *i;
	RFlag *f = r_flag_new ();
	r_flag_set (f, "rip", 0xfff333999000LL, 1);
	r_flag_set (f, "rip", 0xfff333999002LL, 1);
	r_flag_unset (f, "rip", NULL);
	r_flag_set (f, "rip", 3, 4);
	r_flag_set (f, "rip", 4, 4);
	r_flag_set (f, "corwp", 300, 4);
	r_flag_set (f, "barp", 300, 4);
	r_flag_set (f, "rip", 3, 4);
	r_flag_set (f, "rip", 4, 4);

	i = r_flag_get (f, "rip");
	if (i) printf ("nRIP: %p %llx\n", i, i->offset);
	else printf ("nRIP: null\n");

	i = r_flag_get_i (f, 0xfff333999000LL);
	if (i) printf ("iRIP: %p %llx\n", i, i->offset);
	else printf ("iRIP: null\n");
}
