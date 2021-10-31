O := model
SRCS := $(O).c
OBJS := $(SRCS:%.c=%.o)

MODEL := camera

CC := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip
GDB := $(CROSS_COMPILE)gdb

CFLAGS_COMMON := -std=c2x -m64

CFLAGS_DEBUG := $(CFLAGS_COMMON)
CFLAGS_DEBUG += -pedantic -Wall -Wextra
CFLAGS_DEBUG += -g3 -O0

CFLAGS_RELEASE := $(CFLAGS_COMMON)
CFLAGS_RELEASE += -Ofast
CFLAGS_RELEASE += -ftree-vectorize -ffast-math -funroll-loops

all: $(O) $(MODEL).blend $(MODEL).bin indexed-$(MODEL).bin

clean:
	@rm -Rfv $(O) *.o *.bin

distclean: clean
	@rm -Rfv $(shell cat .gitignore)

$(O): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS_DEBUG) $(CFLAGS)

run: indexed-$(MODEL).bin

debug: $(O) $(MODEL).bin
	$(GDB) -ex run --batch --args ./$^

release: $(SRCS)
	$(CC) $^ -o $(O) $(CFLAGS_RELEASE) $(CFLAGS) $(LDFLAGS)
	$(STRIP) -s $(O)

dist: release
	@mkdir -pv dist
	@cp -v $(O) dist

$(MODEL).blend:
	@if [ ! -e $@ ]; then cp -v ../$@ .; fi

$(MODEL).bin: $(MODEL).blend export.py
	blender $< -b -P export.py

indexed-$(MODEL).bin: $(O) $(MODEL).bin
	./$^

help:
	@echo "all - Build executable and export/convert $(MODEL)"
	@echo "clean - Remove executable, object(s) and all exported/converted model(s) directory"
	@echo "distclean - Remove everything in .gitignore"
	@echo "$(O) - Build $(O)"
	@echo "<object>.o - Build <object>.o"
	@echo "run - Run executable"
	@echo "debug - Run executable with gdb"
	@echo "release - Build optimised executable"
	@echo "dist - Package optimised executable and all local dependencies in a dist directory"
	@echo "$(MODEL).blend - Copy Blender model to this directory"
	@echo "$(MODEL).bin - Export vertex data from $(MODEL).blend"
	@echo "indexed-$(MODEL).bin - Convert exported model vertex data to indexed vertex data"
