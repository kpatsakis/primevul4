  void setHandshakeKeys() {
    conn_.handshakeWriteCipher = createNoOpAead();
    conn_.handshakeWriteHeaderCipher = createNoOpHeaderCipher();
    handshakeReadCipher_ = createNoOpAead();
    handshakeReadHeaderCipher_ = createNoOpHeaderCipher();
  }