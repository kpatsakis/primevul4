static OPJ_BOOL bmp_read_file_header(FILE* IN, OPJ_BITMAPFILEHEADER* header)
{
    header->bfType  = (OPJ_UINT16)getc(IN);
    header->bfType |= (OPJ_UINT16)((OPJ_UINT32)getc(IN) << 8);

    if (header->bfType != 19778) {
        fprintf(stderr, "Error, not a BMP file!\n");
        return OPJ_FALSE;
    }

    /* FILE HEADER */
    /* ------------- */
    header->bfSize  = (OPJ_UINT32)getc(IN);
    header->bfSize |= (OPJ_UINT32)getc(IN) << 8;
    header->bfSize |= (OPJ_UINT32)getc(IN) << 16;
    header->bfSize |= (OPJ_UINT32)getc(IN) << 24;

    header->bfReserved1  = (OPJ_UINT16)getc(IN);
    header->bfReserved1 |= (OPJ_UINT16)((OPJ_UINT32)getc(IN) << 8);

    header->bfReserved2  = (OPJ_UINT16)getc(IN);
    header->bfReserved2 |= (OPJ_UINT16)((OPJ_UINT32)getc(IN) << 8);

    header->bfOffBits  = (OPJ_UINT32)getc(IN);
    header->bfOffBits |= (OPJ_UINT32)getc(IN) << 8;
    header->bfOffBits |= (OPJ_UINT32)getc(IN) << 16;
    header->bfOffBits |= (OPJ_UINT32)getc(IN) << 24;
    return OPJ_TRUE;
}
