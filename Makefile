CC						:= gcc
LD						:= gcc
LX						:= lex
YC						:= yacc
BUILD_DIR			:= build
BINARY_DIR		:= binary

CC_FLAGS_ALL		:= -Wall -Werror -Werror -Isources/include

LD_FLAGS_ALL		:=

define SRC_2_OBJ
  $(foreach src,$(1),$(patsubst sources/%,$(BUILD_DIR)/%,$(src)))
endef

define SRC_2_BIN
  $(foreach src,$(1),$(patsubst sources/%,$(BINARY_DIR)/%,$(src)))
endef

all: targets

# Overriden in rules.mk
TARGETS :=
OBJECTS :=

dir	:= sources
include	$(dir)/rules.mk

$(BINARY_DIR)/%:
	@echo "[LD] $@"
	@mkdir -p $(dir $@)
	@$(LD) -o $@ $(LD_OBJECTS) $(LD_FLAGS_TARGET)

$(BUILD_DIR)/%.o: sources/%.c
	@echo "[CC] $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CC_FLAGS_ALL) $(CC_FLAGS_TARGET) -o $@ -c $<

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	@echo "[CC] $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CC_FLAGS_ALL) $(CC_FLAGS_TARGET) -o $@ -c $<

$(BUILD_DIR)/%.h: $(BUILD_DIR)/%.c
	@#

$(BUILD_DIR)/%.c: sources/%.y
	@echo "[YC] $< -> $@"
	@mkdir -p $(dir $@)
	@$(YC) -d -o $@ $<

$(BUILD_DIR)/%.c: sources/%.l
	@echo "[LX] $< -> $@"
	@mkdir -p $(dir $@)
	@$(LX) --header-file=$(subst .c,.h,$@) -o $@ $<

targets: $(TARGETS)

clean:
	@rm -fr $(BUILD_DIR) $(BINARY_DIR)

info:
	@echo Targets [$(TARGETS)]
	@echo Objects [$(OBJECTS)]

# Remove default rulez
.SUFFIXES:
