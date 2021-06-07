TXT_FILES = $(shell find -name '*.mu.txt' | sed 's/\.txt/\.html/g' | sed 's/^\./build/g')

all: $(TXT_FILES) 

build/%.html: %.txt
	mkdir -p $(shell dirname $(basename $@))
	pandoc -o $@ $<
