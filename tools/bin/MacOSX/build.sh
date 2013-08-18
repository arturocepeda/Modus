
# midi2mscore
clang++ -std=c++11 -stdlib=libc++ -I./../../../include/ ./../../src/midi2mscore/midi2mscore.cpp ./../../src/midi2mscore/midifile.cpp -o ./midi2mscore/midi2mscore

# msamplepack
clang++ -std=c++11 -stdlib=libc++ -I./../../../include/ ./../../src/msamplepack/msamplepack.cpp -o ./msamplepack/msamplepack

# mscript2bin
clang++ -std=c++11 -stdlib=libc++ -I./../../../include/ ./../../src/mscript2bin/mscript2bin.cpp -o ./mscript2bin/mscript2bin ./../../../lib/MacOSX/modus.a
