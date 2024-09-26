# disable gdb gpio
#pidofopenocd=`ps -ef | grep 'openocd' | awk '{print $2}'`
pidofopenocd=`pidof openocd`
if [ -z $pidofopenocd ]; then
	echo "no openocd to quit"
else
	kill $pidofopenocd
fi

# flash
cd /home/pi/Desktop/pinetime-updater
./run-tom.sh /home/pi/Desktop/pinetime-mcuboot-app-image-1.24.0.TOM.bin

# enable gdb gpio
sleep 5
cd /home/pi/Desktop/openocd-spi/scripts/
./start_gdb_gpio.sh 
