static int mxf_metadataset_init(MXFMetadataSet *ctx, enum MXFMetadataSetType type)
{
    switch (type){
    case MultipleDescriptor:
    case Descriptor:
        ((MXFDescriptor*)ctx)->pix_fmt = AV_PIX_FMT_NONE;
        ((MXFDescriptor*)ctx)->duration = AV_NOPTS_VALUE;
        break;
    default:
        break;
    }
    return 0;
}
