
cp ./../../src/*.* ./
cp ./../../include/*.* ./

# Query for the version of GCC that is being used
VERSION=$(gcc -dumpversion)

# Split the version string using the periods and create an array in order to
# extract the major and minor version numbers
VERSION=(${VERSION//./ })
MAJOR=${VERSION[0]}
MINOR=${VERSION[1]}

# Set the C++ standard based upon the version of GCC available
if [ ${MAJOR} -eq 4 ] && [ ${MINOR} -gt 6 ]
then
  echo ${MAJOR} ${MINOR}
  CXXSTD="c++11"
else
  CXXSTD="c++0x"
fi

# Compile and link
g++ -std=${CXXSTD} -w -c *.cpp
ar rvs modus.a *.o

# Clean up
rm -f *.o
rm -f *.cpp
rm -f *.h
rm -f *.*~

