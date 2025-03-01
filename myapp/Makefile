SHELL = /bin/bash

PARAM_FILE:=./param.mk
include $(PARAM_FILE)

SDIR = $(CURDIR)
BUILD_DIR = $(SDIR)/build
LIB_DIR = $(BUILD_DIR)/lib

SRCS = $(wildcard $(SDIR)/src/*.c)
APP_SRC = $(SDIR)/myapp.c
APP_INC = $(SDIR)/include

# Convert source paths to build directory paths
OBJS = $(SRCS:$(SDIR)/%.c=$(BUILD_DIR)/%.o)
APP_OBJS = $(APP_SRC:$(SDIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)
APP_DEPS = $(APP_OBJS:.o=.d)

TARGET = $(BUILD_DIR)/myapp

INCS = -I$(MW_INC) -I$(ISP_INC) \
       -I$(SYS_INC) -I$(SENSOR_LIST_INC) \
       -I$(APP_INC)

PKG_CONFIG_PATH = $(MW_PATH)/pkgconfig
REQUIRES = cvi_common cvi_vdec cvi_sample
LIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --define-variable=mw_dir=$(MW_PATH) $(REQUIRES))
LIBS += -lvdec -lmisc -lsys -lvi -lvpss -lvo  -lgdc -lvenc -lcvi_bin -lcvi_bin_isp -lisp -lisp_algo -lsns_full -lae -laf -lawb -latomic

# Define library paths and files to copy
MW_LIB_DIR = $(MW_PATH)/lib
SYS_LIB_DIR = $(SYS_PATH)/lib
BIN_LIB_DIR = $(BIN_PATH)/lib

FFMPEG_DIR ?=  ../3rd_party/ffmpeg_libs_n4.4.4.2
FFMPEG_INC ?= -I$(FFMPEG_DIR)/include
FFMPEG_LIB ?= -L$(FFMPEG_DIR)/lib -lavformat -lavcodec -lavutil -lswresample

# Copy libraries into a /lib dir
LIBS_TO_COPY = \
    $(wildcard $(MW_LIB_DIR)/lib*.so*) \
    $(wildcard $(SYS_LIB_DIR)/lib*.so*) \
    $(wildcard $(BIN_LIB_DIR)/lib*.so*)

EXTRA_CFLAGS = $(DEFS) $(INCS) $(FFMPEG_INC)
EXTRA_LDFLAGS = $(LIBS) -lini -lm -lpthread -Wl,-rpath,$(LIB_DIR) $(FFMPEG_LIB)

.PHONY : clean all copy_libs create_dirs

all: create_dirs $(TARGET) copy_libs

# Create necessary directories
create_dirs:
	@mkdir -p $(BUILD_DIR)/src
	@mkdir -p $(LIB_DIR)

# Rule for building object files
$(BUILD_DIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo "Compiling: $<"

$(TARGET): $(OBJS) $(APP_OBJS)
	@$(CXX) -o $@ -Wl,--start-group $^ -Wl,--end-group $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

copy_libs:
	@echo "Copying libraries to $(LIB_DIR)"
	@if [ -n "$(LIBS_TO_COPY)" ]; then \
		cp -af $(LIBS_TO_COPY) $(LIB_DIR)/ || \
		(echo "Error copying libraries" && exit 1); \
	else \
		echo "Warning: No libraries found to copy"; \
	fi
	@echo "Library copy completed"

clean:
	@rm -rf $(BUILD_DIR)

-include $(DEPS)
