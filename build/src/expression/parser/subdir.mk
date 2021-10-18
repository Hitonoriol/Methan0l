################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/parser/ClassParser.cpp \
../src/expression/parser/InfixParser.cpp \
../src/expression/parser/MapParser.cpp 

OBJS += \
./src/expression/parser/ClassParser.o \
./src/expression/parser/InfixParser.o \
./src/expression/parser/MapParser.o 

CPP_DEPS += \
./src/expression/parser/ClassParser.d \
./src/expression/parser/InfixParser.d \
./src/expression/parser/MapParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/expression/parser/%.o: ../src/expression/parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -O3 -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


