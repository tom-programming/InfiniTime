echo "USAGE: ip file suffix"
ip=$1
if [ -z $ip ]; then
	ip=10.100.102.202
fi
fileobj=$2
suff=$3

if [ -z $fileobj ]; then
	echo "missing file"
fi

if [ -z $suff ]; then
	echo "missing suffix"
fi

base="${fileobj##*/}"
base="${base%.*}"
ext="${fileobj##*.}"
scp $fileobj pi@$ip:Desktop/$base.$suff.$ext
