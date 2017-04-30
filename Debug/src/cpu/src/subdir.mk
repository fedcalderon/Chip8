################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cpu/src/Chip8.cpp \
../src/cpu/src/Main.cpp 

OBJS += \
./src/cpu/src/Chip8.o \
./src/cpu/src/Main.o 

CPP_DEPS += \
./src/cpu/src/Chip8.d \
./src/cpu/src/Main.d 


# Each subdirectory must supply rules for building sources it contributes
src/cpu/src/%.o: ../src/cpu/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


