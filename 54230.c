cmsBool  ComputeAbsoluteIntent(cmsFloat64Number AdaptationState,
                               const cmsCIEXYZ* WhitePointIn,
                               const cmsMAT3* ChromaticAdaptationMatrixIn,
                               const cmsCIEXYZ* WhitePointOut,
                               const cmsMAT3* ChromaticAdaptationMatrixOut,
                               cmsMAT3* m)
{
    cmsMAT3 Scale, m1, m2, m3, m4;

    if (AdaptationState == 1.0) {

        _cmsVEC3init(&m->v[0], WhitePointIn->X / WhitePointOut->X, 0, 0);
        _cmsVEC3init(&m->v[1], 0, WhitePointIn->Y / WhitePointOut->Y, 0);
        _cmsVEC3init(&m->v[2], 0, 0, WhitePointIn->Z / WhitePointOut->Z);

    }
    else  {

        _cmsVEC3init(&Scale.v[0], WhitePointIn->X / WhitePointOut->X, 0, 0);
        _cmsVEC3init(&Scale.v[1], 0,  WhitePointIn->Y / WhitePointOut->Y, 0);
        _cmsVEC3init(&Scale.v[2], 0, 0,  WhitePointIn->Z / WhitePointOut->Z);


        if (AdaptationState == 0.0) {

            m1 = *ChromaticAdaptationMatrixOut;
            _cmsMAT3per(&m2, &m1, &Scale);

            _cmsMAT3per(m, &m2, ChromaticAdaptationMatrixOut);

            m3 = *ChromaticAdaptationMatrixIn;
            if (!_cmsMAT3inverse(&m3, &m4)) return FALSE;
            _cmsMAT3per(m, &m2, &m4);

        } else {

            cmsMAT3 MixedCHAD;
            cmsFloat64Number TempSrc, TempDest, Temp;

            m1 = *ChromaticAdaptationMatrixIn;
            if (!_cmsMAT3inverse(&m1, &m2)) return FALSE;
            _cmsMAT3per(&m3, &m2, &Scale);

            TempSrc  = CHAD2Temp(ChromaticAdaptationMatrixIn);
            TempDest = CHAD2Temp(ChromaticAdaptationMatrixOut);

            if (TempSrc < 0.0 || TempDest < 0.0) return FALSE; // Something went wrong

            if (_cmsMAT3isIdentity(&Scale) && fabs(TempSrc - TempDest) < 0.01) {

                _cmsMAT3identity(m);
                return TRUE;
            }

            Temp = (1.0 - AdaptationState) * TempDest + AdaptationState * TempSrc;

            Temp2CHAD(&MixedCHAD, Temp);

            _cmsMAT3per(m, &m3, &MixedCHAD);
        }

    }
    return TRUE;

}
