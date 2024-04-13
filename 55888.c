void scale_component(opj_image_comp_t* component, OPJ_UINT32 precision)
{
    int shift;
    OPJ_SIZE_T i, len;

    if (component->prec == precision) {
        return;
    }
    if (component->prec < precision) {
        scale_component_up(component, precision);
        return;
    }
    shift = (int)(component->prec - precision);
    len = (OPJ_SIZE_T)component->w * (OPJ_SIZE_T)component->h;
    if (component->sgnd) {
        OPJ_INT32* l_data = component->data;
        for (i = 0; i < len; ++i) {
            l_data[i] >>= shift;
        }
    } else {
        OPJ_UINT32* l_data = (OPJ_UINT32*)component->data;
        for (i = 0; i < len; ++i) {
            l_data[i] >>= shift;
        }
    }
    component->bpp = precision;
    component->prec = precision;
}
