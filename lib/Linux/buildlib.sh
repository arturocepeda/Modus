
cp ./../../src/*.* ./
cp ./../../include/*.* ./

g++ -std=c++0x -w -c *.cpp
ar rvs modus.a *.o

rm -f *.o
rm -f *.cpp
rm -f *.h
rm -f *.*~

