################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/expression/AssignExpr.cpp \
../src/expression/BinaryOperatorExpr.cpp \
../src/expression/ClassExpr.cpp \
../src/expression/ConditionalExpr.cpp \
../src/expression/Expression.cpp \
../src/expression/FormatStrExpr.cpp \
../src/expression/FunctionExpr.cpp \
../src/expression/IdentifierExpr.cpp \
../src/expression/IndexExpr.cpp \
../src/expression/ListExpr.cpp \
../src/expression/LiteralExpr.cpp \
../src/expression/LoopExpr.cpp \
../src/expression/MapExpr.cpp \
../src/expression/PostfixExpr.cpp \
../src/expression/PrefixExpr.cpp \
../src/expression/RangeExpr.cpp \
../src/expression/TryCatchExpr.cpp \
../src/expression/UnitExpr.cpp 

CPP_DEPS += \
./src/expression/AssignExpr.d \
./src/expression/BinaryOperatorExpr.d \
./src/expression/ClassExpr.d \
./src/expression/ConditionalExpr.d \
./src/expression/Expression.d \
./src/expression/FormatStrExpr.d \
./src/expression/FunctionExpr.d \
./src/expression/IdentifierExpr.d \
./src/expression/IndexExpr.d \
./src/expression/ListExpr.d \
./src/expression/LiteralExpr.d \
./src/expression/LoopExpr.d \
./src/expression/MapExpr.d \
./src/expression/PostfixExpr.d \
./src/expression/PrefixExpr.d \
./src/expression/RangeExpr.d \
./src/expression/TryCatchExpr.d \
./src/expression/UnitExpr.d 

OBJS += \
./src/expression/AssignExpr.o \
./src/expression/BinaryOperatorExpr.o \
./src/expression/ClassExpr.o \
./src/expression/ConditionalExpr.o \
./src/expression/Expression.o \
./src/expression/FormatStrExpr.o \
./src/expression/FunctionExpr.o \
./src/expression/IdentifierExpr.o \
./src/expression/IndexExpr.o \
./src/expression/ListExpr.o \
./src/expression/LiteralExpr.o \
./src/expression/LoopExpr.o \
./src/expression/MapExpr.o \
./src/expression/PostfixExpr.o \
./src/expression/PrefixExpr.o \
./src/expression/RangeExpr.o \
./src/expression/TryCatchExpr.o \
./src/expression/UnitExpr.o 


# Each subdirectory must supply rules for building sources it contributes
src/expression/%.o: ../src/expression/%.cpp src/expression/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -DBITS=$(BITS) -I../include -I../src -O3 -Wall -c -fmessage-length=0 -m$(BITS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-expression

clean-src-2f-expression:
	-$(RM) ./src/expression/AssignExpr.d ./src/expression/AssignExpr.o ./src/expression/BinaryOperatorExpr.d ./src/expression/BinaryOperatorExpr.o ./src/expression/ClassExpr.d ./src/expression/ClassExpr.o ./src/expression/ConditionalExpr.d ./src/expression/ConditionalExpr.o ./src/expression/Expression.d ./src/expression/Expression.o ./src/expression/FormatStrExpr.d ./src/expression/FormatStrExpr.o ./src/expression/FunctionExpr.d ./src/expression/FunctionExpr.o ./src/expression/IdentifierExpr.d ./src/expression/IdentifierExpr.o ./src/expression/IndexExpr.d ./src/expression/IndexExpr.o ./src/expression/ListExpr.d ./src/expression/ListExpr.o ./src/expression/LiteralExpr.d ./src/expression/LiteralExpr.o ./src/expression/LoopExpr.d ./src/expression/LoopExpr.o ./src/expression/MapExpr.d ./src/expression/MapExpr.o ./src/expression/PostfixExpr.d ./src/expression/PostfixExpr.o ./src/expression/PrefixExpr.d ./src/expression/PrefixExpr.o ./src/expression/RangeExpr.d ./src/expression/RangeExpr.o ./src/expression/TryCatchExpr.d ./src/expression/TryCatchExpr.o ./src/expression/UnitExpr.d ./src/expression/UnitExpr.o

.PHONY: clean-src-2f-expression

