
cp ./../../src/*.* ./
cp ./../../include/*.* ./

g++ -m32 -w -c *.cpp
ar rvs libModus_sim.a *.o

rm *.o
rm *.cpp
rm *.h

