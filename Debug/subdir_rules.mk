################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.4.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="D:/Program/HTTP_sample_MSp430fr5969_sim800/HTTP_MSP430fr5969_SIM800_noFOTA/MQTT_SIM800_FR5969_No_FOTA/driverlib/MSP430FR5xx_6xx" --include_path="D:/Program/HTTP_sample_MSp430fr5969_sim800/HTTP_MSP430fr5969_SIM800_noFOTA/MQTT_SIM800_FR5969_No_FOTA/MQTTPacket" --include_path="D:/Program/HTTP_sample_MSp430fr5969_sim800/HTTP_MSP430fr5969_SIM800_noFOTA/MQTT_SIM800_FR5969_No_FOTA/Source" --include_path="C:/ti/ccsv8/ccs_base/msp430/include" --include_path="D:/Program/HTTP_sample_MSp430fr5969_sim800/HTTP_MSP430fr5969_SIM800_noFOTA/MQTT_SIM800_FR5969_No_FOTA" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.4.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=SIERRA_SIM -g --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


