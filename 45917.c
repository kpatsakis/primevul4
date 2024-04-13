void PrelinEval8(register const cmsUInt16Number Input[],
                  register cmsUInt16Number Output[],
                  register const void* D)
{

    cmsUInt8Number         r, g, b;
    cmsS15Fixed16Number    rx, ry, rz;
    cmsS15Fixed16Number    c0, c1, c2, c3, Rest;
    int                    OutChan;
    register cmsS15Fixed16Number    X0, X1, Y0, Y1, Z0, Z1;
    Prelin8Data* p8 = (Prelin8Data*) D;
    register const cmsInterpParams* p = p8 ->p;
    int                    TotalOut = p -> nOutputs;
    const cmsUInt16Number* LutTable = p -> Table;

    r = Input[0] >> 8;
    g = Input[1] >> 8;
    b = Input[2] >> 8;

    X0 = X1 = p8->X0[r];
    Y0 = Y1 = p8->Y0[g];
    Z0 = Z1 = p8->Z0[b];

    rx = p8 ->rx[r];
    ry = p8 ->ry[g];
    rz = p8 ->rz[b];

    X1 = X0 + ((rx == 0) ? 0 : p ->opta[2]);
    Y1 = Y0 + ((ry == 0) ? 0 : p ->opta[1]);
    Z1 = Z0 + ((rz == 0) ? 0 : p ->opta[0]);


    for (OutChan=0; OutChan < TotalOut; OutChan++) {

        c0 = DENS(X0, Y0, Z0);

        if (rx >= ry && ry >= rz)
        {
            c1 = DENS(X1, Y0, Z0) - c0;
            c2 = DENS(X1, Y1, Z0) - DENS(X1, Y0, Z0);
            c3 = DENS(X1, Y1, Z1) - DENS(X1, Y1, Z0);
        }
        else
            if (rx >= rz && rz >= ry)
            {
                c1 = DENS(X1, Y0, Z0) - c0;
                c2 = DENS(X1, Y1, Z1) - DENS(X1, Y0, Z1);
                c3 = DENS(X1, Y0, Z1) - DENS(X1, Y0, Z0);
            }
            else
                if (rz >= rx && rx >= ry)
                {
                    c1 = DENS(X1, Y0, Z1) - DENS(X0, Y0, Z1);
                    c2 = DENS(X1, Y1, Z1) - DENS(X1, Y0, Z1);
                    c3 = DENS(X0, Y0, Z1) - c0;
                }
                else
                    if (ry >= rx && rx >= rz)
                    {
                        c1 = DENS(X1, Y1, Z0) - DENS(X0, Y1, Z0);
                        c2 = DENS(X0, Y1, Z0) - c0;
                        c3 = DENS(X1, Y1, Z1) - DENS(X1, Y1, Z0);
                    }
                    else
                        if (ry >= rz && rz >= rx)
                        {
                            c1 = DENS(X1, Y1, Z1) - DENS(X0, Y1, Z1);
                            c2 = DENS(X0, Y1, Z0) - c0;
                            c3 = DENS(X0, Y1, Z1) - DENS(X0, Y1, Z0);
                        }
                        else
                            if (rz >= ry && ry >= rx)
                            {
                                c1 = DENS(X1, Y1, Z1) - DENS(X0, Y1, Z1);
                                c2 = DENS(X0, Y1, Z1) - DENS(X0, Y0, Z1);
                                c3 = DENS(X0, Y0, Z1) - c0;
                            }
                            else  {
                                c1 = c2 = c3 = 0;
                            }


                            Rest = c1 * rx + c2 * ry + c3 * rz + 0x8001;
                            Output[OutChan] = (cmsUInt16Number)c0 + ((Rest + (Rest>>16))>>16);

    }
}
