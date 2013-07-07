
cp ./../../src/*.* ./
cp ./../../include/*.* ./

clang++ -std=c++11 -stdlib=libc++ -w -c *.cpp
ar rvs modus.a *.o

rm *.o
rm *.cpp
rm *.h

