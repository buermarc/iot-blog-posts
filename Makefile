TXT_FILES = $(shell find -name '*.mu.txt' | sed 's/\.txt/\.html/g' | sed 's/^\./build/g')
BUILD_DIR = build

all: $(TXT_FILES) 

$(BUILD_DIR)/%.html: %.txt
	mkdir -p $(shell dirname $(basename $@))
	pandoc -o $@ $<
