################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
utils/uartstdio.obj: C:/ti/tivaware_c_series_2_1_4_178/utils/uartstdio.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_5.2.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_5.2.9/include" --include_path="C:/Users/abdir/workspace_v12/freertos_demo" --include_path="C:/ti/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c123gxl" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" -g --c89 --c++03 --relaxed_ansi --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/$(basename $(<F)).d_raw" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

utils/ustdlib.obj: C:/ti/tivaware_c_series_2_1_4_178/utils/ustdlib.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_5.2.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_5.2.9/include" --include_path="C:/Users/abdir/workspace_v12/freertos_demo" --include_path="C:/ti/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c123gxl" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" -g --c89 --c++03 --relaxed_ansi --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/$(basename $(<F)).d_raw" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


