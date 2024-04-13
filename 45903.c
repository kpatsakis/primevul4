void FillSecondShaper(cmsUInt16Number* Table, cmsToneCurve* Curve, cmsBool Is8BitsOutput)
{
    int i;
    cmsFloat32Number R, Val;

    for (i=0; i < 16385; i++) {

        R   = (cmsFloat32Number) (i / 16384.0);
        Val = cmsEvalToneCurveFloat(Curve, R);    // Val comes 0..1.0

        if (Is8BitsOutput) {

            cmsUInt16Number w = _cmsQuickSaturateWord(Val * 65535.0);
            cmsUInt8Number  b = FROM_16_TO_8(w);

            Table[i] = FROM_8_TO_16(b);
        }
        else Table[i]  = _cmsQuickSaturateWord(Val * 65535.0);
    }
}
