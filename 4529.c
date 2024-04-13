  std::unique_ptr<PacketNumberCipher> getInitialHeaderCipher(
      QuicVersion version = QuicVersion::MVFST) {
    FizzCryptoFactory cryptoFactory;
    return cryptoFactory.makeClientInitialHeaderCipher(
        *initialDestinationConnectionId, version);
  }