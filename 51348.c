void free_tga(oTga * tga)
{
	if (tga) {
		if (tga->ident)
			gdFree(tga->ident);
		if (tga->bitmap)
			gdFree(tga->bitmap);
		gdFree(tga);
	}
}
