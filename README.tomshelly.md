# Watch Hackers

## Building

### In Debian:

```
./cleanbuild
./scp_to_pi.sh current_ip_of_pi path_to_build suffix
```

`path_to_build` is usually `build/output/pinetime-mcuboot-app-image-1.24.0.bin`

Make the tunnel first:
`ssh -L 3333:localhost:3333 pi@10.100.102.14` (replace with current_ip_of_pi)

## Flashing

### In Raspberry

Flashing:
go to `~/Desktop/pinetime-updater/`.

(no longer needed! ) ~edit `run-shelly.sh` to refer to the newly transferred file.~

run `run-shelly.sh path_to_file` with the bin file transferred.

## Debugging

### In Raspberry

From `~/Desktop/openocd-spi/scripts`
run `start_gdb_gpio.sh`.

### In Debian

0. Make sure the ssh tunnel is enabled: `start_ssh.sh ip`

1. Start gdb
2. `target remote localhost:3333`
3. Add symbols: `add-symbol-file ./build/output/src/pinetime-mcuboot-app-1.24.0.out`

Recommended breakpoint: `HardFault_Handler`

