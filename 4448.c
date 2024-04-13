  std::unique_ptr<Aead> getInitialCipher(
      QuicVersion version = QuicVersion::MVFST) {
    FizzCryptoFactory cryptoFactory;
    return cryptoFactory.getClientInitialCipher(
        *initialDestinationConnectionId, version);
  }