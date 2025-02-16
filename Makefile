CXX = g++
CXXFLAGS = -std=c++20
LDFLAGS = -lcurl $(shell pkg-config --libs simdjson)
CPPFLAGS = $(shell pkg-config --cflags simdjson)

TARGET = fakerest_test_brightsign
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET)
