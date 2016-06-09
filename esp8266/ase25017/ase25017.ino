#include <WString.h>

#include <memory>
#include <vector>

class Foo {
public:
  Foo() {
    Serial.println("foo: ctor");
  }
  ~Foo() {
    Serial.println("foo: dtor");
  }
};

void tryfoo() {
  std::unique_ptr<Foo[]> fp(new Foo[4]);
  Serial.println("in tryfoo");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  tryfoo();
  std::vector<String> vi(3);
  vi[0] = "zero";
  vi[1] = "one";
  vi[2] = "two";
  for (auto i : vi) {
    Serial.println(i);
  }
  // This indeed does not work.
  // Serial.println(vi.at(8));
}

void loop() {
  delay(1);
}
