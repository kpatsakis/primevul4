static int are_comps_similar(opj_image_t * image)
{
    unsigned int i;
    for (i = 1; i < image->numcomps; i++) {
        if (image->comps[0].dx != image->comps[i].dx ||
                image->comps[0].dy != image->comps[i].dy ||
                (i <= 2 &&
                 (image->comps[0].prec != image->comps[i].prec ||
                  image->comps[0].sgnd != image->comps[i].sgnd))) {
            return OPJ_FALSE;
        }
    }
    return OPJ_TRUE;
}
