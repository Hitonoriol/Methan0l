################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/parser/BinaryOperatorParser.cpp \
../src/expression/parser/ClassParser.cpp \
../src/expression/parser/InfixParser.cpp \
../src/expression/parser/InfixWordOperatorParser.cpp \
../src/expression/parser/ListParser.cpp \
../src/expression/parser/MapParser.cpp \
../src/expression/parser/WordOperatorParser.cpp 

OBJS += \
./src/expression/parser/BinaryOperatorParser.o \
./src/expression/parser/ClassParser.o \
./src/expression/parser/InfixParser.o \
./src/expression/parser/InfixWordOperatorParser.o \
./src/expression/parser/ListParser.o \
./src/expression/parser/MapParser.o \
./src/expression/parser/WordOperatorParser.o 

CPP_DEPS += \
./src/expression/parser/BinaryOperatorParser.d \
./src/expression/parser/ClassParser.d \
./src/expression/parser/InfixParser.d \
./src/expression/parser/InfixWordOperatorParser.d \
./src/expression/parser/ListParser.d \
./src/expression/parser/MapParser.d \
./src/expression/parser/WordOperatorParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/expression/parser/%.o: ../src/expression/parser/%.cpp src/expression/parser/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I../include -I../src -O3 -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


