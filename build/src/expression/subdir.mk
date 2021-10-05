################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/Expression.cpp \
../src/expression/LoopExpr.cpp 

OBJS += \
./src/expression/Expression.o \
./src/expression/LoopExpr.o 

CPP_DEPS += \
./src/expression/Expression.d \
./src/expression/LoopExpr.d 


# Each subdirectory must supply rules for building sources it contributes
src/expression/%.o: ../src/expression/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


