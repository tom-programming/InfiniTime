ip=$1
if [ -z $ip ]; then
	ip=10.100.102.202
fi
ssh -L 3333:localhost:3333 pi@$ip
