target remote localhost:3333
add-symbol-file ./build/output/src/pinetime-mcuboot-app-1.24.0.out
b HardFault_Handler


set pagination off
set logging file output_main.log
set logging redirect on
set logging on 
b HeartRateTask.cpp:206
set $ind=0

while($ind==0)
	p timer_index
	p new_raw_data
	p als_raw_data
	c
end
