
# 01_PolonaiseMIDI
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -D__MACOSX_CORE__ ./../source/01_PolonaiseMIDI/main.cpp ./../../soundgen/MIDI/mxsoundgenmidi.cpp ./../../soundgen/externals/RtMidi/RtMidi.cpp -o ./bin/01_PolonaiseMIDI ./../../lib/MacOSX/modus.a -framework CoreMidi -framework CoreAudio -framework CoreFoundation

# 02_PolonaiseAudio
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ ./../source/02_PolonaiseAudio/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/02_PolonaiseAudio ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib

# 03_PolonaiseThread
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -lpthread ./../source/03_PolonaiseThread/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/03_PolonaiseThread ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib

# 04_PlayPiano
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -D__MACOSX_CORE__ ./../source/04_PlayPiano/main.cpp ./../../soundgen/externals/RtMidi/RtMidi.cpp ./../source/audio/audio.openal.cpp ./../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o ./bin/04_PlayPiano ./../../lib/MacOSX/modus.a ./../../soundgen/externals/OggVorbis/lib.macosx/libogg.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbis.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbisfile.a -framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenAL

# 05_RealTimeSettings
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ ./../source/05_RealTimeSettings/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/05_RealTimeSettings ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib

# 06_Recording
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -D__MACOSX_CORE__ ./../source/06_Recording/main.cpp ./../../soundgen/externals/RtMidi/RtMidi.cpp ./../source/audio/audio.openal.cpp ./../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o ./bin/06_Recording ./../../lib/MacOSX/modus.a ./../../soundgen/externals/OggVorbis/lib.macosx/libogg.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbis.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbisfile.a -framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenAL

# 07_AccelRit
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ ./../source/07_AccelRit/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/07_AccelRit ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib

# 08_Bending
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -lpthread ./../source/08_Bending/main.cpp ./../source/audio/audio.openal.cpp ./../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o ./bin/08_Bending ./../../lib/MacOSX/modus.a ./../../soundgen/externals/OggVorbis/lib.macosx/libogg.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbis.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbisfile.a -framework OpenAL

# 09_Intensity
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ ./../source/09_Intensity/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/09_Intensity ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib

# 10_ModeChange
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -lpthread ./../source/10_ModeChange/main.cpp ./../source/audio/audio.openal.cpp ./../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o ./bin/10_ModeChange ./../../lib/MacOSX/modus.a ./../../soundgen/externals/OggVorbis/lib.macosx/libogg.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbis.a ./../../soundgen/externals/OggVorbis/lib.macosx/libvorbisfile.a -framework OpenAL

# 11_NotemapsScripts
clang++ -std=c++11 -stdlib=libc++ -I./../../include/ -I./../../soundgen/ -lpthread ./../source/11_NotemapsScripts/main.cpp ./../source/audio/audio.fmod.cpp ./../../soundgen/FMOD/mxsoundgenfmod.cpp -o ./bin/11_NotemapsScripts ./../../lib/MacOSX/modus.a ./../../soundgen/externals/FMOD/lib.macosx/libfmodex.dylib
