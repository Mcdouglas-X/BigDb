
CXX = g++
CXXFLAGS = -pthread -Wno-write-strings
LDFLAGS = -lssl -lcrypto
SOURCES = main.cpp Bigdb.cpp threads.cpp utils.cpp hashtable/HT.cpp secp256k1/SECP256k1.cpp secp256k1/IntGroup.cpp secp256k1/Random.cpp secp256k1/Int.cpp secp256k1/IntMod.cpp secp256k1/Point.cpp secp256k1/Timer.cpp bloom/bloom.cpp
TARGET = Bigdb

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) -o $(TARGET) $(SOURCES) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o
