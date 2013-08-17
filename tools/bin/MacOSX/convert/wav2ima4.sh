
for f in $1*.wav; do echo "Converting $f to IMA4…"; afconvert -f caff -d ima4 "$f"; done
