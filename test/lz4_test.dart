library dart_lz4.tests;

import "dart:io";
import "dart:typed_data";
import "package:unittest/unittest.dart";
import '../lib/dart_lz4.dart' as lz4;

void main(List<String> args) {

  lz4.Lz4Codec fastCodec = new lz4.Lz4Codec(mode : lz4.CompressionMode.FAST);
  lz4.Lz4Codec hcCodec = new lz4.Lz4Codec(mode : lz4.CompressionMode.HIGH);

  group("LZ4:", () {

    test("compress without an instance of Uint8List as input should throw", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        dynamic _data = new Uint16List.fromList(data);
        expect(() => fastCodec.encode(_data)
        , throwsA((e) => e == "Data to be compressed should be an instance of Uint8List")
        );
      });
    });

    test("compress (fast) -> decompress", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        Uint8List compressed = fastCodec.encode(data);
        expect(compressed, new isInstanceOf<Uint8List>());
        Uint8List decompressed = fastCodec.decode(compressed);
        expect(decompressed, new isInstanceOf<Uint8List>());
      });
    });

    test("compress (hc) -> decompress", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        Uint8List compressed = hcCodec.encode(data);
        expect(compressed, new isInstanceOf<Uint8List>());
        Uint8List decompressed = hcCodec.decode(compressed);
        expect(decompressed, new isInstanceOf<Uint8List>());
      });
    });

  });
}