sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)

TARGET					:= $(call SRC_2_BIN, $(d)/emulator)
TARGETS 				+= $(TARGET)
OBJS_$(d)				:= $(call SRC_2_OBJ, $(d)/parser.o $(d)/lexer.o \
		$(d)/interpreter.o $(d)/options.o)

OBJECTS 				+= $(OBJS_$(d))

$(OBJS_$(d))		:  CC_FLAGS_TARGET	:= -I$(d) -I$(call SRC_2_OBJ, $(d))

$(TARGET)				:  LD_FLAGS_TARGET	:= -ly -lfl
$(TARGET)				:  LD_OBJECTS	:= $(OBJS_$(d))
$(TARGET)				:  $(OBJS_$(d))

# Lex and yacc files dependecies
$(call SRC_2_OBJ, $(d)/parser.o): $(call SRC_2_OBJ, $(d)/lexer.h)
$(call SRC_2_OBJ, $(d)/lexer.o): $(call SRC_2_OBJ, $(d)/parser.h)
$(call SRC_2_OBJ, $(d)/interpreter.o): $(call SRC_2_OBJ, $(d)/parser.h)

d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
