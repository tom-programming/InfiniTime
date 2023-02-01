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


#### Mysterious bug: FS faults (interaction with BLE? FreeRTOS?)

Thanks to Sugru we were able to crash the Pinetime a hundred times more than usual! (Yay!)
We delved into the depth of Tom's BLE client, writing raw buffers with commands such as read, delete file.
We "extended" the command set to support starting and stopping the heart rate task from the file read command.
We saw that combinations such as start-stop-ls-start are crashing. But not always...
We found that the lfs mlist and its next element are in constant memory locations:
```
(gdb) p &(lfs->mlist)
$22 = (struct lfs_mlist **) 0x2000054c <fs+124>
(gdb) p &(lfs->mlist->next)
$23 = (struct lfs_mlist **) 0x20006b10 <ucHeap+6932>
```
so we tried to watch those addresses. We discovered that the code of FreeRTOS that is responsible for context switching tends to write to `0x20006b10` sometimes 0, sometimes 1. This could explain some of the crashes.
But why would we see such writes? This is something we do not yet understand.

We disabled the sketchy Arduino-like linked library to connect to the heart sensor and read/writes to `settings.dat` itself in the hope to isolate the problem. Nothing changed, except maybe we get more crashes now.

We made LFS nuke itself everytime FS is initialized (format).

Who will be our next Tarnegol Kaparot?

## GDB Tips & Tricks

`dump binary memory result.bin 0x20000000 0x20010000`
