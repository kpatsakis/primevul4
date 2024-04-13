static OPJ_BOOL bmp_read_raw_data(FILE* IN, OPJ_UINT8* pData, OPJ_UINT32 stride,
                                  OPJ_UINT32 width, OPJ_UINT32 height)
{
    OPJ_ARG_NOT_USED(width);

    if (fread(pData, sizeof(OPJ_UINT8), stride * height, IN) != (stride * height)) {
        fprintf(stderr,
                "\nError: fread return a number of element different from the expected.\n");
        return OPJ_FALSE;
    }
    return OPJ_TRUE;
}
