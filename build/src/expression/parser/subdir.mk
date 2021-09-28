################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/parser/InfixParser.cpp 

OBJS += \
./src/expression/parser/InfixParser.o 

CPP_DEPS += \
./src/expression/parser/InfixParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/expression/parser/%.o: ../src/expression/parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


