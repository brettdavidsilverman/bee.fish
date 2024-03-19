OUTPUT = ../../build/b-string
CFLAGS = -std=gnu++17 -Wall -fmax-errors=1 -Ofast
DEPS=character.h b-string.h bit-stream.h data.h base64.h misc.h string.h stream.h test.h main.cpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	clean
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
