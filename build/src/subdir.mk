################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ExprEvaluator.cpp \
../src/InteractiveRunner.cpp \
../src/Interpreter.cpp \
../src/Lexer.cpp \
../src/Methan0l.cpp \
../src/Methan0lParser.cpp \
../src/Parser.cpp \
../src/Runner.cpp \
../src/Token.cpp \
../src/type.cpp 

OBJS += \
./src/ExprEvaluator.o \
./src/InteractiveRunner.o \
./src/Interpreter.o \
./src/Lexer.o \
./src/Methan0l.o \
./src/Methan0lParser.o \
./src/Parser.o \
./src/Runner.o \
./src/Token.o \
./src/type.o 

CPP_DEPS += \
./src/ExprEvaluator.d \
./src/InteractiveRunner.d \
./src/Interpreter.d \
./src/Lexer.d \
./src/Methan0l.d \
./src/Methan0lParser.d \
./src/Parser.d \
./src/Runner.d \
./src/Token.d \
./src/type.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I../include -I../src -O3 -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


