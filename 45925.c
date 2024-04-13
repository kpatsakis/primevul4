cmsBool _Remove1Op(cmsPipeline* Lut, cmsStageSignature UnaryOp)
{
    cmsStage** pt = &Lut ->Elements;
    cmsBool AnyOpt = FALSE;

    while (*pt != NULL) {

        if ((*pt) ->Implements == UnaryOp) {
            _RemoveElement(pt);
            AnyOpt = TRUE;
        }
        else
            pt = &((*pt) -> Next);
    }

    return AnyOpt;
}
