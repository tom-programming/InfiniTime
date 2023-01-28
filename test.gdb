target remote localhost:3333
add-symbol-file ./build/output/src/pinetime-mcuboot-app-1.24.0.out
b HardFault_Handler