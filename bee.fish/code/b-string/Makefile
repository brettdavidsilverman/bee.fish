BUILD  = ../../build
OUTPUT = $(BUILD)/b-string
CFLAGS = -std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS= $(BUILD)/power-encoding char.h b-string.h bit-stream.h data.h base64.h misc.h string.h stream.h test.h main.cpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test

$(BUILD)/power-encoding:
	cd ../power-encoding && make $(DEBUG)

debug:	CFLAGS += -g -D DEBUG
debug:	clean
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
