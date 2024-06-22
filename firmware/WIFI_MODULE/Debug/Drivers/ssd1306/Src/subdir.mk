################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ssd1306/Src/ssd1306.c \
../Drivers/ssd1306/Src/ssd1306_fonts.c \
../Drivers/ssd1306/Src/ssd1306_tests.c 

OBJS += \
./Drivers/ssd1306/Src/ssd1306.o \
./Drivers/ssd1306/Src/ssd1306_fonts.o \
./Drivers/ssd1306/Src/ssd1306_tests.o 

C_DEPS += \
./Drivers/ssd1306/Src/ssd1306.d \
./Drivers/ssd1306/Src/ssd1306_fonts.d \
./Drivers/ssd1306/Src/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ssd1306/Src/%.o Drivers/ssd1306/Src/%.su Drivers/ssd1306/Src/%.cyclo: ../Drivers/ssd1306/Src/%.c Drivers/ssd1306/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ssd1306-2f-Src

clean-Drivers-2f-ssd1306-2f-Src:
	-$(RM) ./Drivers/ssd1306/Src/ssd1306.cyclo ./Drivers/ssd1306/Src/ssd1306.d ./Drivers/ssd1306/Src/ssd1306.o ./Drivers/ssd1306/Src/ssd1306.su ./Drivers/ssd1306/Src/ssd1306_fonts.cyclo ./Drivers/ssd1306/Src/ssd1306_fonts.d ./Drivers/ssd1306/Src/ssd1306_fonts.o ./Drivers/ssd1306/Src/ssd1306_fonts.su ./Drivers/ssd1306/Src/ssd1306_tests.cyclo ./Drivers/ssd1306/Src/ssd1306_tests.d ./Drivers/ssd1306/Src/ssd1306_tests.o ./Drivers/ssd1306/Src/ssd1306_tests.su

.PHONY: clean-Drivers-2f-ssd1306-2f-Src

