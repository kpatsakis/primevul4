TIFFInitNeXT(TIFF* tif, int scheme)
{
	(void) scheme;
	tif->tif_predecode = NeXTPreDecode;  
	tif->tif_decoderow = NeXTDecode;  
	tif->tif_decodestrip = NeXTDecode;  
	tif->tif_decodetile = NeXTDecode;
	return (1);
}
