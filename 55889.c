static void scale_component_up(opj_image_comp_t* component,
                               OPJ_UINT32 precision)
{
    OPJ_SIZE_T i, len;

    len = (OPJ_SIZE_T)component->w * (OPJ_SIZE_T)component->h;
    if (component->sgnd) {
        OPJ_INT64  newMax = (OPJ_INT64)(1U << (precision - 1));
        OPJ_INT64  oldMax = (OPJ_INT64)(1U << (component->prec - 1));
        OPJ_INT32* l_data = component->data;
        for (i = 0; i < len; ++i) {
            l_data[i] = (OPJ_INT32)(((OPJ_INT64)l_data[i] * newMax) / oldMax);
        }
    } else {
        OPJ_UINT64  newMax = (OPJ_UINT64)((1U << precision) - 1U);
        OPJ_UINT64  oldMax = (OPJ_UINT64)((1U << component->prec) - 1U);
        OPJ_UINT32* l_data = (OPJ_UINT32*)component->data;
        for (i = 0; i < len; ++i) {
            l_data[i] = (OPJ_UINT32)(((OPJ_UINT64)l_data[i] * newMax) / oldMax);
        }
    }
    component->prec = precision;
    component->bpp = precision;
}
