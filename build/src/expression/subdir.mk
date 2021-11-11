################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/BinaryOperatorExpr.cpp \
../src/expression/ClassExpr.cpp \
../src/expression/Expression.cpp \
../src/expression/FunctionExpr.cpp \
../src/expression/IdentifierExpr.cpp \
../src/expression/IndexExpr.cpp \
../src/expression/ListExpr.cpp \
../src/expression/LoopExpr.cpp \
../src/expression/MapExpr.cpp \
../src/expression/PrefixExpr.cpp 

OBJS += \
./src/expression/BinaryOperatorExpr.o \
./src/expression/ClassExpr.o \
./src/expression/Expression.o \
./src/expression/FunctionExpr.o \
./src/expression/IdentifierExpr.o \
./src/expression/IndexExpr.o \
./src/expression/ListExpr.o \
./src/expression/LoopExpr.o \
./src/expression/MapExpr.o \
./src/expression/PrefixExpr.o 

CPP_DEPS += \
./src/expression/BinaryOperatorExpr.d \
./src/expression/ClassExpr.d \
./src/expression/Expression.d \
./src/expression/FunctionExpr.d \
./src/expression/IdentifierExpr.d \
./src/expression/IndexExpr.d \
./src/expression/ListExpr.d \
./src/expression/LoopExpr.d \
./src/expression/MapExpr.d \
./src/expression/PrefixExpr.d 


# Each subdirectory must supply rules for building sources it contributes
src/expression/%.o: ../src/expression/%.cpp src/expression/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -O3 -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


