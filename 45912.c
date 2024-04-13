cmsBool PreOptimize(cmsPipeline* Lut)
{
    cmsBool AnyOpt = FALSE, Opt;

    do {

        Opt = FALSE;

        Opt |= _Remove1Op(Lut, cmsSigIdentityElemType);

        Opt |= _Remove2Op(Lut, cmsSigXYZ2LabElemType, cmsSigLab2XYZElemType);

        Opt |= _Remove2Op(Lut, cmsSigLab2XYZElemType, cmsSigXYZ2LabElemType);

        Opt |= _Remove2Op(Lut, cmsSigLabV4toV2, cmsSigLabV2toV4);

        Opt |= _Remove2Op(Lut, cmsSigLabV2toV4, cmsSigLabV4toV2);

        Opt |= _Remove2Op(Lut, cmsSigLab2FloatPCS, cmsSigFloatPCS2Lab);

        Opt |= _Remove2Op(Lut, cmsSigXYZ2FloatPCS, cmsSigFloatPCS2XYZ);

        if (Opt) AnyOpt = TRUE;

    } while (Opt);

    return AnyOpt;
}
