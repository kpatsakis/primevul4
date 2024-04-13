static void fourcc_clear(struct de_fourcc *fourcc)
{
	de_zeromem(fourcc, sizeof(struct de_fourcc));
}