################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/structure/DataTable.cpp \
../src/structure/Function.cpp \
../src/structure/Unit.cpp \
../src/structure/Value.cpp \
../src/structure/ValueRef.cpp 

CPP_DEPS += \
./src/structure/DataTable.d \
./src/structure/Function.d \
./src/structure/Unit.d \
./src/structure/Value.d \
./src/structure/ValueRef.d 

OBJS += \
./src/structure/DataTable.o \
./src/structure/Function.o \
./src/structure/Unit.o \
./src/structure/Value.o \
./src/structure/ValueRef.o 


# Each subdirectory must supply rules for building sources it contributes
src/structure/%.o: ../src/structure/%.cpp src/structure/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -DBITS=$(BITS) -I../include -I../src -O3 -Wall -c -fmessage-length=0 -m$(BITS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-structure

clean-src-2f-structure:
	-$(RM) ./src/structure/DataTable.d ./src/structure/DataTable.o ./src/structure/Function.d ./src/structure/Function.o ./src/structure/Unit.d ./src/structure/Unit.o ./src/structure/Value.d ./src/structure/Value.o ./src/structure/ValueRef.d ./src/structure/ValueRef.o

.PHONY: clean-src-2f-structure

