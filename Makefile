TXT_FILES = $(shell find -name '*.mu.txt' | sed 's/\.txt/\.html/g' | sed 's/^\./build/g')
DOCX_FILES = $(shell find -name '*.mu.html' | sed 's/\.mu\.html/\.docx/g')
BUILD_DIR = build

all: $(TXT_FILES) .git/hooks/pre-commit

doc: $(DOCX_FILES)

$(BUILD_DIR)/%.html: %.txt
	mkdir -p $(shell dirname $(basename $@))
	pandoc -o $@ $<


.git/hooks/pre-commit:
	# This should help to prevent leaking my wifi password
	cp hooks/pre-commit ./.git/hooks/pre-commit

%.docx: %.mu.html
	pandoc -o $@ $<
