echo "USAGE: file suffix"
fileobj=$1
suff=$2

if [ -z $fileobj ]; then
	echo "missing file"
fi

if [ -z $suff ]; then
	echo "missing suffix"
fi

base="${fileobj##*/}"
base="${base%.*}"
ext="${fileobj##*.}"
scp $fileobj pi:Desktop/$base.$suff.$ext
