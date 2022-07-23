################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ExceptionHandler.cpp \
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

CPP_DEPS += \
./src/ExceptionHandler.d \
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

OBJS += \
./src/ExceptionHandler.o \
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


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -DBITS=$(BITS) -I../include -I../src -O3 -Wall -c -fmessage-length=0 -m$(BITS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/ExceptionHandler.d ./src/ExceptionHandler.o ./src/ExprEvaluator.d ./src/ExprEvaluator.o ./src/InteractiveRunner.d ./src/InteractiveRunner.o ./src/Interpreter.d ./src/Interpreter.o ./src/Lexer.d ./src/Lexer.o ./src/Methan0l.d ./src/Methan0l.o ./src/Methan0lParser.d ./src/Methan0lParser.o ./src/Parser.d ./src/Parser.o ./src/Runner.d ./src/Runner.o ./src/Token.d ./src/Token.o ./src/type.d ./src/type.o

.PHONY: clean-src

