#!/bin/sh

MUELLER_DIR=${HOME}/projects/sdcv-project/dictionaries/mueller7gpl
SDICT_DIR=${HOME}/projects/sdcv-project/dictionaries/sdict
DICTD_DIR=${HOME}/projects/sdcv-project/dictionaries/dictd
APRESYAN_DIR=${HOME}/projects/sdcv-project/dictionaries/apresyan
DSL_DIR=${HOME}/projects/sdcv-project/dictionaries/dsl

RES_DIR=/tmp/xdxf

LOG=/tmp/makedict.log

alias VALIDATE="xmlvalid --dtd=./dtd/xdxf_lousy.dtd"

if [ -z "$1" -o -z "$2" ]; then
        echo "Usage: $0 input_format(you can use all) ouput_format" >&2
        exit 1
fi

TO=$2

if [ "$TO" == "stardict" ]; then
		RES_DIR=/tmp/stardict
		alias VALIDATE="./validate_stardict"
fi

rm -f $LOG
rm -fr $RES_DIR
mkdir $RES_DIR

let NCONVERTED=0
let SUCCESS_CONVERTED=0
let NVALID=0

convert() {
		if [ -z "$1" -o -z "$2" ]; then
				echo "Usage convert dir template" >&2
				exit 1
		fi
IFS="
"
		for i in `find $1 -name *${2} -print`; do
					if [ $2 == ".dsl" -a -z "`echo $i | grep -v 'abrv.dsl$' | grep -v 'abbrev.dsl$'`" ]; then
						continue;
					fi
					echo "convert $i" >> $LOG
					BASENAME=`basename $i`
					OPTS=`grep "^${BASENAME}" makedict_options | awk -F"=>" '{print $2}'`
					unset IFS
					let NCONVERTED=NCONVERTED+1
					if ../src/makedict  $OPTS -o $TO --work-dir "$RES_DIR" "$i" >> $LOG 2>&1; then
							let SUCCESS_CONVERTED=SUCCESS_CONVERTED+1
					fi
					
IFS="
"
          if [ "$TO" == "stardict" ]; then
							RES=`basename $i $2`
							RES="stardict-"$RES"-2.4.2"
							RES=`ls "${RES_DIR}"/$RES/*.ifo`
					else
							RES=$RES_DIR/`basename $i $2`/dict.xdxf
					fi
					#echo "result: $RES"
					if VALIDATE  $RES >> $LOG 2>&1; then
							let NVALID=NVALID+1
					fi
		done
}

echo "if something wrong look at $LOG"

FORMAT="$1"

if [ "$FORMAT" == "all" ]; then
	convert $DSL_DIR ".dsl"
	convert $APRESYAN_DIR ".koi"		
	convert $MUELLER_DIR ".koi"
	convert $DICTD_DIR ".index"
	convert $SDICT_DIR ".dct"
elif [ "$FORMAT" == "mueller7" ]; then
	convert $MUELLER_DIR ".koi"
elif [ "$FORMAT" == "apresyan" ]; then
	convert $APRESYAN_DIR ".koi"		
elif [ "$FORMAT" == "dsl" ]; then
	convert $DSL_DIR ".dsl"
elif [ "$FORMAT" == "sdict" ]; then
	convert $SDICT_DIR ".dct"
elif [ "$FORMAT" == "dictd" ]; then
	convert $DICTD_DIR ".index"
else
	echo "invalid format" >&2
	exit 1
fi

echo $SUCCESS_CONVERTED"/"$NCONVERTED " good/converted"
echo $NVALID"/"$NCONVERTED " valid/converted"
