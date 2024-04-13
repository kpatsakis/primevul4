cmsBool _Remove2Op(cmsPipeline* Lut, cmsStageSignature Op1, cmsStageSignature Op2)
{
    cmsStage** pt1;
    cmsStage** pt2;
    cmsBool AnyOpt = FALSE;

    pt1 = &Lut ->Elements;
    if (*pt1 == NULL) return AnyOpt;

    while (*pt1 != NULL) {

        pt2 = &((*pt1) -> Next);
        if (*pt2 == NULL) return AnyOpt;

        if ((*pt1) ->Implements == Op1 && (*pt2) ->Implements == Op2) {
            _RemoveElement(pt2);
            _RemoveElement(pt1);
            AnyOpt = TRUE;
        }
        else
            pt1 = &((*pt1) -> Next);
    }

    return AnyOpt;
}
