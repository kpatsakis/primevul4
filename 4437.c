  void setEarlyKeys() {
    oneRttWriteCipher_ = createNoOpAead();
    oneRttWriteHeaderCipher_ = createNoOpHeaderCipher();
    zeroRttReadCipher_ = createNoOpAead();
    zeroRttReadHeaderCipher_ = createNoOpHeaderCipher();
  }