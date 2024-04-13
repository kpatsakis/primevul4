void Eval16nop1D(register const cmsUInt16Number Input[],
                 register cmsUInt16Number Output[],
                 register const struct _cms_interp_struc* p)
{
    Output[0] = Input[0];

    cmsUNUSED_PARAMETER(p);
}
