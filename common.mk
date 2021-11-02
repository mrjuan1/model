O := model
SRCS := $(O).c
OBJS := $(SRCS:%.c=%.o)

MODELS := quad.bin camera.bin

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

all: $(O) indexed-quad.bin indexed-camera.bin

clean:
	@rm -Rfv $(O) *.o *.bin

distclean: clean
	@rm -Rfv $(shell cat .gitignore)

$(O): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS_DEBUG) $(CFLAGS)

run: $(O) $(MODELS)
	./$^

debug: $(O) $(MODELS)
	$(GDB) -ex run --batch --args ./$^

release: $(SRCS)
	$(CC) $^ -o $(O) $(CFLAGS_RELEASE) $(CFLAGS) $(LDFLAGS)
	$(STRIP) -s $(O)

dist: release
	@mkdir -pv dist
	@cp -v $(O) dist

%.blend:
	@if [ ! -e $@ ]; then cp -v ../$@ .; fi

%.bin: %.blend export.py
	blender $< -b -P export.py

indexed-%.bin: $(O) %.bin
	./$^

help:
	@echo "all - Build executable and export/convert $(MODEL)"
	@echo "clean - Remove executable, object(s) and all exported/converted model(s)"
	@echo "distclean - Remove everything in .gitignore"
	@echo "$(O) - Build $(O)"
	@echo "<object>.o - Build <object>.o"
	@echo "run - Run executable"
	@echo "debug - Run executable with gdb"
	@echo "release - Build optimised executable"
	@echo "dist - Package optimised executable and all local dependencies in a dist directory"
	@echo "<model>.blend - Copy Blender model from parent directory to this directory"
	@echo "<model>.bin - Export vertex data from <model>.blend"
	@echo "indexed-<model>.bin - Convert exported vertex data to indexed vertex data"
