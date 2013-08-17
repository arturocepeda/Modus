
for f in $1*.wav; do echo "Converting $f to IMA4…"; afconvert -f caff -d ima4 -c 1 "$f"; done
