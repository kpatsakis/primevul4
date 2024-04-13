static AVStream* mxf_get_opatom_stream(MXFContext *mxf)
{
    int i;

    if (mxf->op != OPAtom)
        return NULL;

    for (i = 0; i < mxf->fc->nb_streams; i++) {
        if (mxf->fc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_DATA)
            continue;
        return mxf->fc->streams[i];
    }
    return NULL;
}
