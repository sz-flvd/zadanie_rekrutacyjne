# Project structure section
TARGET_EXEC := main.out

APP_DIRS := ./app
INC_DIRS := ./inc
SRC_DIRS := ./src
TEST_DIRS := ./test

SRC := $(wildcard $(SRC_DIRS)/*.c)
SRC_ALL := $(SRC) $(wildcard $(APP_DIRS)/*.c)
OBJ := $(SRC_ALL:%.c=%.o)
OBJ_ALL := $(OBJ)
DEP := $(OBJ_ALL:%.o=%.d)

LIB_PATHS :=
LIBS := pthread rt

# Compiler + flag section
ifneq ("$(origin CC)","command line")
	CC = gcc
endif

C_FLAGS := -Wall -Wextra -mavx -pthread
DEP_FLAGS := -MMD -MP

HDR_INC := $(addprefix -I,$(INC_DIRS))
LIB_INC := $(addprefix -l,$(LIBS))
LIB_PATH := $(addprefix -L,$(LIB_PATHS))

# -Wno-vla flag added to prevent compiler from warning about intended usage of VLAs in analyser thread 
# -Wno-disabled-macro-expasion flag added because of warning that could not be fixed, caused by assigning to sa_handler field of sigaction struct in warehouse
ifeq ($(CC),clang)
	OTHER_FLAGS += -Weverything -Wno-vla -Wno-disabled-macro-expansion
else ifneq (, $(filter $(CC), cc gcc))
	OTHER_FLAGS += -rdynamic
endif

ifeq ("$(origin O)","command line")
	OPT := -O$(O)
else
	OPT :=
endif

ifeq ("$(origin G)","command line")
	GDB := -ggdb$(G)
else
	GDB :=
endif

C_FLAGS += $(OTHER_FLAGS) $(OPT) $(GDB) $(DEP_FLAGS)

# Other options section
RM := rm -rf

ifeq ("$(origin V)","command line")
	PRINT := $(nullstring)
else
	PRINT ?= @
endif


all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJ_ALL)
	$(PRINT)$(CC) $(C_FLAGS) $(HDR_INC) $(OBJ_ALL) -o $@ $(LIB_PATH) $(LIB_INC)

%.o: %.c
	$(PRINT)$(CC) $(C_FLAGS) $(HDR_INC) -c $< -o $@

clean:
	$(PRINT)$(RM) $(TARGET_EXEC)
	$(PRINT)$(RM) $(OBJ)
	$(PRINT)$(RM) $(DEP)

-include $(DEPS)