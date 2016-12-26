
CMAKE_BUILD_TYPE ?= Debug

CMAKE_FLAGS ?=
CMAKE_EXTRA_FLAGS ?=

BUILD_DIR ?= build
DESTDIR ?=

ifneq ($(wildcard $(BUILD_DIR)/build.ninja),) 
  BUILD_TYPE ?= Ninja
else
  BUILD_TYPE ?= Unix Makefiles
endif

ifeq (Ninja,$(BUILD_TYPE))
  BUILD_TOOL := ninja
  BUILD_TOOL_ARGS =
else
  BUILD_TOOL = $(MAKE)
  JOBS = $(shell (type nproc > /dev/null 2>&1 && nproc || 1 ) )
  $(info Running $(JOBS) build jobs)
  BUILD_TOOL_ARGS = -j$(JOBS) -s
endif

build=$(BUILD_TOOL) -C $(BUILD_DIR)

all: fovris

fovris: $(BUILD_DIR)/.cmake-run
	$(build) $(BUILD_TOOL_ARGS)
	
unittest functionaltest: | fovris
	$(build) $(BUILD_TOOL_ARGS) $@

test: unittest functionaltest

install: | fovris
	 DESTDIR="$(DESTDIR)" $(build) $@

uninstall: | fovris
	 DESTDIR="$(DESTDIR)" $(build) $@

clean:
	test -d $(BUILD_DIR) && $(build) clean || true

$(BUILD_DIR)/.cmake-run:
	mkdir -p $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	cmake -G '$(BUILD_TYPE)' -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) $(CMAKE_FLAGS) $(CMAKE_EXTRA_FLAGS) .. && \
	cd .. && \
	touch $@

.PHONY: test functionaltest unittest benchmark install uninstall
