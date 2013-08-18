
# midi2mscore
g++ -std=c++11 -I./../../../include/ ./../../src/midi2mscore/midi2mscore.cpp ./../../src/midi2mscore/midifile.cpp -o ./midi2mscore/midi2mscore

# msamplepack
g++ -std=c++11 -I./../../../include/ ./../../src/msamplepack/msamplepack.cpp -o ./msamplepack/msamplepack

# mscript2bin
g++ -std=c++11 -I./../../../include/ ./../../src/mscript2bin/mscript2bin.cpp -o ./mscript2bin/mscript2bin ./../../../lib/Linux/modus.a
