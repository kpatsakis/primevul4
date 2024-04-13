cmsStage* _cmsStageAllocNamedColor(cmsNAMEDCOLORLIST* NamedColorList, cmsBool UsePCS)
{
    return _cmsStageAllocPlaceholder(NamedColorList ->ContextID,
                                   cmsSigNamedColorElemType,
                                   1, UsePCS ? 3 : NamedColorList ->ColorantCount,
                                   UsePCS ? EvalNamedColorPCS : EvalNamedColor,
                                   DupNamedColorList,
                                   FreeNamedColorList,
                                   cmsDupNamedColorList(NamedColorList));

}
