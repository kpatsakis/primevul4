    void Temp2CHAD(cmsMAT3* Chad, cmsFloat64Number Temp)
{
    cmsCIEXYZ White;
    cmsCIExyY ChromaticityOfWhite;

    cmsWhitePointFromTemp(&ChromaticityOfWhite, Temp);
    cmsxyY2XYZ(&White, &ChromaticityOfWhite);
    _cmsAdaptationMatrix(Chad, NULL, &White, cmsD50_XYZ());
}
