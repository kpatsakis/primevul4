int imagetoraw(opj_image_t * image, const char *outfile)
{
    return imagetoraw_common(image, outfile, OPJ_TRUE);
}
