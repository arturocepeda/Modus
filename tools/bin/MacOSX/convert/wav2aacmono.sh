
for f in $1*.wav; do echo "Converting $f to AAC…"; afconvert -f caff -d aac -c 1 "$f"; done
