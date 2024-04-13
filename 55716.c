OPJ_BOOL opj_tcd_copy_tile_data(opj_tcd_t *p_tcd,
                                OPJ_BYTE * p_src,
                                OPJ_UINT32 p_src_length)
{
    OPJ_UINT32 i, j, l_data_size = 0;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcd_tilecomp_t * l_tilec = 00;
    OPJ_UINT32 l_size_comp, l_remaining;
    OPJ_UINT32 l_nb_elem;

    l_data_size = opj_tcd_get_encoded_tile_size(p_tcd);
    if (l_data_size != p_src_length) {
        return OPJ_FALSE;
    }

    l_tilec = p_tcd->tcd_image->tiles->comps;
    l_img_comp = p_tcd->image->comps;
    for (i = 0; i < p_tcd->image->numcomps; ++i) {
        l_size_comp = l_img_comp->prec >> 3; /*(/ 8)*/
        l_remaining = l_img_comp->prec & 7;  /* (%8) */
        l_nb_elem = (OPJ_UINT32)((l_tilec->x1 - l_tilec->x0) * (l_tilec->y1 -
                                 l_tilec->y0));

        if (l_remaining) {
            ++l_size_comp;
        }

        if (l_size_comp == 3) {
            l_size_comp = 4;
        }

        switch (l_size_comp) {
        case 1: {
            OPJ_CHAR * l_src_ptr = (OPJ_CHAR *) p_src;
            OPJ_INT32 * l_dest_ptr = l_tilec->data;

            if (l_img_comp->sgnd) {
                for (j = 0; j < l_nb_elem; ++j) {
                    *(l_dest_ptr++) = (OPJ_INT32)(*(l_src_ptr++));
                }
            } else {
                for (j = 0; j < l_nb_elem; ++j) {
                    *(l_dest_ptr++) = (*(l_src_ptr++)) & 0xff;
                }
            }

            p_src = (OPJ_BYTE*) l_src_ptr;
        }
        break;
        case 2: {
            OPJ_INT32 * l_dest_ptr = l_tilec->data;
            OPJ_INT16 * l_src_ptr = (OPJ_INT16 *) p_src;

            if (l_img_comp->sgnd) {
                for (j = 0; j < l_nb_elem; ++j) {
                    *(l_dest_ptr++) = (OPJ_INT32)(*(l_src_ptr++));
                }
            } else {
                for (j = 0; j < l_nb_elem; ++j) {
                    *(l_dest_ptr++) = (*(l_src_ptr++)) & 0xffff;
                }
            }

            p_src = (OPJ_BYTE*) l_src_ptr;
        }
        break;
        case 4: {
            OPJ_INT32 * l_src_ptr = (OPJ_INT32 *) p_src;
            OPJ_INT32 * l_dest_ptr = l_tilec->data;

            for (j = 0; j < l_nb_elem; ++j) {
                *(l_dest_ptr++) = (OPJ_INT32)(*(l_src_ptr++));
            }

            p_src = (OPJ_BYTE*) l_src_ptr;
        }
        break;
        }

        ++l_img_comp;
        ++l_tilec;
    }

    return OPJ_TRUE;
}
