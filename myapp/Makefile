SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=./param.mk
	include $(PARAM_FILE)
endif

SDIR = $(CURDIR)
# Update BUILD_DIR to use absolute path from current working directory
BUILD_DIR = $(shell pwd)/build

SRCS = $(wildcard $(SDIR)/*.c) $(wildcard $(SDIR)/src/*.c)
MYAPP_INC=$(SDIR)/include
#
# Fix include paths - remove empty -I flags
INCS = -I$(MYAPP_INC) -I$(MW_INC) -I$(ISP_INC) -I$(KERNEL_INC) -I../common -I$(SYS_INC) -I$(SENSOR_LIST_INC)
#
# Modify object paths to use build directory
OBJS = $(SRCS:$(SDIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)
#
TARGET = myapp
#
TARGET_WITH_ASAN =
#
PKG_CONFIG_PATH = $(MW_PATH)/pkgconfig
REQUIRES = cvi_common cvi_vdec cvi_sample
LIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --define-variable=mw_dir=$(MW_PATH) $(REQUIRES))
LIBS += -lvdec -lmisc -lsys -lvi -lvpss -lgdc -lvenc -lcvi_bin -lcvi_bin_isp -lisp -lisp_algo -lsns_full -lae -laf -lawb -latomic
#
ifeq ($(MULTI_PROCESS_SUPPORT), 1)
DEFS += -DRPC_MULTI_PROCESS
LIBS += -lnanomsg
endif
#
EXTRA_CFLAGS = $(DEFS) $(INCS)
EXTRA_LDFLAGS = $(LIBS) -lini -lm -lpthread

EXTRA_LDFLAGS_WITH_ASAN = -Wl,-Bdynamic $(LIBS) -lini -lm -lpthread
EXTRA_LDFLAGS_WITH_ASAN +=  -fno-omit-frame-pointer -fsanitize-recover=address -lasan

# Add debug print to see what's happening
$(info SRCS = $(SRCS))
$(info OBJS = $(OBJS))
$(info BUILD_DIR = $(BUILD_DIR))

# Add more debug prints
$(info Current directory = $(CURDIR))
$(info Source files found = $(SRCS))

# Add more detailed debug info
$(info CC = $(CC))
$(info CFLAGS = $(CFLAGS))
$(info EXTRA_CFLAGS = $(EXTRA_CFLAGS))

.PHONY : clean all
all: $(TARGET) $(TARGET_WITH_ASAN)

# Create build directory targets
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src

# Simplified and fixed build rules
$(BUILD_DIR)/%.o: $(SDIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(BUILD_DIR)/src/%.o: $(SDIR)/src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(MW_LIB)/libvdec.a $(MW_LIB)/libsys.a
	@$(CXX) -o $@ -Wl,--start-group $(OBJS) -Wl,--end-group $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

$(TARGET_WITH_ASAN): $(VDEC_OBJS) $(OBJS)
	@$(CXX) -o $@ -Wl,--start-group $(OBJS) $(VDEC_OBJS) -Wl,--end-group $(ELFFLAGS) $(EXTRA_LDFLAGS_WITH_ASAN)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

clean:
	@rm -rf $(BUILD_DIR) $(TARGET) $(TARGET_WITH_ASAN)

-include $(VDEC_DEPS) $(DEPS)
