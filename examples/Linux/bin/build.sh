
# 01_PolonaiseMIDI
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ -D__LINUX_ALSA__ ./../../source/01_PolonaiseMIDI/main.cpp ./../../../soundgen/MIDI/mxsoundgenmidi.cpp ./../../../soundgen/externals/RtMidi/RtMidi.cpp -o 01_PolonaiseMIDI ./../../../lib/Linux/modus.a -lasound -lpthread

# 02_PolonaiseAudio
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/02_PolonaiseAudio/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 02_PolonaiseAudio ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

# 03_PolonaiseThread
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/03_PolonaiseThread/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 03_PolonaiseThread ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

# 04_PlayPiano
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ -D__LINUX_ALSA__ ./../../source/04_PlayPiano/main.cpp ./../../../soundgen/externals/RtMidi/RtMidi.cpp ./../../source/audio/audio.openal.cpp ./../../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o 04_PlayPiano ./../../../lib/Linux/modus.a -lopenal -logg -lvorbis -lvorbisfile -lasound

# 05_RealTimeSettings
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/05_RealTimeSettings/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 05_RealTimeSettings ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

# 06_Recording
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ -D__LINUX_ALSA__ ./../../source/06_Recording/main.cpp ./../../../soundgen/externals/RtMidi/RtMidi.cpp ./../../source/audio/audio.openal.cpp ./../../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o 06_Recording ./../../../lib/Linux/modus.a -lopenal -logg -lvorbis -lvorbisfile -lasound

# 07_AccelRit
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/07_AccelRit/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 07_AccelRit ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

# 08_Bending
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/08_Bending/main.cpp ./../../source/audio/audio.openal.cpp ./../../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o 08_Bending ./../../../lib/Linux/modus.a -lopenal -logg -lvorbis -lvorbisfile -lasound

# 09_Intensity
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/09_Intensity/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 09_Intensity ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

# 10_ModeChange
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/10_ModeChange/main.cpp ./../../source/audio/audio.openal.cpp ./../../../soundgen/OpenAL/mxsoundgenopenal.cpp ./../../../soundgen/OpenAL/mxopenalsourcemanager.cpp -o 10_ModeChange ./../../../lib/Linux/modus.a -lopenal -logg -lvorbis -lvorbisfile -lasound

# 11_NotemapsScripts
g++ -std=c++11 -I./../../../include/ -I./../../../soundgen/ ./../../source/11_NotemapsScripts/main.cpp ./../../source/audio/audio.fmod.cpp ./../../../soundgen/FMOD/mxsoundgenfmod.cpp -o 11_NotemapsScripts ./../../../lib/Linux/modus.a ./../../../soundgen/externals/FMOD/lib.linux/libfmodex.so -pthread -ldl

