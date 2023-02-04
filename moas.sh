# MOAS

# clean
#/opt/build.sh clean

# build
#/opt/build.sh


# check if tunnel exists
if nc -z localhost 3333 
then
    echo "tunnel open"
else
    echo "no tunnel found"
    exit
fi

# transfer
./moas_transfer.sh build/output/pinetime-mcuboot-app-image-1.24.0.bin TOM

# flash
ssh pi /home/pi/Desktop/moas_pi.sh
