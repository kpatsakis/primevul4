cmsFloat64Number CHAD2Temp(const cmsMAT3* Chad)
{
    cmsVEC3 d, s;
    cmsCIEXYZ Dest;
    cmsCIExyY DestChromaticity;
    cmsFloat64Number TempK;
    cmsMAT3 m1, m2;

    m1 = *Chad;
    if (!_cmsMAT3inverse(&m1, &m2)) return FALSE;

    s.n[VX] = cmsD50_XYZ() -> X;
    s.n[VY] = cmsD50_XYZ() -> Y;
    s.n[VZ] = cmsD50_XYZ() -> Z;

    _cmsMAT3eval(&d, &m2, &s);

    Dest.X = d.n[VX];
    Dest.Y = d.n[VY];
    Dest.Z = d.n[VZ];

    cmsXYZ2xyY(&DestChromaticity, &Dest);

    if (!cmsTempFromWhitePoint(&TempK, &DestChromaticity))
        return -1.0;

    return TempK;
}
