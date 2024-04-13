static OPJ_BOOL opj_tcd_dc_level_shift_decode(opj_tcd_t *p_tcd)
{
    OPJ_UINT32 compno;
    opj_tcd_tilecomp_t * l_tile_comp = 00;
    opj_tccp_t * l_tccp = 00;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcd_resolution_t* l_res = 00;
    opj_tcd_tile_t * l_tile;
    OPJ_UINT32 l_width, l_height, i, j;
    OPJ_INT32 * l_current_ptr;
    OPJ_INT32 l_min, l_max;
    OPJ_UINT32 l_stride;

    l_tile = p_tcd->tcd_image->tiles;
    l_tile_comp = l_tile->comps;
    l_tccp = p_tcd->tcp->tccps;
    l_img_comp = p_tcd->image->comps;

    for (compno = 0; compno < l_tile->numcomps; compno++) {
        l_res = l_tile_comp->resolutions + l_img_comp->resno_decoded;
        l_width = (OPJ_UINT32)(l_res->x1 - l_res->x0);
        l_height = (OPJ_UINT32)(l_res->y1 - l_res->y0);
        l_stride = (OPJ_UINT32)(l_tile_comp->x1 - l_tile_comp->x0) - l_width;

        assert(l_height == 0 ||
               l_width + l_stride <= l_tile_comp->data_size / l_height); /*MUPDF*/

        if (l_img_comp->sgnd) {
            l_min = -(1 << (l_img_comp->prec - 1));
            l_max = (1 << (l_img_comp->prec - 1)) - 1;
        } else {
            l_min = 0;
            l_max = (OPJ_INT32)((1U << l_img_comp->prec) - 1);
        }

        l_current_ptr = l_tile_comp->data;

        if (l_tccp->qmfbid == 1) {
            for (j = 0; j < l_height; ++j) {
                for (i = 0; i < l_width; ++i) {
                    *l_current_ptr = opj_int_clamp(*l_current_ptr + l_tccp->m_dc_level_shift, l_min,
                                                   l_max);
                    ++l_current_ptr;
                }
                l_current_ptr += l_stride;
            }
        } else {
            for (j = 0; j < l_height; ++j) {
                for (i = 0; i < l_width; ++i) {
                    OPJ_FLOAT32 l_value = *((OPJ_FLOAT32 *) l_current_ptr);
                    OPJ_INT32 l_value_int = (OPJ_INT32)opj_lrintf(l_value);
                    if (l_value > INT_MAX ||
                            (l_value_int > 0 && l_tccp->m_dc_level_shift > 0 &&
                             l_value_int > INT_MAX - l_tccp->m_dc_level_shift)) {
                        *l_current_ptr = l_max;
                    } else {
                        *l_current_ptr = opj_int_clamp(
                                             l_value_int + l_tccp->m_dc_level_shift, l_min, l_max);
                    }
                    ++l_current_ptr;
                }
                l_current_ptr += l_stride;
            }
        }

        ++l_img_comp;
        ++l_tccp;
        ++l_tile_comp;
    }

    return OPJ_TRUE;
}
