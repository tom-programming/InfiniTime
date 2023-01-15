ip=$1
if [ -z $ip ]; then
	ip=10.100.102.6
fi
ssh -L 3333:localhost:3333 pi@$ip
