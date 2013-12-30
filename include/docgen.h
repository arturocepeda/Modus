
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.53
//  C++ Music Library
//
//  Copyright (c) 2012-2013 Arturo Cepeda Pérez
//
//  --------------------------------------------------------------------
//
//  This file is part of Modus. Permission is hereby granted, free 
//  of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the "Software"), to deal in the 
//  Software without restriction, including without limitation the 
//  rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit 
//  persons to whom the Software is furnished to do so, subject to 
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be 
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
//  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////

/** \mainpage Modus v0.53 - C++ Music Library

 <div align="center"><img src="modus.jpg"></div>

 \section contents Contents
 General:<br>
 @ref overview <br>
 @ref license <br>
 @ref components <br>
 @ref nomenclature <br>
 
 Concepts:<br>
 @ref musicalnotes <br>
 @ref time <br>
 @ref mainobjects <br>
 @ref soundgenerators <br>
 @ref samplesets <br>
 @ref samplepacks <br>

 Project Settings:<br>
 @ref projectsettings <br>
 
 Working with Modus:<br>
 @ref notemaps <br> 
 @ref scripts <br>
 @ref pitchvalues <br>
 @ref callbacks <br>
 @ref representing <br>
 @ref tipsandtricks <br><br>



 \section overview Overview

 Modus is an open source, cross-platform C++ library which allows you to handle music from code. This means that you can:
 
 <ul>
   <li>Manage interactive and adaptive music</li>
   <li>Use some kind of algorithm to improvise</li>
   <li>Represent visually (simulate) musical performances</li>
   <li>Select in real time the instruments that are going to play a previously written song</li>
   <li>Let the user take part on the performance through any type of interface, by playing an instrument, changing the tempo, 
       choosing the instruments, designing the structure of the song, etc.</li>
   <li>Define song structures with metric modulations, accelerandos and ritardandos</li>
   <li>Write your own scores, which can then be assigned to instruments to be played</li>
   <li>Play along with a pre-recorded song or represent the performance</li>
   <li>Everything else that comes into your head</li>        
 </ul>

 Modus works with instrument objects, which represent the instruments in real life, in addition to other more abstract concepts 
 such as samplers, voices, noise generators or whatever. You need to attach each instrument object to a sound generator to make 
 it sound, and then you can play notes by just calling a method. <br>
 
 Besides the possibility of calling the instrument object to play each note, you can also use a powerful tool: the score object. 
 This object tells an instrument what to play, and has the advantage that can be programmed dynamically. In order to make this 
 process easier, Modus provides you with a script syntax, so you can just write a script and load it into the score, either 
 from a plain text file or through a string inside the code. <br>
 
 But if you don't want to bother to write scripts for entire songs -what might be a lot of work-, just use the <i>MIDI-to-MScore</i>
 tool, which generate scripts automatically from MIDI files, so you have the possibility to utilize your favorite MIDI editor or score 
 editing software to enter the music and then create scripts for Modus. <br>
 
 Modus provides objects to work with harmony and scales too, including lists -in the same way of the score object- and static classes 
 which allow you to easily build note maps with scales, chords and voicings. <br>
 
 Along with this release of Modus a MIDI sound generator, based on the RtMidi library, in addition to three different audio sound 
 generators based on the following engines have been included:
 
 <ul>
   <li>FMOD Ex</li>
   <li>OpenAL</li>
   <li>irrKlang</li>
 </ul>
 
 Also, you have at your disposal some instruments to be used with these sound generators, including complete sample-packs, so you can 
 start to generate music from the first moment! <br><br>

 <div align="center">
 <i>NOTE: <b>FMOD Ex</b> and <b>irrKlang</b> are not free products. They can only be used free <br>
    of charge for applications that are not going to be commercially released. So if you are <br>
    going to utilize either FMOD or irrKlang with your applications, please keep in mind that <br>
    they cannot be commercial products unless you buy the corresponding license. Visit <br>
	FMOD's or irrKlang's web site in order to get further information about this.</i>
 <br><br>
 </div>
 

 \section license License

 Modus has been released under the MIT license. That means that you can freely use Modus both for commercial and non-commercial 
 applications and you are also allowed to modify and/or distribute it (in that case, please read "license.txt").
 <br><br>
 

 \section components Components
 
 Modus is made up of the following componentes:
 
 <ul>
   <li>Base library. Every application that makes use of Modus must include this library. The base library is written in standard 
       C++, so it's platform independent.</li><br>
	   
   <li>Sound generators. Objects that depend on external audio/MIDI engines and are capable of generating sound through the sound 
       card. A sound generator may be cross-platform as long as the base engine is also cross-platform.</li><br>
	   
   <li>Extensions. Modules which utilize AI processes and are designed to interact with the base library's objects. Along with this 
       release, an improviser object and a humanizer static class are provided. They are intented to be cross-platform too.</li>
 </ul>
 <br>
 
 
 \section nomenclature Nomenclature

 <ul>
   <li>Simple data or vector type definitions: <b>MT...</b></li>
   <li>Data structures: <b>MS...</b></li>
   <li>Classes: <b>MC...</b></li>
 </ul>
 <br>
 

 \section musicalnotes Musical notes
  
 In Modus you will always work with musical notes. Each note is defined by 8 values, some of which are not required. Thinking on the users 
 who are used to the MIDI protocol, Modus was intended to work with the same range of values to define musical notes:
 
 <ul>
   <li>Pitch. Defined by the MIDI pitch value, where the corresponding value for the middle C (C4) is 60.</li>
   
   <li>Intensity. Similar to the velocity in the MIDI's world (1-127). Intensity 0 means "rest".</li>
 </ul>
 
 <br> The rest of the values that define a musical note in Modus are:
 
 <ul>
   <li>Duration. Given in ticks (see "Time"). Duration 0 means that the note must not be released until the sample's end is reached or 
       a rest comes.</li>

   <li>Channel. For the instruments to be polifonic (that is to say, to be able to play several notes at the same time), you have to 
       work with channels. If an instrument has N channels, that means that it is capable to play up to N independent notes at the same time. 
	   For example, a guitar would have 6 channels since it has 6 independent strings. The channel is defined through a value between 0 and N-1.</li>

   <li>Mode. Sometimes you may want to use an instrument in some different ways instead of creating two or more instruments (for example, 
       a trumpet can play with or without mute, or a drum set can be played either with sticks or with brushes). With this purposal you have the 
	   mode field at your disposal, so that you would only have to specify the mode for each note to indicate how it should be played.</li>

   <li>Metadata. Into the data structure that represents musical notes, three metadata fields have been added for you to use them in the 
       way you want. These additional fields will be normally used to represent visually musical performances, by being filled with technical 
	   details such as finger on piano, string and fret on guitar, state of the keys/valves on wind instruments, etc.
 </ul>
 <br>
 
 
 \section time Time
 
 The time positions in Modus are defined by 3 values: measure, beat and tick. The "tick" is the smallest unit of time in Modus. The number 
 of beats per measure will be defined by the user, whereas each beat always contains 24 ticks. Thus, regardless of the musical time unit 
 you want to use you will have great versatility to represent what you want. In the following tables you can see the relationship between 
 the rhythmic figures and the number of ticks: <br>
 
 <div align="center">
 <i>Rhythmic unit: quarter note</i>
 <table border="1" cellpadding=10>
  <tr>
   <th>Figure</th>
   <th>Number of ticks</th>
  </tr>
  <tr>
   <td><img src="note_w.jpg"></td>
   <td>96</td>
  </tr>
  <tr>
   <td><img src="note_hd.jpg"></td>
   <td>72</td>
  </tr>
  <tr>
   <td><img src="note_h.jpg"></td>
   <td>48</td>
  </tr>
  <tr>
   <td><img src="note_4d.jpg"></td>
   <td>36</td>
  </tr>
  <tr>
   <td><img src="note_4.jpg"></td>
   <td>24</td>
  </tr>
  <tr>
   <td><img src="note_8d.jpg"></td>
   <td>18</td>
  </tr>
  <tr>
   <td><img src="note_4tr.jpg"></td>
   <td>16</td>
  </tr>  
  <tr>
   <td><img src="note_8.jpg"></td>
   <td>12</td>
  </tr>
  <tr>
   <td><img src="note_8tr.jpg"></td>
   <td>8</td>
  </tr>
  <tr>
   <td><img src="note_16.jpg"></td>
   <td>6</td>
  </tr>  
  <tr>
   <td><img src="note_32.jpg"></td>
   <td>3</td>
  </tr>    
 </table>
 
 <br><br>
 
 <i>Rhythmic unit: dotted quarter note</i>
 <table border="1" cellpadding=10>
  <tr>
   <th>Figure</th>
   <th>Number of ticks</th>
  </tr>
  <tr>
   <td><img src="note_hd.jpg"></td>
   <td>48</td>
  </tr>
  <tr>
   <td><img src="note_4d.jpg"></td>
   <td>24</td>
  </tr>
  <tr>
   <td><img src="note_4.jpg"></td>
   <td>16</td>
  </tr>
  <tr>
   <td><img src="note_8d.jpg"></td>
   <td>12</td>
  </tr>
  <tr>
   <td><img src="note_8.jpg"></td>
   <td>8</td>
  </tr>
  <tr>
   <td><img src="note_16.jpg"></td>
   <td>4</td>
  </tr>
  <tr>
   <td><img src="note_32.jpg"></td>
   <td>2</td>
  </tr>
 </table>
 </div>
 
 <br><br>
 
 
 \section mainobjects Main objects
 
 <b>MCInstrument</b><br>
 
 The instrument object represents something that can play music, like, for example, a real instrument. It provides you with the <i>playNote()</i>
 and <i>release()</i> methods, so it's very simple and intuitive to use. <br>
 
 To create an instrument object, you need to specify a number of channels. The number of channels an instrument has determines how many
 independent notes the instrument can play at the same time. For example, a string instrument would have so many channels as strings, 
 whereas a wind instrument would have only one channel (unless you want it to be able to produce multiphonics or something like that). <br>
 
 Besides just playing and releasing notes, other interesting things you can do with an instrument are bend notes, use a "damper pedal" 
 (sustain the notes), define a transposition, set an intensity variation or define callback functions to be called when specific events 
 occur (see "Callbacks" section). <br><br>
 
 <b>MCTimer</b><br>
  
 This object is the heart of all applications which utilize Modus. It calls a callback function each time a new tick comes, so that you 
 will be able to update the instruments and do all the stuff you need from there. For the timer to work, you need to update it from inside 
 a loop. You can either make this call from an independent thread or include it into the main loop (in case your application has one):

 \code
 while(!bAppEnd)
 {
     // check input
     ...
 
     // update data
     ...
	 
     // update music timer
     mTimer->update();

     // render
     ...
 }
 \endcode
 
 \code
 while(!bThreadEnd)
 {
     mTimer->update();
 }
 \endcode

 The <i>update()</i> method returns a boolean value that indicates whether the update took place or not. <br><br>
 The timer object can be used in two different ways:
  
 <ul>
   <li>Fixed values. It has a tempo and a number of beats per measure, and these values are fixed. You can change them as many times as 
       you want manually, though.</li>

   <li>Song structure. This option is more powerful and is the best way to manage situations in which you want to include metric modulations 
       and/or accelerandos or ritardandos. To specify a song structure there is the MCSongStructure object, which is attached to the timer 
	   through the <i>setSongStructure()</i> method.</li>
 </ul>

 <br>
 <b>MCScore</b><br>
 
 It's basically an ordered list of notes to play. The good part of the score object is that its data can be modified in real time, so you have 
 the possibility to use it with different purposes:
 
 <ul>
   <li>As a traditional score: write the score, and then an instrument will perform the music that has been written on it.</li>
   
   <li>As a kind of buffer: write notes on the score directly from software and play them after (for example, a music recorder).</li>
   
   <li>Mixing both ideas: add, modify or remove notes from software on a previously written score.</li>
 </ul>
 <br>

 
 \section soundgenerators Sound generators

 The sound generator is the way through which the instrument objects produce sound in Modus. They are based on external libraries and 
 can generate sound either by synthesis (MIDI) or by playing digital audio samples (audio). When you want an instrument object to sound, 
 you only have to create an instance of a sound generator, configure it and attach it to the instrument object. <br> 
 
 First of all, you should read some notes regarding the audio sound generators:
 
 <ul>
   <li>Never delete a MCSoundGenAudio object after having released the corresponding audio system. That would cause an error due 
       to invalid memory access.</li>
	   
   <li>You don't need to unload the samples that you previously loaded by calling <i>unloadSamples()</i>, unless you want to replace them with 
       others. That is because the destructor of the class does it already when you delete the object.</li>
	   
   <li>When using FMOD-based sound generators, make sure that you are calling periodically the FMOD system's <i>update()</i> method, since through 
       this method FMOD handles the virtualization of the voices and allows you to use as many channels as you want with your instruments. 
	   In addition, you will need to update FMOD if you want your instruments to change their positions, either on stereo or multichannel 
	   speaker systems.</li>
	   
   <li>When using FMOD-based sound generators, keep in mind that you can specify the DSP buffer size at the moment of the initialization
       through the FMOD system's <i>setDSPBufferSize()</i> method. The smaller this value, the lower the latency, what can be very important for 
	   applications in which one or more users have to play music in real time. So in these cases, try to use a small value as DSP buffer 
	   size, but be aware that a too small value would cause an unwanted distortion.
 </ul>
 
 The sound generators must be updated with each new tick, but if the sound generator is attached to an instrument, you will only need to update 
 the instrument, since the instrument updates its sound generator automatically. <br><br>
 
 
 \section samplesets Sample-sets
 
 All the audio-based sound generators work with audio samples. Each time a note is requested to be played, the sound generator finds the
 appropriate sample to make it sound. For the sound generator to know which sample must be played for each note, you 
 need to define a list of sample-sets. In order to define a sample-set, the following parameters must be specified:

 <ul> 
   <li>ID: each sample-set in the sound generator will have its own ID number.</li>
   
   <li>Path: path in which the sample files are located, with the '/' or '\\' symbol (depending on the platform) at the end.</li>
   
   <li>Format: extension of the sound files, without dot at the beginning.</li>
   
   <li>Mode: the samples contained in the sample-set will be assigned to an unique mode.</li>
   
   <li>Intensity range: the samples contained in the sample-set will be selected only when the note's intensity is inside this range.</li>
   
   <li>Pitch range: these two values define how many samples are in the sample-set.</li>
 </ul>

 Once you have defined all these values for the sample-set (MSSampleSet struct), then you can add it to the sound generator's sample-set 
 list through the <i>addSampleSet()</i> method. When all the sample-sets you want to use for the sound generator have been added, you just have 
 to call the <i>loadSamples()</i> method to load the sample files into memory. <br>
 
 Previously, of course you need to have all the sample files ready to be loaded. For each sample-set:

 <ol>
   <li>Select the audio files you want to load. Make sure that all of them have the same format and the same extension.</li>
   
   <li>Place them together into the same folder (that will be the folder you have to specify in the field "path").</li>
   
   <li>Name each sample file like this: <sample-set ID (two digits) - underscore - pitch (three digits) - dot - format> <br>
       (Example: 00_060.wav, 00_061.wav, 00_062.wav...)</li>
 </ol>
 <br>
 
 
 \section samplepacks Sample-packs

 The sample-packs are a very comfortable way to manage sample-sets. A sample-pack is an unique file which contains both information 
 about all the sample-sets included in it and all the audio files defined by these sample-sets. The sample-pack files have the extension 
 ".msp" ("Modus Sample Pack"). <br>
 
 In order to create your own sample-packs, you have to prepare the audio files in the same way it was explained in the previous section, 
 and then you can use the <i>msamplepack</i> tool (included with Modus). <br>
 
 To load a sample-pack file from a sound generator object, you only have to call the <i>loadSamplePack()</i> method. <br><br><br>
  

 \section projectsettings Project Settings
 
 Whatever are the development environment, compiler or linker you are working with, you will need to follow these steps to set up
 a project that utilizes Modus:
 
 <ol>
 
 <li>
 Add include directories. Modus/include and Modus/soundgen are required in all cases, and if you want to use any extension you will 
 also need to add Modus/extensions.
 </li>
 
 <li>
 Add Modus library. You will find the appropriate library file in the Modus/lib/[platform] directory.
 </li>
 
 <li>
 Add sound generator's source files. This means that you have to add the cpp files in the Modus/soundgen/[chosen_soundgen] directory.
 <br><br>
 <b>Note for iOS:</b> use "mxsoundgenopenal.mm" instead of "mxsoundgenopenal.cpp".
 </li>
 
 <li>
 Add sound generator's external dependencies. All the files you may need are inside the Modus/soundgen/externals/[library] directory.
 If you are using OpenAL, don't forget to include also the Ogg Vorbis libraries in case you want to load audio samples in this format
 (the sample-packs that come along with the examples are made up of Ogg Vorbis audio files), as OpenAL does not support compressed
 audio decoding in itself.
 <br><br>
 <b>Note for iOS:</b> OpenAL is included as a framework, so you will just need to add OpenAL.framework to the project
 to use the OpenAL-based sound generator. On the other hand, keep in mind that you can work with any compressed audio format
 supported by the AudioToolBox framework (such as AAC or IMA4).
 </li>
 
 </ol>
  
 <br>
 
 
 \section notemaps Note maps
 
 One interesting feature in Modus is the MTNoteMap. It just consist of a list of notes (pitches), but may be very useful for some 
 particular tasks, though. 
 
 <ul>
   <li>The MCNoteMaps static class offers several methods to work with note maps</li>
   
   <li>MCChords works with note maps, considered as chord patterns</li>
   
   <li>MCScales works with note maps, considered as scale patterns</li>
 </ul>

 Note maps can be used to restrict the set of notes which should be allowed to be played in a particular moment. For example, to improvise 
 you might want to let your algorithm generate notes that belong to a specific scale inside a specific range. <br><br>
 
 
 \section scripts Scripts
 
 They can be written either in strings or text files and then loaded from some of the objects, through the <i>loadScriptFromString()</i>
 and the <i>loadScriptFromFile()</i> methods, respectively. In addition, you have the possibility of creating binary files from previously
 written text files using the included <b>mscript2bin</b> tool, and load them with the <i>loadScriptFromBinaryData()</i> and the
 <i>loadScriptFromBinaryFile()</i> methods. Scripts in binary format are smaller and faster to load than scripts in text format, but they
 cannot be modified once generated.<br><br>
 
 <b>MCScore</b><br>
 
 Since a score object is basically a list of notes attached to time positions, there are commands to specify note values and time 
 values. The following table includes the commands which are referred to the time:
 
 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>Command</th>
   <th>Description</th>
   <th>Value range</th>
  </tr>
  <tr>
   <td>bpm:&lt;val&gt;</td>
   <td>Beats per measure</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>[m:&lt;val&gt;]</td>
   <td>Time position: measure</td>
   <td>0-...</td>
  </tr>
  <tr>
   <td>[b:&lt;val&gt;]</td>
   <td>Time position: beat</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>[t:&lt;val&gt;]</td>
   <td>Time position: tick</td>
   <td>0-23</td>
  </tr>  
 </table>
 </div>
 
 <br> Through these commands the time position's cursor will be set at the specified position, so the next notes you write will have 
 assigned that time position. The commands referred to time positions must always be inside square brackets and can be combined 
 as follows:
 
 \code
 [m:1 b:3 t:12]  ; current time position -> Measure 1, Beat 3, Tick 12
 [m:3]           ; current time position -> Measure 3, Beat 1, Tick 0
 [m:4 b:2]       ; current time position -> Measure 4, Beat 2, Tick 0
 \endcode
 
 <br>To add notes to the score, you can use these commands:
 
 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>Command</th>
   <th>Description</th>
   <th>Value range</th>
  </tr>
  <tr>
   <td>p:&lt;val&gt;</td>
   <td>Note pitch</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>ch:&lt;val&gt;</td>
   <td>Channel</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>d:&lt;val&gt;</td>
   <td>Note duration</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>i:&lt;val&gt;</td>
   <td>Note intensity</td>
   <td>0-127</td>
  </tr>
  <tr>
   <td>mode:&lt;val&gt;</td>
   <td>Mode</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>mda:&lt;val&gt;</td>
   <td>Metadata A</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>mdb:&lt;val&gt;</td>
   <td>Metadata B</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>mdc:&lt;val&gt;</td>
   <td>Metadata C</td>
   <td>0-255</td>
  </tr>
  <tr>
   <td>+</td>
   <td>Add duration to the current time position</td>
   <td>---</td>
  </tr>
  <tr>
   <td>></td>
   <td>Insert note into score</td>
   <td>---</td>
  </tr>
  </table>
 </div> 
 
 <br> It's important to keep in mind that you don't need to set all these values each time you write a note on the script, 
 but they stay until you change them again. This means that if you write "i:96" and then several notes, all these notes 
 will be interpreted with 96 as their intensity. There is one only exception: the metadata values, which are automatically 
 set to 0 every time there is an insert command (>) on the script.
 
 The add duration command (+) may be useful if you want to write a legato-style line, so that the current note's release 
 moment is the same than the moment in which the next one must start to sound. If you place the add duration command (+) 
 and the insert command (>) on the same line, the duration of the current note will be added to the current time position 
 automatically, so you won't need to specify the time position in which the next note comes:
 
 \code
 [m:5]
 p:60 i:64 d:12 > +    ; added at time position (5, 1, 0)
 p:62 i:64 d:6 > +     ; added at time position (5, 1, 12)
 p:64 i:64 d:6 > +     ; added at time position (5, 1, 18)
 p:60 i:64 d:24 >      ; added at time position (5, 2, 0)
 \endcode
 <br>

 Examples:
 
 <img src="score1.jpg">
 \code
 bpm:4 mode:0 ch:0 i:64

 p:72 d:48 > +
 p:76 d:24 > +
 p:79 d:24 > +

 p:71 d:36 > +
 p:72 d:6 > +
 p:74 d:6 > +
 p:72 d:24 > +
 p:0 d:24 > +
 \endcode
 <br><br>

 <img src="score2.jpg">
 \code
 bpm:3 mode:0 ch:0 i:64

 [m:1 b:3]
 p:63 d:21 >
 [t:21]
 p:62 d:3 >

 [m:2]
 p:62 d:24 >
 [b:3]
 p:62 d:21 >
 [t:21]
 p:63 d:3 >

 [m:3]
 p:63 d:24 >
 \endcode
 <br><br>
 
 <img src="score3.jpg">
 \code
 bpm:4 mode:0 i:64 d:48

 p:67 ch:0 >
 p:72 ch:1 >
 p:76 ch:2 > +

 p:69 ch:0 >
 p:72 ch:1 >
 p:77 ch:2 > +

 p:67 ch:0 >
 p:71 ch:1 >
 p:74 ch:2 > +

 p:67 ch:0 >
 p:72 ch:1 >
 p:76 ch:2 > +
 \endcode
 <br><br>
 
 <b>MCHarmonyPattern</b><br>
  
 First of all, let's talk about the definition string: in order to reference chord patterns from the scripts, you will need 
 to have assigned an unique ID number to each chord pattern you want to have at your disposal. This can be done by writing 
 the definition string. To know its structure and be able to customize it, you can take a look to the default definition 
 placed both in the "scripts" directory and in the "mchords.cpp" source file: <br>
 
 \code
 ;-------------------------------
 ;
 ; Chords definition file
 ;
 ;-------------------------------

 0: 0,4,7           ; M
 1: 0,4,7,9         ; M6
 2: 0,4,7,11        ; M7
 3: 0,4,8           ; Maug5
 
 4: 0,3,7           ; m
 5: 0,3,7,10        ; m7
 6: 0,3,6,10        ; m7b5
 7: 0,3,7,9         ; m6
 8: 0,3,7,11        ; mM7

 9: 0,4,7,10        ; 7
 10: 0,4,7,10,14    ; 9
 11: 0,4,7,10,13    ; 7m9

 12: 0,3,6          ; dis
 13: 0,3,6,9        ; dis7

 14: 0,5,7          ; sus4
 15: 0,5,7,10       ; 7sus4

 16: 0,4,6,10       ; 7b5
 17: 0,4,8,10       ; 7aug5
 \endcode
 
 <br> Once you have either built your own definition string or decided to use the one that comes by default, then you can write 
 the scripts based on it. The commands referred to the time position are the same that you already know. The rest of the commands are:
 
 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>Command</th>
   <th>Description</th>
   <th>Value range</th>
  </tr>
  <tr>
   <td>root:&lt;val&gt;</td>
   <td>Root note</td>
   <td>0-11</td>
  </tr>
  <tr>
   <td>chord:&lt;val&gt;</td>
   <td>Chord pattern's ID number</td>
   <td>0-...</td>
  </tr>
  <tr>
   <td>bass:&lt;val&gt;</td>
   <td>Bass note (only if different than root)</td>
   <td>0-11</td>
  </tr>
  <tr>
   <td>tensions:&lt;val,...&gt;</td>
   <td>List of tensions, separated by commas</td>
   <td>0-...</td>
  </tr>
  <tr>
   <td>></td>
   <td>Insert entry</td>
   <td>---</td>
  </tr>
  </table>
 </div> 
 
 <br> Example:
 
 \code
 [m:1]
 root:5  chord:9 tensions:14,21 >

 [m:2]
 root:10 chord:9 tensions:14,21 >

 [m:3]
 root:5  chord:9 tensions:14,21 >

 [m:5]
 root:10 chord:9 tensions:14,21 >

 [m:7]
 root:5  chord:9 tensions:14,21 >

 [m:9]
 root:7  chord:5 >

 [m:10]
 root:0  chord:9 tensions:14,21 >

 [m:11]
 root:5  chord:9 tensions:14,21 >

 [m:12]
 root:7  chord:5 >
 [b:3]
 root:0  chord:9 tensions:15 >
 \endcode
 
 
 <br><b>MCScalePattern</b><br>
 
 The definition string works in the same way than we have already seen with the harmony pattern. In this case the default 
 definition is placed both in the "scripts" directory and in the "mscales.cpp" source file: <br>
 
 \code
 ;-------------------------------
 ;
 ; Scales definition file
 ;
 ;-------------------------------
 
 0: 0,2,4,5,7,9,11              ; Major
 1: 0,2,3,5,7,8,10              ; Minor
 2: 0,2,3,5,7,8,11              ; Minor Harmonic
 3: 0,2,3,5,7,9,11              ; Minor Melodic
 4: 0,2,4,5,7,8,10              ; Major-Minor

 5: 0,2,3,5,7,9,10              ; Dorian
 6: 0,1,3,5,7,8,10              ; Phrygian
 7: 0,2,4,6,7,9,11              ; Lydian
 8: 0,2,4,5,7,9,10              ; Mixolydian
 9: 0,1,3,5,6,8,10              ; Locrian

 10: 0,2,4,6,7,9,10             ; Acoustic
 11: 0,1,3,4,6,8,10             ; Altered

 12: 0,2,4,7,9                  ; Pentatonic Major
 13: 0,3,5,7,10                 ; Pentatonic Minor
 14: 0,3,5,6,7,10               ; Blues
 15: 0,3,5,6,7,9,10             ; Blues + Sixth

 16: 0,1,4,5,7,8,10             ; Gypsy
 17: 0,1,4,5,7,8,11             ; Double Harmonic

 18: 0,2,4,6,8,10               ; Whole Tone
 19: 0,2,3,5,6,8,9,11           ; Octatonic (H-W)
 20: 0,1,3,4,6,7,9,10           ; Octatonic (W-H)
 21: 0,1,2,3,4,5,6,7,8,9,10,11  ; Chromatic
 \endcode
 
 <br> The time position commands are also the same, whereas the specific commands work as follows:
 
 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>Command</th>
   <th>Description</th>
   <th>Value range</th>
  </tr>
  <tr>
   <td>root:&lt;val&gt;</td>
   <td>Root note</td>
   <td>0-11</td>
  </tr>
  <tr>
   <td>scale:&lt;val&gt;</td>
   <td>Scale pattern's ID number</td>
   <td>0-...</td>
  </tr>
  <tr>
   <td>></td>
   <td>Insert entry</td>
   <td>---</td>
  </tr>
  </table>
 </div> 
 
 <br> Example:
 
 \code
 [m:1]
 root:10 scale:8 >

 [m:5]
 root:3  scale:8 >

 [m:7]
 root:10 scale:8 >

 [m:9]
 root:5  scale:8 >

 [m:10]
 root:3  scale:8 >

 [m:11]
 root:8  scale:10 >
 \endcode
 
 
 <br><b>MCSongStructure</b><br>
 
 The song structure scripts don't use a time position cursor, so there are no square-brackets commands. When a new value
 is assigned to the tempo, the target tempo receive the same value unless you specify another value for it, so you will 
 only have to worry about the target tempo if you want to program an accelerando or ritardando. <br>
 
 To write song structure scripts you have the following commands at your disposal: <br>
 
 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>Command</th>
   <th>Description</th>
   <th>Value range</th>
  </tr>
  <tr>
   <td>bpm:&lt;val&gt;</td>
   <td>Beats per measure</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>from:&lt;val&gt;</td>
   <td>First measure of the section</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>to:&lt;val&gt;</td>
   <td>Last measure of the section</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>tempo:&lt;val&gt;</td>
   <td>Tempo</td>
   <td>1-...</td>
  </tr>
  <tr>
   <td>target:&lt;val&gt;</td>
   <td>Target tempo (accelerandos, ritardandos)</td>
   <td>1-...</td>
  </tr>        
  <tr>
   <td>></td>
   <td>Insert entry</td>
   <td>---</td>
  </tr>
  </table>
 </div> 
 
 <br> Example:
 
 \code
 bpm:4
 from:1 to:8 tempo:132 >
 from:9 to:12 tempo:180 >

 bpm:3
 from:13 to:20 tempo:120 >
 from:21 to:24 tempo:120 target:60 >    ; ritardando (120 -> 60)
 \endcode
 <br><br>
 
 
 \section pitchvalues Pitch values

 As we have already seen, in a MSNote structure the pitch is the note's MIDI pitch value. A MIDI pitch can take
 values from 0 ("note off" in MIDI, "rest" in Modus) to 127, since it is a 7-bit value, whereas a MSNote pitch 
 is a 8-bit value, so it can have values up to 255. Thus, pitch values between 128 and 255 will have a special
 meaning in Modus: some are predefined control values, and the rest are available for you to interpret them
 in the way you want. <br>
 
 The predefined control values are:
 
 <ul>
  <li>Pitch 128 - Bending.
   <ul>
    <li>Channel: channel through which the note to modify is sounding</li>
    <li>Mode: 0 up, 1 down</li>
	<li>Intensity: number of semitones</li>
	<li>MDA: number of cents</li>
	<li>Duration: number of ticks</li>
   </ul>
  </li>
  
  <li>Pitch 129 - Damper.
   <ul>
    <li>Intensity: 0 off, 1 on</li>
   </ul>
  </li>
  
  <li>Pitch 130 - Intensity change.
   <ul>
    <li>Channel: channel through which the note to modify is sounding</li>
	<li>Intensity: new intensity value</li>
	<li>Duration: number of ticks</li>
   </ul>
  </li>
  
  <li>Pitch 131 - Set release time.
   <ul>
	<li>Intensity: from 1 (slow) to 255 (fast). The default value is 100</li>
   </ul>
  </li>  

  <li>Pitch 132 - Set vibrato.
   <ul>
    <li>Channel: channel through which the note to modify is sounding</li>
	<li>Intensity: number of cents</li>
	<li>Duration: cycle length in number of ticks</li>
   </ul>
  </li>

 </ul>
 <br>


 \section callbacks Callbacks
 
 Besides the already known timer's tick callback, you can ask Modus to let you know when some events occur, so you 
 don't need to do a continuous polling to check them:
 
 <br><b>MCTimer</b><br>
 
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Tick</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A new tick has come</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Current time position</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackTick()</i></td>
  </tr>
 </table>
 
 <br>
 
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Section</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A new section has come (it must have been defined in a song structure object)</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Number of the current section</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackSection()</i></td>
  </tr>
 </table>

 <br>
  
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>End</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>The song has ended (all the song's sections must have been defined in a song structure object)</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>-</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackEnd()</i></td>
  </tr>
 </table>
 
 <br>
 
 <br><b>MCInstrument</b><br>
 
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Play</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A note is being played</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Note that is being played</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackPlay()</i></td>
  </tr>
 </table>
 
 <br>

 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Next note</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A note is being played and there are more notes in the score</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Note that comes after the current note or chord, current time moment and next note's time moment<br>
   (in case several notes are coming together this method will be called once for each note)</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackNextNote()</i></td>
  </tr>
 </table>
 
 <br>

 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Release</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A note is being released</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Note that is being released</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackRelease()</i></td>
  </tr>
 </table>
 
 <br>
 
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Bending</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A note is being bended</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Note and frequency variation in cents</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackBending()</i></td>
  </tr>
 </table>
 
 <br>

 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Damper</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>The state of the damper pedal has changed</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Current state of the damper pedal</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackDamper()</i></td>
  </tr>
 </table>
 
 <br>
 
 <table border="1" cellpadding=10>
  <tr>
   <th>Callback</th>
   <td>Intensity</td>
  </tr>
  <tr>
   <th>Called when</th>
   <td>A note's intensity is being changed</td>
  </tr>
  <tr>
   <th>Information</th>
   <td>Note and current intensity</td>
  </tr>
  <tr>
   <th>Method to register</th>
   <td><i>setCallbackIntensity()</i></td>
  </tr>
 </table>
 
 <br>
 

 \section representing Representing musical performances
 
 These callback services you have just known are a very good way to represent visually musical performances. A first approach 
 could be register play and release callback functions and do something on the screen each time one of these events happens. <br>
 
 However, this solution is not valid if you want to include animations, because in that case you will need to know which note is 
 the next one and when it must be played before it comes (for example, to take the player's arm to the correct place before playing 
 the note). For this purpose the instrument object's "Next note" service may be particularly useful: it calls your previously defined 
 function each time a note or chord is played, indicating the current time moment, the time moment in which the next note is going to
 be played, and which note it is. <br>
 
 But if you want to go one step further and represent a performance accurately, besides the animations you should take into account 
 that into each note (MSNote structure) there are three metadata fields (MDA, MDB, MDC) at your disposal, which can store <i>unsigned 
 char</i> values (from 0 to 255). These values will be typically utilized for representing technical aspects of the performance, for 
 example:
 
 <ul>
  <li>Piano: hand (left or right), finger</li>
  <li>Guitar: string, fret, right hand technique</li>
  <li>Wind instrument: positioning, state of the holes/keys/valves</li>
  <li>Percussion: strike's point</li>
 </ul>
 
 Lastly, you may also want to include other things such as musician's movements or something like that. For this purposal, the best way 
 would be to use pitch values out of the instrument's range to denote additional things that you can define and then interpret in the 
 way you want. <br>
 
 In summary: as you can see, with Modus you have a complete platform to represent or simulate musical performances. All you 
 have to do is fill your scores with the appropriate values along with a good graphic work, and you will be able to achieve excelent 
 results. In the following table you can see all the possibilities together: <br>

 <div align="center">
 <table border="1" cellpadding=10>
  <tr>
   <th>If you want to represent...</th>
   <th>...then you'll need</th>
  </tr>
  <tr>
   <td>Schematic performance</td>
   <td>Play callback<br>Release callback<br>[Damper callback]<br>[Bending callback]<br>[Intensity callback]</td>
  </tr>
  <tr>
   <td>Animations</td>
   <td>Next note callback</td>
  </tr>
  <tr>
   <td>Technical details</td>
   <td>Metadata fields</td>
  </tr>
  <tr>
   <td>Musician's movements, gestures, etc.</td>
   <td>Pitches out of range</td>
  </tr>
 </table>
 </div>
 <br>

 
 \section tipsandtricks Tips and tricks
 
 <ul> 
 <li>
 Loop management. This can be done easily inside the timer's tick callback function, by checking the current time 
 position and displacing the score each time a multiple of the length of the loop -given in number of measures- is 
 reached:
 
 \code
 void CallbackTick(const MSTimePosition& mPos, void* pData)
 {
     // loop management
     if(mPos.Measure % LOOP == 1 && mPos.Measure > LOOP && mPos.Beat == 1 && mPos.Tick == 0)
     {   
         mScore->displace(LOOP);
     }

     // update instruments
     ...
 }
 \endcode
 
 The loop management code must be always placed before the instrument's update call. Otherwise, the note(s) on the 
 measure 1, beat 1 and tick 0 in the score wouldn't be played.
 </li>
 
 <br>
 
 <li>
 Score on countoff. Notes cannot be placed into a score to be played before the measure 0, but that can be done by 
 following this simple trick (remember that you can make displacements with negative numbers):
 
 \code
 MCScore mScore;
 mScore.loadScriptFromFile("myscore.txt");
 mScore.displace(-2);
 \endcode
 </li>

 <br>
 
 <li>
 "Repeat signs" in a score from code. If you want a certain section to be repeated several times, you can use a
 score object to store that section and then add it to the final score as many times as you need:
 
 \code
 MCScore mSection;
 MCScore mFinalScore;
 
 // fill the section with notes
 ...
 
 const int iNumberOfRepetitions = 4;
 const int iSectionLengthInMeasures = 16;
 
 for(int i = 0; i < iNumberOfRepetitions; i++)
 {
     mFinalScore.addScore(mSection);
     mSection.displace(iSectionLengthInMeasures);
 }
 \endcode
 </li>
 
 <br>

 <li>
 Separated hands. Don't forget the possibility to write several scripts and combine them to be played by the same 
 instrument when the music to write is too complex. For example, in a piano piece you could write a script for each 
 hand and then put it all together:
 
 \code
 MCScore mScoreLeftHand;
 mScoreLeftHand.loadScriptFromFile("score.lefthand.txt");

 MCScore mScorePiano;
 mScorePiano.loadScriptFromFile("score.righthand.txt");
 mScorePiano.addScore(mScoreLeftHand);

 mPiano->setScore(&mScorePiano);
 \endcode
 </li>

 <br>

 <li>
 Sforzando-piano. For wind instruments it can be achieved by playing a note with a high intensity and then setting a quick 
 change of intensity like this:

 \code
 MSNote mNote;
 mNote.Pitch = 48;
 mNote.Intensity = 127;
 mNote.Channel = 0;
 mNote.Duration = 0;

 mTenorSax->playNote(mNote);
 mTenorSax->changeIntensity(0, 48, 4);
 \endcode
 </li>

 <br>

 <li>
 Metronome. It's very easy to create a simple metronome in case you need a tempo reference. One way to do it is playing 
 a note inside the timer's tick callback function, like this:
 
 \code
 void CallbackTick(const MSTimePosition& mTimePosition, void* pData)
 {
     // metronome
     if(mTimePosition.Tick == 0)
     {
         MSNote mClick;
         mClick.Pitch = (mTimePosition.Beat == 1)? 81: 69;
         mClick.Intensity = 127;
         mClick.Duration = 4;
         mPiano->playNote(mClick);
     }
     
     // update instruments
     ...
 }
 \endcode

 Remember that the instrument's update must be always done after the rest of the code in the timer tick callback function! <br>
 </li>

 <br>

 <li>
 Improviser. Try not to generate music for too many measures when you use the improviser, since the <i>improvise()</i> method could 
 need much CPU usage and it might appear a small pause in the music each time you call it. If you need to generate a large number 
 of measures, you can divide them into smaller pieces so the improviser does not take so long -for example, for 64 measures you could 
 call the improviser four times with 16, or even eight times with 8-. The <i>setPreviousNote()</i> and <i>setIntensity()</i> methods 
 are provided for you to be able to ensure the continuity of the improvisation.
 </li>

 </ul>

 <br><br>

 <div align="center">
 Copyright (c) 2012-2013 Arturo Cepeda P&eacute;rez
 </div>

 */
