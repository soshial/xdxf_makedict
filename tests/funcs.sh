generate_file() {
		if [ -z "$1" -o -z "$2" ]; then
				echo "Usage: generate_file nwords file" >&2
				exit 1
		fi
		RANGE=5000
		for ((i=1; i<=$1; i++)); do
				echo $RANDOM | uuencode -m /dev/stdout | grep -v "/dev/stdout" >> $2
		done
}