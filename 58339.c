AVCPBProperties *av_cpb_properties_alloc(size_t *size)
{
    AVCPBProperties *props = av_mallocz(sizeof(AVCPBProperties));
    if (!props)
        return NULL;

    if (size)
        *size = sizeof(*props);

    props->vbv_delay = UINT64_MAX;

    return props;
}
