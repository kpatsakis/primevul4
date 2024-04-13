void CMSEXPORT cmsFreeNamedColorList(cmsNAMEDCOLORLIST* v)
{
    if (v == NULL) return;
    if (v ->List) _cmsFree(v ->ContextID, v ->List);
    _cmsFree(v ->ContextID, v);
}
