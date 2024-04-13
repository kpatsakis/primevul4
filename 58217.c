static int cxusb_lgz201_tuner_attach(struct dvb_usb_adapter *adap)
{
	dvb_attach(dvb_pll_attach, adap->fe_adap[0].fe, 0x61, NULL, DVB_PLL_LG_Z201);
	return 0;
}
