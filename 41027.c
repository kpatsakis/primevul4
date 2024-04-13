cfb_ofb_mode (const struct key_type* kt)
{
  if (kt && kt->cipher) {
      const unsigned int mode = cipher_kt_mode (kt->cipher);
      return mode == OPENVPN_MODE_CFB || mode == OPENVPN_MODE_OFB;
  }
  return false;
}
