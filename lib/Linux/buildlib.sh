
cp ./../../src/*.* ./
cp ./../../include/*.* ./

g++ -std=c++11 -w -c *.cpp
ar rvs modus.a *.o

rm *.o
rm *.cpp
rm *.h
rm *.*~

