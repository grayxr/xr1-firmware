#include <Arduino.h>
#include <SPI.h>
#include <MIDI.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_TLC5947.h>
#include <Adafruit_MPR121.h>
#include <FastTouch.h>
#include <Keypad.h>
#include <Audio.h>
#include <Config.h>
#include <AudioConfig.h>
#include <uClock.h>
#include <U8g2lib.h>
#include <Fonts.h>
#include <map>
#include <string>

void logMetrics(void);
void initMain(void);
void u8g2_prepare(void);
void initEncoders(void);
void encoder_set(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop);
void encoder_setValue(int addr, int16_t rval);
void initDACs(void);
void writeToDAC(int chip, int chan, int val);
bool btnCharIsATrack(char btnChar);
void toggleSequencerPlayback(char btn);
void toggleSelectedStep(uint8_t step);
void initTrackSounds(void);
void initSoundsForTrack(int t);
void changeTrackSoundType(uint8_t track, TRACK_TYPE newType);
void changeSampleTrackSoundType(uint8_t t, TRACK_TYPE newType);
void swapSequencerMemoryForPattern(int newBank, int newPattern);
void configureVoiceSettingsForTrack(int t);
void configureVoiceSettingsOnLoad(void);
void configureSampleVoiceSettingsOnLoad(int t);
void initUsableSampleNames(void);
void initUsableWavSampleNames(void);
void ratchetTrack(uint8_t t, uint8_t note);
void noteOffForAllSounds(void);
void initProject();
void saveProject();
void loadLatestProject();
void wipeProject(void);
void initExternalSequencer(void);
void initExternalPatternMods(void);
void prepareSequencer(void);
void savePatternModsToSdCard(void);
void loadPatternModsFromSdCard(void);
void loadRawSamplesFromSdCard(void);
void parseRootForWavSamples(void);
void parseRootForRawSamples(void);
void rewindAllCurrentStepsForAllTracks(void);
void triggerTrackManually(uint8_t t, uint8_t note);
void triggerCvGateNoteOn(uint8_t t, uint8_t note);
void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note);
void triggerRawSampleNoteOn(uint8_t t, uint8_t note);
void triggerWavSampleNoteOn(uint8_t t, uint8_t note);
void triggerDexedNoteOn(uint8_t t, uint8_t note);
void triggerAllStepsForAllTracks(uint32_t tick);
void triggerRatchetingTrack(uint32_t tick);
void updateCurrentPatternStepState(void);
void updateAllTrackStepStates(void);
void updateTrackLength(int diff);
void updateTrackAmpEnvAttack(int diff);
void updateSubtractiveSynthWaveform(int diff);
void updateSubtractiveSynthNoiseAmt(int diff);
void updateSubtractiveSynthFilterEnvAttack(int diff);
void updateComboTrackLevel(int diff);
void handleQueueActions(void);
void handleSoloForTrack(uint8_t track, bool undoSoloing);
void handleRawSampleNoteOnForTrack(int track);
void handleWavSampleNoteOnForTrack(int track);
void handleDexedNoteOnForTrack(int track);
void handleSubtractiveSynthNoteOnForTrack(int track);
void handleCvGateNoteOnForTrack(int track);
void handleRawSampleNoteOnForTrackStep(int track, int step);
void handleWavSampleNoteOnForTrackStep(int track, int step);
void handleDexedNoteOnForTrackStep(int track, int step);
void handleSubtractiveSynthNoteOnForTrackStep(int track, int step);
void handleMIDINoteOnForTrackStep(int track, int step);
void handleCvGateNoteOnForTrackStep(int track, int step);
void handleNoteOnForTrack(int track);
void handleNoteOnForTrackStep(int track, int step);
void handleNoteOffForTrackStep(int track, int step);
void handleNoteOffForTrack(int track);
void handleRemoveFromStepStack(uint32_t tick);
void handleEncoderSubtractiveSynthModA(int diff);
void handleEncoderSubtractiveSynthModB(int diff);
void handleEncoderSubtractiveSynthModC(int diff);
void handleEncoderSubtractiveSynthModD(int diff);
void handleEncoderDexedModA(int diff);
void handleEncoderDexedModB(int diff);
void handleEncoderDexedModC(int diff);
void handleEncoderDexedModD(int diff);
void handleEncoderRawSampleModA(int diff);
void handleEncoderRawSampleModB(int diff);
void handleEncoderRawSampleModC(int diff);
void handleEncoderRawSampleModD(int diff);
void handleEncoderWavSampleModA(int diff);
void handleEncoderWavSampleModB(int diff);
void handleEncoderWavSampleModC(int diff);
void handleEncoderWavSampleModD(int diff);
void handleEncoderCvGateModA(int diff);
void handleEncoderCvGateModB(int diff);
void handleEncoderCvGateModC(int diff);
void handleEncoderCvGateModD(int diff);
void handleEncoderPatternModA(int diff);
void handleEncoderPatternModB(int diff);
void handleEncoderPatternModC(int diff);
void handleEncoderPatternModD(int diff);
void handleEncoderSetTrackMods(void);
void handleEncoderTraversePages(void);
void handleEncoderSetPatternMods(void);
void handleKeyboardSetRatchets(void);
void handleHeadphoneAdjustment(void);
void handleEncoderSetTempo();
void handleAddToStepStack(uint32_t tick, int track, int step);
void handleAddToRatchetStack();
void handleRemoveFromRatchetStack();
void handleSwitchStates(bool discard);
void handleKeyboardStates(void);
void handleEncoderStates(void);

void drawSequencerScreen(bool queueBlink = false);
void drawSetTempoOverlay(void);
void drawInitProject();
void drawCopySelOverlay(std::string type);
void drawCopyConfirmOverlay(std::string type, uint8_t num);
void drawPasteConfirmOverlay(std::string type, uint8_t num);
void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum);
void drawHatchedBackground();
void drawGenericOverlayFrame(void);
void drawErrorMessage(std::string message);
void drawSetupScreen();
void drawControlMods(void);
void drawPatternControlMods(void);
void drawPageNumIndicators(void);
void displayCurrentlySelectedBank(void);
void displayCurrentlySelectedPattern(void);
void displayCurrentlySelectedTrack(void);
void displayInitializedPatternLEDs();
void displayInitializedTrackLEDs();
void displayPageLEDs(int currentBar = -1);
void displayCurrentOctaveLEDs();
void displayPerformModeLEDs(void);
void displayMuteLEDs(void);
void setDisplayStateForAllStepLEDs(void);
void setDisplayStateForPatternActiveTracksLEDs(bool enable);

void setLEDPWM(uint8_t lednum, uint16_t pwm);
void setLEDPWMDouble(uint8_t lednum1, uint16_t pwm1, uint8_t lednum2, uint16_t pwm2);
void clearAllStepLEDs(void);
void clearPageLEDs();

void handle_bpm_step(uint32_t tick);
void handle_sixteenth_step(uint32_t tick);
void ClockOut96PPQN(uint32_t tick);
void ClockOut16PPQN(uint32_t tick);
void onClockStart();
void onClockStop();

bool isWavFile(const char* filename);
bool isRawFile(const char* filename);

int8_t getKeyLED(char idx);
int16_t encoder_getValue(int addr);

time_t getTeensy3Time();

const char* getKeyStr(char idx);

std::string getTrackTypeNameStr(TRACK_TYPE type);
std::string getTrackMetaStr(TRACK_TYPE type);
std::string strldz(std::string inputStr, const int zeroNum);
std::string getNewProjectName();;
std::string getWaveformName(uint8_t waveform);
std::string getDisplayNote(void);
std::string getPercentageStr(float rate);
std::string getPlaybackSpeedStr(float rate);
std::string getCurrPageNameForTrack(void);
std::string getLoopTypeName(void);

u_int8_t getKeyStepNum(char idx);

PANNED_AMOUNTS getStereoPanValues(float pan);

TRACK getHeapTrack(int track);
TRACK_STEP getHeapStep(int track, int step);
PATTERN getHeapCurrentSelectedPattern(void);
TRACK getHeapCurrentSelectedTrack(void);
TRACK_STEP getHeapCurrentSelectedTrackStep(void);

SOUND_CONTROL_MODS getSubtractiveSynthControlModData();
SOUND_CONTROL_MODS getRawSampleControlModData();
SOUND_CONTROL_MODS getWavSampleControlModData();
SOUND_CONTROL_MODS getDexedControlModData();
SOUND_CONTROL_MODS getMidiControlModData();
SOUND_CONTROL_MODS getCvGateControlModData();
SOUND_CONTROL_MODS getCvTrigControlModData();
SOUND_CONTROL_MODS getControlModDataForTrack();
SOUND_CONTROL_MODS getControlModDataForPattern();

void setup() {
  Serial.begin(9600);

  if (CrashReport) {
    Serial.print(CrashReport);
  }

  setSyncProvider(getTeensy3Time);
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");    

  new_project_name = getNewProjectName();  
  Serial.println("set project name");

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(50);
  Serial.println("buffered audio mem");

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.lineInLevel(0);
  sgtl5000_1.volume(1.0);
  sgtl5000_1.lineOutLevel(29);
  Serial.println("enabled sgtl");

  MIDI.begin();
  Serial.println("enabled MIDI");

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  Serial.println("setup SD card");

  current_UI_mode = PROJECT_BUSY;

  // discard any dirty reads
  handleSwitchStates(true);
  Serial.println("handled switch states during project busy");

  // prepare sample names to be occupied
  initUsableSampleNames();
  Serial.println("init'd usable sample names");

  initTrackSounds();
  Serial.println("init'd track sounds");
  
  delay(25);

  Wire1.begin();
  Serial.println("started Wire1");

  SPI1.begin();
  SPI1.setMOSI(DAC_MOSI);
  SPI1.setSCK(DAC_SCK);
  Serial.println("started DAC SPI");

  pinMode(CS1, OUTPUT); // CS
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT); // CS
  digitalWrite(CS2, HIGH);
  pinMode(CS3, OUTPUT); // CS
  digitalWrite(CS3, HIGH);
  pinMode(CS4, OUTPUT); // CS
  digitalWrite(CS4, HIGH);

  initDACs();
  Serial.println("init'd DACs");

  tlc.begin();
  if (tlc5947_oe >= 0) {
    pinMode(tlc5947_oe, OUTPUT);
    digitalWrite(tlc5947_oe, LOW);
  }
  Serial.println("began TLC driver");

  for (int i=0; i<25; i++) {
    setLEDPWM(i, 0); // sets all 24 outputs to no brightness
  }
  Serial.println("init'd LEDs");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5B, &Wire2)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("began MPR121");

  // init display
  u8g2.begin();
  Serial.println("began u8g2");

  // initialize encoders
  initEncoders();
  Serial.println("init'd uencoders");

  kpd.setHoldTime(150);

  delay(100);

  // Setup our clock system
  // Inits the clock
  uClock.init();
  // Set the callback function for the clock output to send MIDI Sync message.
  uClock.setClock96PPQNOutput(ClockOut96PPQN);
  // Set the callback function for the 16th step ticks
  uClock.setClock16PPQNOutput(ClockOut16PPQN);
  // Set the callback function for MIDI Start and Stop messages.
  uClock.setOnClockStartOutput(onClockStart);  
  uClock.setOnClockStopOutput(onClockStop);
  // Set the clock BPM to 120 BPM
  uClock.setTempo(120);
    // enable/disable shuffle
  uClock.setShuffle(false);
  // set a template for shuffle
  //uClock.setShuffleTemplate(shuffle_off_tempo);
  uClock.setShuffleTemplate(shuffle_templates[1]);
  Serial.println("init'd uClock");

  u8g2_prepare();
  initMain();
  Serial.println("init'd main screen");

  delay(100);

  // wipeProject();
  // Serial.println("wiped project! restart!");
  // drawErrorMessage("wiped project, restart!");
  // return;

  File projectListFile = SD.open("/project_list.txt", FILE_READ);
  if (!projectListFile.available()) {
    current_UI_mode = PROJECT_INITIALIZE;
    drawInitProject();
    Serial.println("handled project init");
  } else {
    projectListFile.close();
    // load project here
    loadLatestProject();
    //wipeProject();
    Serial.println("handled project load");
  }

  Serial.print("sizeof(_seq_heap.pattern)");
  Serial.println(sizeof(_seq_heap.pattern));

  Serial.print("sizeof(_pattern_mods_mem)");
  Serial.println(sizeof(_pattern_mods_mem));

  Serial.print("sizeof(_seq_external)");
  Serial.println(sizeof(_seq_external));

  // IMPORTANT: DO THIS AFTER SD IS INITIALIZED ABOVE
  // load short project mono samples into PSRAM
  loadRawSamplesFromSdCard();
  parseRootForWavSamples();
  Serial.println("prepared samples");
}

void logMetrics(void)
{
  if (LOG_METRICS_ENABLED && !(elapsed % 1000)) {
    Serial.print("Memory: ");
    Serial.print(AudioMemoryUsage());
    Serial.print(",");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
  }
}

void loop(void)
{
  analog.update();
  
  handleHeadphoneAdjustment();
  handleSwitchStates(false);
  handleKeyboardStates();
  handleEncoderStates();
  handleQueueActions();

  logMetrics();
}

// if swing enabled, pause every other 16th note for x amt of ppqn

bool proceed = false;
bool swingEnabled = true;
bool startSwingCounter = false;
int swingMaxPPQN = 3;
int swingCurrPPQN = 0;
long tNow;

// Internal clock handlers
void ClockOut96PPQN(uint32_t tick) {
  if (!project_initialized) {
    return;
  }

  // Send MIDI_CLOCK to external gears
  //usbMIDI.sendRealTime(usbMIDI.Clock);

  handle_bpm_step(tick);
}

void onClockStart() {
  if (!project_initialized) {
    return;
  }

  //usbMIDI.sendRealTime(usbMIDI.Start);
}

void onClockStop() {
  if (!project_initialized) {
    return;
  }

  //usbMIDI.sendRealTime(usbMIDI.Stop);

  noteOffForAllSounds();
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

std::string getNewProjectName()
{
  std::string newProjectName = "project_";
  
  newProjectName += std::to_string(year());

  if (month() < 10) {
    newProjectName += strldz(std::to_string(month()),2);
  } else {
    newProjectName += std::to_string(month());
  }

  if (day() < 10) {
    newProjectName += strldz(std::to_string(day()),2);
  } else {
    newProjectName += std::to_string(day());
  }

  if (hour() < 10) {
    newProjectName += strldz(std::to_string(hour()),2);
  } else {
    newProjectName += std::to_string(hour());
  }

  if (minute() < 10) {
    newProjectName += strldz(std::to_string(minute()),2);
  } else {
    newProjectName += std::to_string(minute());
  }

  if (second() < 10) {
    newProjectName += strldz(std::to_string(second()),2);
  } else {
    newProjectName += std::to_string(second());
  }

  return newProjectName;
}

void drawErrorMessage(std::string message)
{
  u8g2.clearBuffer();

  drawGenericOverlayFrame();

  u8g2.drawStr(37, 6, "ERROR!");
  u8g2.drawStr(10, 22, message.c_str());

  u8g2.sendBuffer();
}

TRACK getHeapTrack(int track)
{
  return _seq_heap.pattern.tracks[track];
}

TRACK_STEP getHeapStep(int track, int step)
{
  return _seq_heap.pattern.tracks[track].steps[step];
}

PATTERN getHeapCurrentSelectedPattern()
{
  return _seq_heap.pattern;
}

TRACK getHeapCurrentSelectedTrack(void)
{
  return _seq_heap.pattern.tracks[current_selected_track];
}

TRACK_STEP getHeapCurrentSelectedTrackStep(void)
{
  return _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];
}

void swapSequencerMemoryForPattern(int newBank, int newPattern)
{

//AudioNoInterrupts();
  PATTERN currPatternData = _seq_heap.pattern;
  PATTERN newPatternData = _seq_external.banks[newBank].patterns[newPattern];

  // save any mods for current pattern to SD
  savePatternModsToSdCard();

  // swap memory data
  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern] = currPatternData;
  _seq_heap.pattern = newPatternData;

  // initialize new pattern
  _seq_heap.pattern.initialized = true;

  // update currently selected vars
  current_selected_bank = newBank;
  current_selected_pattern = newPattern;

  // load any mods for new bank/pattern to SD
  loadPatternModsFromSdCard();

  // TODO: initTrackSounds() is causing the audio glitching when changing patterns due to applying audio object
  // changes for all 16 tracks at once.
  // Maybe have each track re-init their own sound and voice settings individually as a new noteOn is triggered?
  
  //initTrackSounds();
  //configureVoiceSettingsOnLoad();
//AudioInterrupts();

  // temp: trying this approach ^
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
    _trkNeedsInit[t] = true;
  }
}

PANNED_AMOUNTS getStereoPanValues(float pan)
{
  PANNED_AMOUNTS amounts;

  amounts.left = 1.0;
  if (pan < 0) {
    amounts.left += pan;
  }

  amounts.right = 1.0;
  if (pan > 0) {
    amounts.right -= pan;
  }

  return amounts;
}

std::string getTrackTypeNameStr(TRACK_TYPE type)
{
  std::string str = "MIDI"; // default

  switch (type)
  {
  case TRACK_TYPE::CV_GATE :
    str = "CV/GATE";

    break;
  
  case TRACK_TYPE::CV_TRIG :
    str = "CV/TRIG";
    
    break;
  
  case TRACK_TYPE::RAW_SAMPLE :
    str = "MONO SAMPLE";
    
    break;
  
  case TRACK_TYPE::WAV_SAMPLE :
    str = "STEREO SAMPLE";
    
    break;
  
  case TRACK_TYPE::SUBTRACTIVE_SYNTH :
    str = "2-OSC SUBTRACTIVE";
    
    break;
  
  case TRACK_TYPE::DEXED :
    str = "DEXED";
    
    break;
  
  default:
    break;
  }

  return str;
}

void noteOffForAllSounds(void)
{
  for (int t = 0; t < 4; t++)
  {
    TRACK currTrack = getHeapTrack(t);
  
    if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
      comboVoices[t].ampEnv.noteOff();
      comboVoices[t].filterEnv.noteOff();
    } else {
      comboVoices[t].ampEnv.noteOff();
    }
  }

  for (int s = 0; s < 12; s++)
  {
    sampleVoices[s].ampEnv.noteOff();
  }
}

void initSoundsForTrack(int t)
{
  AudioNoInterrupts();
  // TODO: eventually need to restore all sounds for all patterns and their tracks?
  TRACK currTrack = getHeapTrack(t);

  if (t < 4) {
    // init mono RAW sample
    comboVoices[t].rSample.setPlaybackRate(currTrack.sample_play_rate);
    comboVoices[t].rSample.enableInterpolation(true);

    // init dexed
    // comboVoices[t].dexed.loadInitVoice();
    comboVoices[t].dexed.loadVoiceParameters(fmpiano_sysex);

    // init synth
    comboVoices[t].osca.begin(currTrack.waveform);
    comboVoices[t].osca.amplitude(currTrack.oscalevel);
    comboVoices[t].osca.frequency(261.63); // C4 TODO: use find freq LUT with track note
    comboVoices[t].osca.pulseWidth(currTrack.width);
    comboVoices[t].oscb.begin(currTrack.waveform);
    comboVoices[t].oscb.amplitude(currTrack.oscblevel);
    comboVoices[t].oscb.frequency(261.63); // C3 TODO: use find freq LUT with track note + detune
    comboVoices[t].oscb.pulseWidth(currTrack.width);
    comboVoices[t].noise.amplitude(currTrack.noise);
    comboVoices[t].oscMix.gain(0, 0.33);
    comboVoices[t].oscMix.gain(1, 0.33);
    comboVoices[t].oscMix.gain(2, 0.33);
    comboVoices[t].dc.amplitude(currTrack.filterenvamt);
    comboVoices[t].lfilter.frequency(currTrack.cutoff);
    comboVoices[t].lfilter.resonance(currTrack.res);
    comboVoices[t].lfilter.octaveControl(4);
    comboVoices[t].filterEnv.attack(currTrack.filter_attack);
    comboVoices[t].filterEnv.decay(currTrack.filter_decay);
    comboVoices[t].filterEnv.sustain(currTrack.filter_sustain);
    comboVoices[t].filterEnv.release(currTrack.filter_release);
    comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

    // output
    comboVoices[t].mix.gain(0, 1); // raw sample
    comboVoices[t].mix.gain(1, 1); // synth

    // mono to L&R
    comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    comboVoices[t].leftSubMix.gain(1, currTrack.level); // wav sample left
    comboVoices[t].leftSubMix.gain(0, currTrack.level); // dexed left
    comboVoices[t].rightSubMix.gain(1, currTrack.level); // wav sample right
    comboVoices[t].rightSubMix.gain(0, currTrack.level); // dexed right
  } else {
    // init mono RAW sample
    int tOffset = t-4;

    sampleVoices[tOffset].rSample.setPlaybackRate(currTrack.sample_play_rate);
    sampleVoices[tOffset].rSample.enableInterpolation(true);

    sampleVoices[tOffset].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

    // mono to L&R
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    sampleVoices[tOffset].leftSubMix.gain(1, currTrack.level); // raw sample / synth left
    sampleVoices[tOffset].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
  }

  configureVoiceSettingsForTrack(t);

  AudioInterrupts();
  _trkNeedsInit[t] = false;
}

void initTrackSounds()
{
  // configure combo voice audio objects
  for (int v = 0; v < COMBO_VOICE_COUNT; v++) {    
    // TODO: eventually need to restore all sounds for all patterns and their tracks?
    TRACK currTrack = getHeapTrack(v);

    // init mono RAW sample
    comboVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
    comboVoices[v].rSample.enableInterpolation(true);

    // init dexed
    // comboVoices[v].dexed.loadInitVoice();
    comboVoices[v].dexed.loadVoiceParameters(fmpiano_sysex);

    // init synth
    comboVoices[v].osca.begin(currTrack.waveform);
    comboVoices[v].osca.amplitude(currTrack.oscalevel);
    comboVoices[v].osca.frequency(261.63); // C4 TODO: use find freq LUT with track note
    comboVoices[v].osca.pulseWidth(currTrack.width);
    comboVoices[v].oscb.begin(currTrack.waveform);
    comboVoices[v].oscb.amplitude(currTrack.oscblevel);
    comboVoices[v].oscb.frequency(261.63); // C3 TODO: use find freq LUT with track note + detune
    comboVoices[v].oscb.pulseWidth(currTrack.width);
    comboVoices[v].noise.amplitude(currTrack.noise);
    comboVoices[v].oscMix.gain(0, 0.33);
    comboVoices[v].oscMix.gain(1, 0.33);
    comboVoices[v].oscMix.gain(2, 0.33);
    comboVoices[v].dc.amplitude(currTrack.filterenvamt);
    comboVoices[v].lfilter.frequency(currTrack.cutoff);
    comboVoices[v].lfilter.resonance(currTrack.res);
    comboVoices[v].lfilter.octaveControl(4);
    comboVoices[v].filterEnv.attack(currTrack.filter_attack);
    comboVoices[v].filterEnv.decay(currTrack.filter_decay);
    comboVoices[v].filterEnv.sustain(currTrack.filter_sustain);
    comboVoices[v].filterEnv.release(currTrack.filter_release);
    comboVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

    // output
    comboVoices[v].mix.gain(0, 1); // raw sample
    comboVoices[v].mix.gain(1, 1); // synth

    // mono to L&R
    comboVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    comboVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    comboVoices[v].leftSubMix.gain(1, currTrack.level); // raw sample / synth left
    comboVoices[v].leftSubMix.gain(0, currTrack.level); // dexed left
    comboVoices[v].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
    comboVoices[v].rightSubMix.gain(0, currTrack.level); // dexed right
  }

  // configure sample voice audio objects
  for (int v = 0; v < SAMPLE_VOICE_COUNT; v++) {
    // TODO: eventually need to restore all sounds for all patterns and their tracks?
    TRACK currTrack = getHeapTrack(v+4); // offset by 4 since the 12 sample voices start at track 5

    // init mono RAW sample
    sampleVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
    sampleVoices[v].rSample.enableInterpolation(true);

    sampleVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
    //sampleVoices[v].ampEnv.releaseNoteOn(15);

    // mono to L&R
    sampleVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    sampleVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    sampleVoices[v].leftSubMix.gain(1, currTrack.level); // raw sample / synth left
    sampleVoices[v].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
  }

  mixerLeft1.gain(0, 1);
  mixerRight1.gain(0, 1);
  mixerLeft1.gain(1, 1);
  mixerRight1.gain(1, 1);
  mixerLeft1.gain(2, 1);
  mixerRight1.gain(2, 1);
  mixerLeft1.gain(3, 1);
  mixerRight1.gain(3, 1);

  mixerLeft2.gain(0, 1);
  mixerRight2.gain(0, 1);
  mixerLeft2.gain(1, 1);
  mixerRight2.gain(1, 1);
  mixerLeft2.gain(2, 1);
  mixerRight2.gain(2, 1);
  mixerLeft2.gain(3, 1);
  mixerRight2.gain(3, 1);

  mixerLeft3.gain(0, 1);
  mixerRight3.gain(0, 1);
  mixerLeft3.gain(1, 1);
  mixerRight3.gain(1, 1);
  mixerLeft3.gain(2, 1);
  mixerRight3.gain(2, 1);
  mixerLeft3.gain(3, 1);
  mixerRight3.gain(3, 1);

  mixerLeft4.gain(0, 1);
  mixerRight4.gain(0, 1);
  mixerLeft4.gain(1, 1);
  mixerRight4.gain(1, 1);
  mixerLeft4.gain(2, 1);
  mixerRight4.gain(2, 1);
  mixerLeft4.gain(3, 1);
  mixerRight4.gain(3, 1);

  // Main L&R output mixer
  mainMixerLeft.gain(0, 1);
  mainMixerRight.gain(0, 1);
  mainMixerLeft.gain(1, 1);
  mainMixerRight.gain(1, 1);
  mainMixerLeft.gain(2, 1);
  mainMixerRight.gain(2, 1);
  mainMixerLeft.gain(3, 1);
  mainMixerRight.gain(3, 1);

  // L&R input mixer
  inputMixerLeft.gain(0, 0.25);
  inputMixerRight.gain(0, 0.25);

  // Main L&R output mixer
  OutputMixerLeft.gain(0, 1);
  OutputMixerRight.gain(0, 1);
  OutputMixerLeft.gain(1, 1);
  OutputMixerRight.gain(1, 1);
}

void changeSampleTrackSoundType(uint8_t t, TRACK_TYPE newType)
{
  TRACK currTrack = getHeapTrack(t);
  TRACK_TYPE currType = currTrack.track_type;

  if (currType == newType) return;

  if (currType == WAV_SAMPLE) {
    // sampleVoices[t-4].wSample.disposeBuffer();
  }

  if (newType == RAW_SAMPLE) {
    _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;
  } else if (newType == DEXED) {
    _seq_heap.pattern.tracks[t].track_type = DEXED;
  } else if (newType == WAV_SAMPLE) {
    // only create buffers for stereo samples when needed
    // sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

    _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;
  } else if (newType == MIDI_OUT) {
    _seq_heap.pattern.tracks[t].track_type = MIDI_OUT;
  } else if (newType == CV_GATE) {
    _seq_heap.pattern.tracks[t].track_type = CV_GATE;
  } else if (newType == CV_TRIG) {
    _seq_heap.pattern.tracks[t].track_type = CV_TRIG;
  }
}

void configureSampleVoiceSettingsOnLoad(int t)
{
  TRACK track = getHeapTrack(t);

  if (track.track_type == WAV_SAMPLE) {
    // only create buffers for stereo samples when needed
    // sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

    _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;
  }
}

void configureVoiceSettingsForTrack(int t)
{
  if (t > 3) {
    SampleVoice trackVoice = sampleVoices[t-4];

    if (_seq_heap.pattern.tracks[t].track_type == WAV_SAMPLE) {
      // only create buffers for stereo samples when needed
      // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);
    }
    return;
  }

  ComboVoice trackVoice = comboVoices[t];

  if (_seq_heap.pattern.tracks[t].track_type == RAW_SAMPLE) {
    _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;

    // turn sample volume all the way up
    trackVoice.mix.gain(0, 1);
    // turn synth volume all the way down
    trackVoice.mix.gain(1, 0); // synth
  } else if (_seq_heap.pattern.tracks[t].track_type == DEXED) {
    _seq_heap.pattern.tracks[t].track_type = DEXED;

    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth

    trackVoice.leftSubMix.gain(0,0);
    trackVoice.leftSubMix.gain(1,1);
    trackVoice.rightSubMix.gain(0,0);
    trackVoice.rightSubMix.gain(1,1);
  } else if (_seq_heap.pattern.tracks[t].track_type == WAV_SAMPLE) {
    _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;

    // only create buffers for stereo samples when needed
    // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth
  } else if (_seq_heap.pattern.tracks[t].track_type == SUBTRACTIVE_SYNTH) {
    _seq_heap.pattern.tracks[t].track_type = SUBTRACTIVE_SYNTH;

    TRACK currTrack = getHeapTrack(t);

    // turn sample volume all the way down
    trackVoice.mix.gain(0, 0);
    // turn synth volumes all the way up
    trackVoice.mix.gain(1, 1); // ladder

    // TESTING: revert amp env to normal synth setting
    trackVoice.ampEnv.attack(currTrack.amp_attack);
    trackVoice.ampEnv.decay(currTrack.amp_decay);
    trackVoice.ampEnv.sustain(currTrack.amp_sustain);
    trackVoice.ampEnv.release(currTrack.amp_release);
  } else if (_seq_heap.pattern.tracks[t].track_type == MIDI_OUT) {
    _seq_heap.pattern.tracks[t].track_type = MIDI_OUT;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth
  } else if (_seq_heap.pattern.tracks[t].track_type == CV_GATE) {
    _seq_heap.pattern.tracks[t].track_type = CV_GATE;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth
  }else if (_seq_heap.pattern.tracks[t].track_type == CV_TRIG) {
    _seq_heap.pattern.tracks[t].track_type = CV_TRIG;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth
  }
}

void configureVoiceSettingsOnLoad(void)
{
  for (int t=0; t<MAXIMUM_SEQUENCER_TRACKS; t++) {
    if (t > 3) {
      configureSampleVoiceSettingsOnLoad(t);
      continue;
    }

    ComboVoice trackVoice = comboVoices[t];

    if (_seq_heap.pattern.tracks[t].track_type == RAW_SAMPLE) {
      _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;

      // turn sample volume all the way up
      trackVoice.mix.gain(0, 1);
      // turn synth volume all the way down
      trackVoice.mix.gain(1, 0); // synth
    } else if (_seq_heap.pattern.tracks[t].track_type == WAV_SAMPLE) {
      _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;

      // only create buffers for stereo samples when needed
      // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

      // turn sample volume all the way up
      trackVoice.mix.gain(0, 1);
      // turn synth volumes all the way down
      trackVoice.mix.gain(1, 0); // synth
    } else if (_seq_heap.pattern.tracks[t].track_type == DEXED) {
      _seq_heap.pattern.tracks[t].track_type = DEXED;

      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // synth

      trackVoice.leftSubMix.gain(0,0);
      trackVoice.leftSubMix.gain(1,1);
      trackVoice.rightSubMix.gain(0,0);
      trackVoice.rightSubMix.gain(1,1);
    }  else if (_seq_heap.pattern.tracks[t].track_type == SUBTRACTIVE_SYNTH) {
      _seq_heap.pattern.tracks[t].track_type = SUBTRACTIVE_SYNTH;

      TRACK currTrack = getHeapTrack(t);

      // turn sample volume all the way down
      trackVoice.mix.gain(0, 0);
      // turn synth volumes all the way up
      trackVoice.mix.gain(1, 1); // ladder

      // TESTING: revert amp env to normal synth setting
      trackVoice.ampEnv.attack(currTrack.amp_attack);
      trackVoice.ampEnv.decay(currTrack.amp_decay);
      trackVoice.ampEnv.sustain(currTrack.amp_sustain);
      trackVoice.ampEnv.release(currTrack.amp_release);
    } else if (_seq_heap.pattern.tracks[t].track_type == MIDI_OUT) {
      _seq_heap.pattern.tracks[t].track_type = MIDI_OUT;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // synth
    } else if (_seq_heap.pattern.tracks[t].track_type == CV_GATE) {
      _seq_heap.pattern.tracks[t].track_type = CV_GATE;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // synth
    }else if (_seq_heap.pattern.tracks[t].track_type == CV_TRIG) {
      _seq_heap.pattern.tracks[t].track_type = CV_TRIG;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // synth
    }
  }
}

void changeTrackSoundType(uint8_t t, TRACK_TYPE newType)
{
  if (t > 3) {
    changeSampleTrackSoundType(t, newType);
    return;
  }

  ComboVoice trackVoice = comboVoices[t];
  TRACK currTrack = getHeapTrack(t);
  TRACK_TYPE currType = currTrack.track_type;

  if (currType == newType) return;

  if (currType == WAV_SAMPLE) {
    // trackVoice.wSample.disposeBuffer();
  }

  if (newType == RAW_SAMPLE) {
   _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;

    // turn sample volume all the way up
    trackVoice.mix.gain(0, 1);
    // turn synth volume all the way down
    trackVoice.mix.gain(1, 0); // synth
  } else if (newType == DEXED) {
    _seq_heap.pattern.tracks[t].track_type = DEXED;

    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // synth

    trackVoice.leftSubMix.gain(0,0);
    trackVoice.leftSubMix.gain(1,1);
    trackVoice.rightSubMix.gain(0,0);
    trackVoice.rightSubMix.gain(1,1);
  } else if (newType == WAV_SAMPLE) {
   _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;

    // only create buffers for stereo samples when needed
    // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

    // turn sample volume all the way up
    trackVoice.mix.gain(0, 1);
    // turn synth volumes all the way down
    trackVoice.mix.gain(1, 0); // synth
  } else if (newType == SUBTRACTIVE_SYNTH) {
   _seq_heap.pattern.tracks[t].track_type = SUBTRACTIVE_SYNTH;

    TRACK currTrack = getHeapTrack(t);

    // turn sample volume all the way down
    trackVoice.mix.gain(0, 0);
    // turn synth volumes all the way up
    trackVoice.mix.gain(1, 1); // ladder

    // TESTING: revert amp env to normal synth setting
    trackVoice.ampEnv.attack(currTrack.amp_attack);
    trackVoice.ampEnv.decay(currTrack.amp_decay);
    trackVoice.ampEnv.sustain(currTrack.amp_sustain);
    trackVoice.ampEnv.release(currTrack.amp_release);
  } else if (newType == MIDI_OUT) {
   _seq_heap.pattern.tracks[t].track_type = MIDI_OUT;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  } else if (newType == CV_GATE) {
   _seq_heap.pattern.tracks[t].track_type = CV_GATE;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  }else if (newType == CV_TRIG) {
   _seq_heap.pattern.tracks[t].track_type = CV_TRIG;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  }
}

void initUsableSampleNames(void)
{
  // TODO: implement a proper project sample pool so that
  // sample IDs adhere to their respective sample names
  // when loading project data from the SD card
  for (int n=0; n<MAX_USABLE_SAMPLE_IDS; n++) {
    strcpy(usableSampleNames[n], "N/A");
  }
}

void initUsableWavSampleNames(void)
{
  // TODO: implement a proper project sample pool so that
  // sample IDs adhere to their respective sample names
  // when loading project data from the SD card
  for (int n=0; n<MAX_USABLE_SAMPLE_IDS; n++) {
    strcpy(usableWavSampleNames[n], "N/A");
  }
}

bool isWavFile(const char* filename) {
  int8_t len = strlen(filename);

  bool result = false;
  String fStr = filename;

  if (fStr.toLowerCase().substring((len - 4), len) == ".wav") {
    result = true;
  }

  return result;
}

void parseRootForWavSamples(void)
{
  File dir = SD.open("/");

  uint8_t currentFileIndex = 0;

  while(true) {
    File entry = dir.openNextFile();
    if (! entry) {
      Serial.println("** no files found **");
      break;
    }

    if (!entry.isDirectory()) {
      if (strlen(entry.name()) > MAX_SAMPLE_NAME_LENGTH) {
        Serial.print("Sample names can only be 32 characters long, not loading: ");
        Serial.println(entry.name());
        continue;
      }

      if ((currentFileIndex + 1) > MAX_USABLE_WAV_SAMPLE_IDS) {
        Serial.println("Reached the 512 WAV sample limit!");
        break;
      }

      if (isWavFile(entry.name())) {
        // files have sizes, directories do not
        strcpy(usableWavSampleNames[currentFileIndex], entry.name());

        ++wavSamplesAvailable;
        ++currentFileIndex;
      }
    }
    
    entry.close();
  }
}

bool isRawFile(const char* filename) {
  int8_t len = strlen(filename);

  bool result = false;
  String fStr = filename;

  if (fStr.toLowerCase().substring((len - 4), len) == ".raw") {
    result = true;
  }

  return result;
}

void parseRootForRawSamples(void)
{
  File dir = SD.open("/");

  uint32_t totalSizeLimit = 8000000; // 8 MB
  uint32_t totalSizeCollected = 0;
  uint8_t currentFileIndex = 0;

  while(true) {
    File entry = dir.openNextFile();
    if (! entry) {
      Serial.println("** no files found **");
      break;
    }

    if (!entry.isDirectory()) {
      if (strlen(entry.name()) > MAX_SAMPLE_NAME_LENGTH) {
        Serial.print("Sample names can only be 32 characters long, not loading: ");
        Serial.println(entry.name());
        continue;
      }

      if (isRawFile(entry.name())) {
        // files have sizes, directories do not
        totalSizeCollected += entry.size();
        if (totalSizeCollected >= totalSizeLimit) {
          Serial.println("size limit reached, not loading any more samples!");
          break;
        }

        if ((currentFileIndex + 1) > MAX_USABLE_SAMPLE_IDS) {
          Serial.println("Reached the 255 RAW sample limit!");
          break;
        }

        strcpy(usableSampleNames[currentFileIndex], entry.name());

        ++rawSamplesAvailable;
        ++currentFileIndex;
      }
    }
    
    entry.close();
  }
}

void savePatternModsToSdCard(void)
{
  std::string currentPatternModFilename = "/"; // e.g. /project_20230101000000_15_15_mods.bin
  currentPatternModFilename += current_project.name;
  currentPatternModFilename += "_";
  currentPatternModFilename += std::to_string(current_selected_bank);
  currentPatternModFilename += "_";
  currentPatternModFilename += std::to_string(current_selected_pattern);
  currentPatternModFilename += "_mods.bin";

  Serial.print("sizeof(_pattern_mods_mem): ");
  Serial.println(sizeof(_pattern_mods_mem));

  File currentPatternModsFile = SD.open(currentPatternModFilename.c_str(), FILE_WRITE);
  currentPatternModsFile.truncate();
  currentPatternModsFile.write((byte *)&_pattern_mods_mem, sizeof(_pattern_mods_mem));
  currentPatternModsFile.close();
  
  Serial.println("Saved pattern mods file for current bank and pattern!");
}

void loadPatternModsFromSdCard(void)
{
  std::string currentPatternModFilename = "/"; // e.g. /project_20230101000000_15_15_mods.bin
  currentPatternModFilename += current_project.name;
  currentPatternModFilename += "_";
  currentPatternModFilename += std::to_string(current_selected_bank);
  currentPatternModFilename += "_";
  currentPatternModFilename += std::to_string(current_selected_pattern);
  currentPatternModFilename += "_mods.bin";

  File currentPatternModsFile = SD.open(currentPatternModFilename.c_str(), FILE_READ);
  if (currentPatternModsFile.available()) {
    currentPatternModsFile.read((byte *)&_pattern_mods_mem, sizeof(_pattern_mods_mem));
    currentPatternModsFile.close();
  } else {
    Serial.println("No pattern mods file for current bank and pattern! Saving new mods for current pattern now!");
    savePatternModsToSdCard();
  }
}

void initExternalSequencer(void)
{
  for (int b = 0; b < MAXIMUM_SEQUENCER_BANKS; b++)
  {
    for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
    {
      _seq_external.banks[b].patterns[p].last_step = DEFAULT_LAST_STEP;
      _seq_external.banks[b].patterns[p].initialized = false;
      _seq_external.banks[b].patterns[p].groove_amount = 0;
      _seq_external.banks[b].patterns[p].groove_id = -1;

      if (p == 0) _seq_external.banks[b].patterns[p].initialized = true;

      for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
      {
        _seq_external.banks[b].patterns[p].tracks[t].track_type = RAW_SAMPLE;
        _seq_external.banks[b].patterns[p].tracks[t].raw_sample_id = 0;
        _seq_external.banks[b].patterns[p].tracks[t].wav_sample_id = 0;
        _seq_external.banks[b].patterns[p].tracks[t].waveform = WAVEFORM_TYPE::SAW;
        _seq_external.banks[b].patterns[p].tracks[t].last_step = DEFAULT_LAST_STEP;
        _seq_external.banks[b].patterns[p].tracks[t].length = 4;
        _seq_external.banks[b].patterns[p].tracks[t].note = 0;
        _seq_external.banks[b].patterns[p].tracks[t].octave = 4;
        _seq_external.banks[b].patterns[p].tracks[t].detune = 0;
        _seq_external.banks[b].patterns[p].tracks[t].fine = 0;
        _seq_external.banks[b].patterns[p].tracks[t].velocity = 50;
        _seq_external.banks[b].patterns[p].tracks[t].microtiming = 0;
        _seq_external.banks[b].patterns[p].tracks[t].probability = 100;
        _seq_external.banks[b].patterns[p].tracks[t].bitrate = 16;
        _seq_external.banks[b].patterns[p].tracks[t].channel = 1;
        _seq_external.banks[b].patterns[p].tracks[t].looptype = 0;
        _seq_external.banks[b].patterns[p].tracks[t].loopstart = 0;
        _seq_external.banks[b].patterns[p].tracks[t].loopfinish = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].playstart = play_start_sample;
        _seq_external.banks[b].patterns[p].tracks[t].level = 0.7;
        _seq_external.banks[b].patterns[p].tracks[t].pan = 0;
        _seq_external.banks[b].patterns[p].tracks[t].sample_play_rate = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].width = 0.5;
        _seq_external.banks[b].patterns[p].tracks[t].oscalevel = 1;
        _seq_external.banks[b].patterns[p].tracks[t].oscblevel = 0.5;
        _seq_external.banks[b].patterns[p].tracks[t].cutoff = 1600;
        _seq_external.banks[b].patterns[p].tracks[t].res = 0;
        _seq_external.banks[b].patterns[p].tracks[t].filterenvamt = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_attack = 0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_decay = 1000;
        _seq_external.banks[b].patterns[p].tracks[t].filter_sustain = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_release = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].amp_attack = 0;
        _seq_external.banks[b].patterns[p].tracks[t].amp_decay = 1000;
        _seq_external.banks[b].patterns[p].tracks[t].amp_sustain = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].amp_release = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].noise = 0;
        _seq_external.banks[b].patterns[p].tracks[t].chromatic_enabled = false;
        _seq_external.banks[b].patterns[p].tracks[t].muted = false;
        _seq_external.banks[b].patterns[p].tracks[t].soloing = false;
        _seq_external.banks[b].patterns[p].tracks[t].initialized = false;

        if (t == 0) _seq_external.banks[b].patterns[p].tracks[t].initialized = true;

        // now fill in steps
        for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        {
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].length = 4;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].note = 0;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].octave = 4;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].velocity = 50;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].microtiming = 0;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].probability = 100;

          _seq_external.banks[b].patterns[p].tracks[t].steps[s].state = TRACK_STEP_STATE::STATE_OFF;
        }
      }
    }
  }
}

void prepareSequencer(void)
{
  project_initialized = true;

  current_selected_pattern = 0;
  current_selected_track = 0;

  current_UI_mode = PATTERN_WRITE;

  drawSequencerScreen(false);
}

void saveProject()
{
  current_UI_mode = PROJECT_BUSY;

  int creatingBoxWidth = 50;
  int creatingBoxHeight = 20;
  int creatingBoxStartX = (DISPLAY_MAX_WIDTH / 2) - (creatingBoxWidth / 2);
  int creatingBoxStartY = (DISPLAY_MAX_HEIGHT / 2) - (creatingBoxHeight / 2);
  int creatingBoxMsgStartX = creatingBoxStartX+8;
  int creatingBoxMsgStartY = creatingBoxStartY+6;

  drawHatchedBackground();

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(creatingBoxStartX-3, creatingBoxStartY-3, creatingBoxWidth+6, creatingBoxHeight+6);
  u8g2.setColorIndex((u_int8_t)1);

  // show project creation status indicator?
  u8g2.drawFrame(creatingBoxStartX, creatingBoxStartY, creatingBoxWidth, creatingBoxHeight);
  u8g2.drawStr(creatingBoxMsgStartX, creatingBoxMsgStartY, "SAVING!");

  delay(100);

  std::string currProjectFilename = "/";
  currProjectFilename += current_project.name;
  currProjectFilename += ".txt";

  Serial.println("Write current project text file to SD card!");

  File currProjectFile = SD.open(currProjectFilename.c_str(), FILE_WRITE);
  currProjectFile.truncate();
  currProjectFile.write((byte *)&current_project, sizeof(current_project));
  currProjectFile.close();

  delay(100);

  // push current heap memory to RAM2
  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern] = _seq_heap.pattern;
  
  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  Serial.println("Write current project sequencer data binary file to SD card!");

  File seqFileW = SD.open(seqFilename.c_str(), FILE_WRITE);
  seqFileW.truncate();
  seqFileW.write((byte *)&_seq_external, sizeof(_seq_external));
  seqFileW.close();

  // TODO: can prob remove this, the current pattern in heap should reflect the current pattern in DMAMEM
  _seq_heap.pattern = _seq_external.banks[current_selected_bank].patterns[current_selected_pattern];

  savePatternModsToSdCard();

  Serial.println("done saving project!");
}

void loadLatestProject()
{
  File lastProjectFile = SD.open("/last_project.txt", FILE_READ);
  String lastProjectFilename = lastProjectFile.readString();
  lastProjectFile.close();

  File projectFile = SD.open(lastProjectFilename.c_str(), FILE_READ);  
  if (projectFile.available()) {    
    projectFile.read((byte *)&current_project, sizeof(current_project));
    projectFile.close();
  } else {
    drawErrorMessage("ERROR LOADING LAST PROJECT!");
    delay(500);

    current_UI_mode = PROJECT_INITIALIZE;
    drawInitProject();

    Serial.println("Could not load last project, initializing new one!");
  }

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  int sequencerFileSize = 0;
  File seqFile = SD.open(seqFilename.c_str(), FILE_READ);
  sequencerFileSize = seqFile.size();

  if (sequencerFileSize > 0) {
    uClock.setTempo(current_project.tempo);

    // copy sequencer data from SD card to RAM2
    seqFile.read((byte *)&_seq_external, sizeof(_seq_external));
    seqFile.close();
    delay(10);

    // copy first pattern from RAM2 to Heap
    _seq_heap.pattern = _seq_external.banks[0].patterns[0];
    delay(10);
    
    initTrackSounds();
    delay(10);

    configureVoiceSettingsOnLoad();
    delay(10);

    prepareSequencer();

    initExternalPatternMods();
    loadPatternModsFromSdCard();
  } else {
    drawErrorMessage("SEQUENCER DATA NOT FOUND!");
  }
}

void initProject()
{
  current_UI_mode = PROJECT_BUSY;

  int creatingBoxWidth = 50;
  int creatingBoxHeight = 20;
  int creatingBoxStartX = (DISPLAY_MAX_WIDTH / 2) - (creatingBoxWidth / 2);
  int creatingBoxStartY = (DISPLAY_MAX_HEIGHT / 2) - (creatingBoxHeight / 2);
  int creatingBoxMsgStartX = creatingBoxStartX+8;
  int creatingBoxMsgStartY = creatingBoxStartY+6;

  drawHatchedBackground();

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(creatingBoxStartX-3, creatingBoxStartY-3, creatingBoxWidth+6, creatingBoxHeight+6);
  u8g2.setColorIndex((u_int8_t)1);

  // show project creation status indicator?
  u8g2.drawFrame(creatingBoxStartX, creatingBoxStartY, creatingBoxWidth, creatingBoxHeight);
  u8g2.drawStr(creatingBoxMsgStartX, creatingBoxMsgStartY, "CREATING!");

  u8g2.sendBuffer();

  strcpy(current_project.name, new_project_name.c_str());

  File newProjectListFile = SD.open("/project_list.txt", FILE_WRITE);
  newProjectListFile.println(new_project_name.c_str());
  newProjectListFile.close();
  delay(50);

  std::string newProjectFilename = "/";
  newProjectFilename += new_project_name;
  newProjectFilename += ".txt";

  Serial.println("Saving new project text file to SD card!");

  File newProjectFile = SD.open(newProjectFilename.c_str(), FILE_WRITE);
  newProjectFile.write((byte *)&current_project, sizeof(current_project));
  newProjectFile.close();
  delay(50);

  // update last opened file
  File lastProjectFile = SD.open("/last_project.txt", FILE_WRITE);
  lastProjectFile.truncate();
  lastProjectFile.print(newProjectFilename.c_str());
  lastProjectFile.close();
  delay(50);

  initExternalSequencer();
  initExternalPatternMods();

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  Serial.println("Saving new project sequencer data binary file to SD card!");

  File seqFileW = SD.open(seqFilename.c_str(), FILE_WRITE);
  seqFileW.write((byte *)&_seq_external, sizeof(_seq_external));
  seqFileW.close();

  _seq_heap.pattern = _seq_external.banks[0].patterns[0];
  delay(50);

  configureVoiceSettingsOnLoad();
  prepareSequencer();
}

void initExternalPatternMods()
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    // now fill in mod flags
    for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
    {
      for (int f = 0; f < MAX_STEP_MOD_ATTRS; f++)
      {
        _pattern_mods_mem.tracks[t].step_mod_flags[s].flags[f] = false;
      }
    }

    // now fill in mods
    for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
    {
      _pattern_mods_mem.tracks[t].steps[s].raw_sample_id = 0;
      _pattern_mods_mem.tracks[t].steps[s].wav_sample_id = 0;
      _pattern_mods_mem.tracks[t].steps[s].waveform = WAVEFORM_TYPE::SAW;
      _pattern_mods_mem.tracks[t].steps[s].detune = 0;
      _pattern_mods_mem.tracks[t].steps[s].fine = 0;
      _pattern_mods_mem.tracks[t].steps[s].looptype = 0;
      _pattern_mods_mem.tracks[t].steps[s].loopstart = 0;
      _pattern_mods_mem.tracks[t].steps[s].loopfinish = 5000;
      _pattern_mods_mem.tracks[t].steps[s].playstart = play_start_sample;
      _pattern_mods_mem.tracks[t].steps[s].level = 0.7;
      _pattern_mods_mem.tracks[t].steps[s].pan = 0;
      _pattern_mods_mem.tracks[t].steps[s].sample_play_rate = 1.0;
      _pattern_mods_mem.tracks[t].steps[s].width = 0.5;
      _pattern_mods_mem.tracks[t].steps[s].oscalevel = 1.0;
      _pattern_mods_mem.tracks[t].steps[s].oscblevel = 0.5;
      _pattern_mods_mem.tracks[t].steps[s].cutoff = 1600;
      _pattern_mods_mem.tracks[t].steps[s].res = 0;
      _pattern_mods_mem.tracks[t].steps[s].filterenvamt = 1.0;
      _pattern_mods_mem.tracks[t].steps[s].filter_attack = 0;
      _pattern_mods_mem.tracks[t].steps[s].filter_decay = 1000;
      _pattern_mods_mem.tracks[t].steps[s].filter_sustain = 1.0;
      _pattern_mods_mem.tracks[t].steps[s].filter_release = 5000;
      _pattern_mods_mem.tracks[t].steps[s].amp_attack = 0;
      _pattern_mods_mem.tracks[t].steps[s].amp_decay = 1000;
      _pattern_mods_mem.tracks[t].steps[s].amp_sustain = 1.0;
      _pattern_mods_mem.tracks[t].steps[s].amp_release = 5000;
      _pattern_mods_mem.tracks[t].steps[s].noise = 0;
    }
  }
}

void drawInitProject()
{
  u8g2.clearBuffer();

  drawGenericOverlayFrame();

  std::string createProjStr = "CREATE PROJECT";
  u8g2.drawStr(37, 6, createProjStr.c_str());

  std::string nameFieldStr = "NAME: ";

  nameFieldStr += new_project_name;

  u8g2.drawStr(10, 22, nameFieldStr.c_str());

  u8g2.drawFrame(106, 49, 17, 11);
  u8g2.drawStr(109, 51, "SEL");
  u8g2.drawStr(10, 52, "CONFIRM?");

  u8g2.sendBuffer();
}

void loadRawSamplesFromSdCard(void)
{  
  parseRootForRawSamples();

  // newdigate::flashloader loader;

  char naChar[32];
  strcpy(naChar, "N/A");
  
  for (int s = 0; s < MAX_PROJECT_RAW_SAMPLES; s++) {
    if (usableSampleNames[s] != naChar) {
      samples[s] = loader.loadSample(usableSampleNames[s]);
    }
  }
}

void wipeProject(void)
{
  Serial.println("wiping known project data!");

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  SD.remove(seqFilename.c_str()); 

  String projFilename = "/";
  projFilename += current_project.name;
  projFilename += ".txt";

  SD.remove(projFilename.c_str());

  SD.remove("/last_project.txt");
  SD.remove("/project_list.txt");
}

void handleQueueActions(void)
{
  if (draw_queue_blink > -1) {
    if (draw_queue_blink == 1) {
      drawSequencerScreen(true);
    } else if (draw_queue_blink == 0) {
      drawSequencerScreen(false);
    }
  }

  if (dequeue_pattern) {
    dequeue_pattern = false;

    loader.clearSamples();

    swapSequencerMemoryForPattern(_queued_pattern.bank, _queued_pattern.number);
    
    // reset queue flags
    _queued_pattern.bank = -1;
    _queued_pattern.number = -1;
    draw_queue_blink = -1;

    current_step_page = 1;
    current_page_selected = 1;
    if (current_UI_mode == PATTERN_WRITE) {
      clearAllStepLEDs();
    } else if (current_UI_mode == TRACK_WRITE) {
      displayPageLEDs(1); // TODO need?
    }

    drawSequencerScreen(false);
  }
}

void initMain()
{
  u8g2.clearBuffer();

  int boxWidth = 60;
  int boxHeight = 30;
  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (boxWidth / 2), (DISPLAY_MAX_HEIGHT / 2) - (boxHeight / 2) - 16, boxWidth, boxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.setFont(bitocra13_c); // u8g2_font_8x13_mr
  u8g2.drawStr(52, 8, "xr-1");

  int fBoxWidth = 60;
  int fBoxHeight = 10;
  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (fBoxWidth / 2), 28, fBoxWidth, fBoxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();
  u8g2.drawStr(38, 29, "audio enjoyer");

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (36 / 2), 45, 36, fBoxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();
  std::string versionStr = "v";
  versionStr += FIRMWARE_VERSION;
  u8g2.drawStr(52, 47, versionStr.c_str());

  u8g2.sendBuffer();

  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();
}

std::string strldz(std::string inputStr, const int zeroNum)
{
  std::string outputStr;

  unsigned int number_of_zeros = zeroNum - inputStr.length();

  outputStr.insert(0, number_of_zeros, '0');
  outputStr.append(inputStr);

  return outputStr;
}

void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum)
{
  if (!hideNum) inputStr += strldz(std::to_string(value), 2);

  u8g2.drawStr(startX, 0, inputStr.c_str());
}

std::string getTrackMetaStr(TRACK_TYPE type)
{
  std::string outputStr;

  switch (type)
  {
  case SUBTRACTIVE_SYNTH:
    outputStr = "SYNTH:";
    break;
  
  case RAW_SAMPLE:
    outputStr = "RSAMPLE:";
    break;
  
  case WAV_SAMPLE:
    outputStr = "WSAMPLE:";
    break;

  case DEXED:
    outputStr = "DEXED:";
    break;
  
  case MIDI_OUT:
    outputStr = "MIDI";
    break;
  
  case CV_GATE:
    outputStr = "CV/GATE";
    break;
  
  case CV_TRIG:
    outputStr = "CV/TRIG";
    break;
  
  default:
    break;
  }

  return outputStr;
}

std::string getWaveformName(uint8_t waveform)
{
  std::string outputStr;

  switch (waveform)
  {
  case WAVEFORM_SAWTOOTH:
    outputStr = "SAW";
    break;
  
  case WAVEFORM_SAWTOOTH_REVERSE:
    outputStr = "RSAW";
    break;
  
  case WAVEFORM_TRIANGLE:
    outputStr = "TRI";
    break;
  
  case WAVEFORM_SQUARE:
    outputStr = "SQR";
    break;
  
  case WAVEFORM_PULSE:
    outputStr = "PUL";
    break;
  
  case WAVEFORM_SINE:
    outputStr = "SIN";
    break;
  
  default:
    break;
  }

  return outputStr;
}

std::string getDisplayNote(void)
{
  std::string outputStr;

  if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
    outputStr += baseNoteToStr[_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].note];
    outputStr += std::to_string(_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].octave);
  } else {
    TRACK currTrack = getHeapCurrentSelectedTrack();
    outputStr += baseNoteToStr[currTrack.note];
    outputStr += std::to_string(currTrack.octave);
  }

  return outputStr;
}

std::string getPercentageStr(float rate)
{
  std::string outputStr;

  outputStr += std::to_string((float)round(rate * 100) / 100);
  outputStr += "%";
  
  return outputStr;
}

std::string getPlaybackSpeedStr(float rate)
{
  std::string rateStr = std::to_string((float)round(rate * 100) / 100);

  int strLen = (int)rate >= 10 ? 4 : 3;
  
  std::string outputStr = rateStr.substr(0,strLen);

  bool t = rate < 0.1;
  if (t) {
    outputStr = "REV";
  }
  
  return outputStr;
}

std::string getCurrPageNameForTrack(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  
  std::string outputStr = trackCurrPageNameMap[currTrack.track_type][current_page_selected];

  return outputStr;
}

std::string getLoopTypeName(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  std::string outputStr;

  uint8_t looptypeToUse = currTrack.looptype;

  if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
    looptypeToUse = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].looptype;
  }

  if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_none) {
    outputStr += "OFF";
  } else if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_repeat) {
    if (currTrack.chromatic_enabled) {
      outputStr += "CHR";
    } else {
    outputStr += "REP";
    }
  }

  return outputStr;
}

SOUND_CONTROL_MODS getSubtractiveSynthControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LSTP";
    mods.bName = "LEN";
    mods.cName = "VELO";
    mods.dName = "PROB";

    mods.aValue = std::to_string(track.last_step);

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      mods.bValue = std::to_string(_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].length);
    } else {
      mods.bValue = std::to_string(track.length);
    }

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      mods.cValue = std::to_string(_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].velocity);
    } else {
      mods.cValue = std::to_string(track.velocity);
    }

    mods.dValue = "100%"; // TODO: impl
    break;
  
  case 1: // OSC
    mods.aName = "WAVE";
    mods.bName = "DET";
    mods.cName = "FINE";
    mods.dName = "WID";

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      mods.aValue = getWaveformName(_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].waveform);
    } else {
      mods.aValue = getWaveformName(track.waveform);
    }
    //mods.aValue = getWaveformName(track.waveform);
    
    mods.bValue = std::to_string(track.detune);
    mods.cValue = std::to_string(track.fine);

    mods.dValue = std::to_string((float)round(track.width * 100) / 100);
    mods.dValue = mods.dValue.substr(0,4);
    break;
  
  case 2: // FILTER
    mods.aName = "NOIS";
    mods.bName = "FREQ";
    mods.cName = "RESO";
    mods.dName = "AMT";

    mods.aValue = std::to_string((float)round(track.noise * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string(round(track.cutoff));
    mods.bValue = mods.bValue.substr(0,5);

    mods.cValue = std::to_string((float)round(track.res * 100) / 100);
    mods.cValue = mods.cValue.substr(0,4);

    //mods.cValue = std::to_string(track.res);

    mods.dValue = std::to_string((float)round(track.filterenvamt * 100) / 100);
    mods.dValue = mods.dValue.substr(0,4);
    break;
  
  case 3: // FILTER ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.filter_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.filter_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,5);

    mods.cValue = std::to_string((float)round(track.filter_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.filter_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,5);
    break;
  
  case 4: // AMP ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.amp_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.amp_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,5);

    mods.cValue = std::to_string((float)round(track.amp_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.amp_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,5);
    break;
  
  case 5: // OUTPUT
    mods.aName = "LVL";
    mods.bName = "PAN";
    mods.cName = "--";
    mods.dName = "--"; // fx send?

    mods.aValue = std::to_string(round(track.level * 100));

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);
    mods.bFloatValue = track.pan;
    mods.bType = RANGE;

    mods.cValue = "--";
    mods.dValue = "--";

    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getRawSampleControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "L.STEP";
    mods.bName = "FILE";
    mods.cName = "SPEED";
    mods.dName = "--";

    mods.aValue = std::to_string(track.last_step);
    mods.bValue = std::to_string(track.raw_sample_id+1);

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      mods.cValue = getPlaybackSpeedStr(_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].sample_play_rate);
    } else {
      mods.cValue = getPlaybackSpeedStr(track.sample_play_rate);
    }

    mods.dValue = "--";
    break;
  
  case 1: // LOOPING
    {
      mods.aName = "TYPE";
      mods.bName = "START";
      mods.cName = "FINISH";
      mods.dName = "PLAYST";

      mods.aValue = getLoopTypeName();

      uint32_t loopstartToUse = track.loopstart;

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        loopstartToUse = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopstart;
      }

      std::string lsStr = std::to_string(loopstartToUse);      
      lsStr += "ms";

      mods.bValue = lsStr;

      uint32_t loopfinishToUse = track.loopfinish;

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        loopfinishToUse = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopfinish;
      }

      std::string lfStr = std::to_string(loopfinishToUse);      
      lfStr += "ms";

      mods.cValue = lfStr;

      play_start playstartToUse = track.playstart;

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        playstartToUse = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].playstart;
      }

      mods.dValue = playstartToUse == play_start::play_start_loop ? "LOOP" : "SAMPLE";
      break;
    }
  case 2: // AMP ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.amp_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.amp_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = std::to_string((float)round(track.amp_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.amp_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,3);

    break;
  
  case 3: // OUTPUT
    mods.aName = "LEVEL";
    mods.bName = "PAN";
    mods.cName = "--";
    mods.dName = "--"; // fx send?

    mods.aValue = std::to_string(round(track.level * 100));
    //mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);
    mods.bFloatValue = track.pan;
    mods.bType = RANGE;

    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getWavSampleControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "L.STEP";
    mods.bName = "FILE";
    mods.cName = "POS";
    mods.dName = "--";

    mods.aValue = std::to_string(track.last_step);
    mods.bValue = std::to_string(track.wav_sample_id+1);
    mods.cValue = "0ms";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getDexedControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LSTP";
    mods.bName = "LEN";
    mods.cName = "--";
    mods.dName = "--";

    mods.aValue = std::to_string(track.last_step); // TODO : impl
    mods.bValue = std::to_string(track.length); // TODO: impl
    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getMidiControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LSTP";
    mods.bName = "LEN";
    mods.cName = "CHAN";
    mods.dName = "VELO";

    mods.aValue = std::to_string(track.last_step); // TODO : impl
    mods.bValue = std::to_string(track.length); // TODO: impl
    mods.cValue = std::to_string(track.channel); // TODO: impl
    mods.dValue = std::to_string(track.velocity); // TODO: impl
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getCvGateControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
  {
    mods.aName = "LSTP";
    mods.bName = "LEN";
    mods.cName = "OUT";
    mods.dName = "PROB";

    mods.aValue = std::to_string(track.last_step);
    mods.bValue = std::to_string(track.length);

    std::string outputChanStr = std::to_string(track.channel);
    outputChanStr += "AB";

    mods.cValue = outputChanStr;
    mods.dValue = "100%";
  }
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getCvTrigControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LSTP";
    mods.bName = "OUT";
    mods.cName = "PROB";
    mods.dName = "--";

    mods.aValue = std::to_string(track.last_step);
    mods.bValue = "1AB"; // TODO: impl
    mods.cValue = "100%"; // TODO: impl
    mods.dValue = "--"; // TODO: impl
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getControlModDataForTrack()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();
  
  switch (track.track_type)
  {
  case SUBTRACTIVE_SYNTH:
    mods = getSubtractiveSynthControlModData();
    break;
  
  case RAW_SAMPLE:
    mods = getRawSampleControlModData();
    break;
  
  case WAV_SAMPLE:
    mods = getWavSampleControlModData();
    break;

  case DEXED:
    mods = getDexedControlModData();
    break;
  
  case MIDI_OUT:
    mods = getMidiControlModData();
    break;
  
  case CV_GATE:
    mods = getCvGateControlModData();
    break;
  
  case CV_TRIG:
    mods = getCvTrigControlModData();
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getControlModDataForPattern()
{
  SOUND_CONTROL_MODS mods;

  PATTERN pattern = getHeapCurrentSelectedPattern();

  mods.aName = "L.STEP";
  mods.bName = "GROOVE";
  mods.cName = "GR.AMT";
  mods.dName = "--";

  mods.aValue = std::to_string(pattern.last_step);
  mods.bValue = pattern.groove_id > -1 ? _grooves.configs[pattern.groove_id].name : "OFF";
  mods.cValue = pattern.groove_id > -1 ? shuffle_name[pattern.groove_amount] : "--";
  mods.dValue = "--";

  return mods;
}

void drawCopySelOverlay(std::string type)
{
  std::string copyStr = "SELECT ";
  copyStr += type;
  copyStr += " TO COPY";

  int centerX = (128 / 2);
  int centerY = (64 / 2);

  int boxWidth = 100;
  int boxHeight = 20;

  int boxStartX = centerX-(boxWidth/2);
  int boxStartY = centerY-(boxHeight/2);

  int msgStartX = (centerX-(boxWidth/2))+6;
  int msgStartY = (centerY-(boxHeight/2))+6;

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.drawStr(msgStartX, msgStartY, copyStr.c_str());

  u8g2.sendBuffer();
}

void drawCopyConfirmOverlay(std::string type, uint8_t num)
{
  std::string copyStr = "CHOOSE WHERE TO PASTE";

  int centerX = (128 / 2);
  int centerY = (64 / 2);

  int boxWidth = 100;
  int boxHeight = 30;

  int boxStartX = centerX-(boxWidth/2);
  int boxStartY = centerY-(boxHeight/2);

  int msgStartX = (centerX-(boxWidth/2))+6;
  int msgStartY = (centerY-(boxHeight/2))+6;

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.drawStr(msgStartX, msgStartY, copyStr.c_str());

  std::string suffixStr = type;
  suffixStr += " ";
  suffixStr += std::to_string(num);

  u8g2.drawStr(msgStartX, msgStartY+10, suffixStr.c_str());

  u8g2.sendBuffer();
}

void drawPasteConfirmOverlay(std::string type, uint8_t num)
{
  std::string pasteStr;
  pasteStr += type;
  pasteStr += " PASTED TO ";

  int centerX = (128 / 2);
  int centerY = (64 / 2);

  int boxWidth = 100;
  int boxHeight = 30;

  int boxStartX = centerX-(boxWidth/2);
  int boxStartY = centerY-(boxHeight/2);

  int msgStartX = (centerX-(boxWidth/2))+6;
  int msgStartY = (centerY-(boxHeight/2))+6;

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
  u8g2.drawStr(msgStartX, msgStartY, pasteStr.c_str());

  std::string suffixStr = type;
  suffixStr += " ";
  suffixStr += std::to_string(num);

  u8g2.drawStr(msgStartX, msgStartY+10, suffixStr.c_str());

  u8g2.sendBuffer();
}

void drawControlModsForADSR(int att, int dec, float sus, int rel);
void drawControlModsForADSR(int att, int dec, float sus, int rel)
{
  int ctrlModHeaderStartX = 1;
  int adsrMaxTopPosY = 24;
  int adsrMaxBottomPosY = 48;
  int adsrMaxHeight = adsrMaxBottomPosY - adsrMaxTopPosY;

  // attack
  int attackStartPosX = ctrlModHeaderStartX;
  int attackEndRawPosX = ctrlModHeaderStartX + (int) (0.025 * att);
  int attackEndPosX = attackEndRawPosX;

  u8g2.drawLine(attackStartPosX, adsrMaxBottomPosY, attackEndPosX, adsrMaxTopPosY);
  
  // decay and sustain
  int sustainStartRawPosY = adsrMaxBottomPosY - (int) (31 * sus);
  int sustainStartPosY = max(adsrMaxTopPosY, sustainStartRawPosY);

  int decayEndRawPosX = attackEndPosX + (int) (0.05 * dec);
  int decayEndPosX = decayEndRawPosX; // min(attackEndPosX+50, decayEndRawPosX);

  int sustainEndPosX = decayEndPosX+20; // static x Pos

  u8g2.drawLine(attackEndPosX, adsrMaxTopPosY, decayEndPosX, sustainStartPosY);
  u8g2.drawLine(decayEndPosX, sustainStartPosY, sustainEndPosX, sustainStartPosY);
  
  // release
  int releaseEndRawPosX = sustainEndPosX + (int) (0.01 * rel);
  int releaseEndPosX = releaseEndRawPosX;
  
  u8g2.drawLine(sustainEndPosX, sustainStartPosY, releaseEndPosX, adsrMaxBottomPosY);
}

void drawExtendedControlMods(void);
void drawExtendedControlMods(void)
{
  int ctrlModHeaderY = 20;
  int ctrlModHeaderStartX = 0;
  int ctrlModHeaderStartCenteredX = ((DISPLAY_MAX_WIDTH / 4) / 2);
  int ctrlModSpaceWidth = (DISPLAY_MAX_WIDTH / 4);

  // header
  u8g2.drawLine(ctrlModHeaderStartX,30,128,30);
  // dividers
  u8g2.drawLine(ctrlModSpaceWidth, 20, ctrlModSpaceWidth, 52);
  u8g2.drawLine(ctrlModSpaceWidth+(ctrlModSpaceWidth*1), 20, ctrlModSpaceWidth+(ctrlModSpaceWidth*1), 52);
  u8g2.drawLine(ctrlModSpaceWidth+(ctrlModSpaceWidth*2), 20, ctrlModSpaceWidth+(ctrlModSpaceWidth*2), 52);

  SOUND_CONTROL_MODS mods = getControlModDataForTrack();

  int aPosX = ctrlModHeaderStartCenteredX;
  aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);
    
  int bPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*1);
  bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);
    
  int cPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*2);
  cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);
  
  int dPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*3);
  dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

  u8g2.drawStr(aPosX, ctrlModHeaderY+1, mods.aName.c_str());
  u8g2.drawStr(bPosX, ctrlModHeaderY+1, mods.bName.c_str());
  u8g2.drawStr(cPosX, ctrlModHeaderY+1, mods.cName.c_str());
  u8g2.drawStr(dPosX, ctrlModHeaderY+1, mods.dName.c_str());
  
  int aValuePos = ctrlModHeaderStartCenteredX;
  aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

  int bValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*1);
  bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

  int cValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*2);
  cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

  int dValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*3);
  dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);
  
  u8g2.drawStr(aValuePos, ctrlModHeaderY+17, mods.aValue.c_str());

  if (mods.bType == RANGE) {
    int centerLineStartX = ctrlModHeaderStartX+(ctrlModSpaceWidth*1);
    int rangeMiddleX = (centerLineStartX+(ctrlModSpaceWidth/2));
    u8g2.drawVLine(centerLineStartX+6, ctrlModHeaderY+16, 9); // left range bound
    u8g2.drawHLine(centerLineStartX+6, ctrlModHeaderY+20, ctrlModSpaceWidth-12); // range width
    u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY+16, 9); // range middle
    u8g2.drawVLine(centerLineStartX+ctrlModSpaceWidth-6, ctrlModHeaderY+16, 9); // right range bound

    // range pos cursor
    int rangePosI = rangeMiddleX + (int)(mods.bFloatValue * 10);
    u8g2.drawFilledEllipse(rangePosI, ctrlModHeaderY+20, 1, 2);
  } else {
    u8g2.drawStr(bValuePos, ctrlModHeaderY+17, mods.bValue.c_str());
  }

  u8g2.drawStr(cValuePos, ctrlModHeaderY+17, mods.cValue.c_str());
  u8g2.drawStr(dValuePos, ctrlModHeaderY+17, mods.dValue.c_str());
}

void drawNormalControlMods(void);
void drawNormalControlMods(void)
{
  int ctrlModHeaderY = 20;
  int ctrlModHeaderStartX = 29;
  int ctrlModHeaderStartCenteredX = (((DISPLAY_MAX_WIDTH - 29) / 4) / 2) + 29;
  int ctrlModSpacerMult = 25;

  // header
  u8g2.drawLine(ctrlModHeaderStartX, 30, 128, 30);
  // dividers
  u8g2.drawLine(ctrlModHeaderStartX, 20, ctrlModHeaderStartX, 52);
  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*1), 20, ctrlModHeaderStartX+(ctrlModSpacerMult*1), 52);
  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*2), 20, ctrlModHeaderStartX+(ctrlModSpacerMult*2), 52);
  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*3), 20, ctrlModHeaderStartX+(ctrlModSpacerMult*3), 52);

  SOUND_CONTROL_MODS mods = getControlModDataForTrack();

  int aPosX = ctrlModHeaderStartCenteredX;
  aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);
    
  int bPosX = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*1);
  bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);
    
  int cPosX = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*2);
  cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);
  
  int dPosX = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*3);
  dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

  u8g2.drawStr(aPosX, ctrlModHeaderY+1, mods.aName.c_str());
  u8g2.drawStr(bPosX, ctrlModHeaderY+1, mods.bName.c_str());
  u8g2.drawStr(cPosX, ctrlModHeaderY+1, mods.cName.c_str());
  u8g2.drawStr(dPosX, ctrlModHeaderY+1, mods.dName.c_str());
  
  int aValuePos = ctrlModHeaderStartCenteredX;
  aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

  int bValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*1);
  bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

  int cValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*2);
  cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

  int dValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpacerMult*3);
  dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);
  
  u8g2.drawStr(aValuePos, ctrlModHeaderY+17, mods.aValue.c_str());

  if (mods.bType == RANGE) {
    int centerLineStartX = ctrlModHeaderStartX+(ctrlModSpacerMult*1);
    int rangeMiddleX = (centerLineStartX+(ctrlModSpacerMult/2));
    u8g2.drawVLine(centerLineStartX+3, ctrlModHeaderY+16, 9); // left range bound
    u8g2.drawHLine(centerLineStartX+3, ctrlModHeaderY+20, ctrlModSpacerMult-6); // range width
    u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY+16, 9); // range middle
    u8g2.drawVLine(centerLineStartX+ctrlModSpacerMult-3, ctrlModHeaderY+16, 9); // right range bound

    // range pos cursor
    int rangePosI = rangeMiddleX + (int)(mods.bFloatValue * 10);
    u8g2.drawFilledEllipse(rangePosI, ctrlModHeaderY+20, 1, 2);
  } else {
    u8g2.drawStr(bValuePos, ctrlModHeaderY+17, mods.bValue.c_str());
  }

  u8g2.drawStr(cValuePos, ctrlModHeaderY+17, mods.cValue.c_str());
  u8g2.drawStr(dValuePos, ctrlModHeaderY+17, mods.dValue.c_str());
}

void drawControlMods(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (
    (currTrack.track_type == RAW_SAMPLE && current_page_selected == 2) ||
    (currTrack.track_type == SUBTRACTIVE_SYNTH && current_page_selected == 4)
  ) {
    drawControlModsForADSR(currTrack.amp_attack, currTrack.amp_decay, currTrack.amp_sustain, currTrack.amp_release);
  } else if (currTrack.track_type == SUBTRACTIVE_SYNTH && current_page_selected == 3) {
    drawControlModsForADSR(currTrack.filter_attack, currTrack.filter_decay, currTrack.filter_sustain, currTrack.filter_release);
  } else if (
    (currTrack.track_type == CV_TRIG) ||
    (currTrack.track_type == WAV_SAMPLE) ||
    (currTrack.track_type == RAW_SAMPLE && current_page_selected == 0) ||
    (currTrack.track_type == RAW_SAMPLE && current_page_selected == 1) ||
    (currTrack.track_type == RAW_SAMPLE && current_page_selected == 3)
  ) {
    drawExtendedControlMods();
  } else {
    drawNormalControlMods();
  }
}

void drawPatternControlMods(void)
{
  int ctrlModHeaderY = 20;
  int ctrlModHeaderBoxSize = 9;
  int ctrlModHeaderStartX = 0;
  int ctrlModSpaceWidth = (DISPLAY_MAX_WIDTH / 4);
  int ctrlModHeaderStartCenteredX = (DISPLAY_MAX_WIDTH / 4) / 2;

  u8g2.drawLine(ctrlModHeaderStartX, 30, 128, 30);

  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpaceWidth*1), 20, ctrlModHeaderStartX+(ctrlModSpaceWidth*1), 52);
  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpaceWidth*2), 20, ctrlModHeaderStartX+(ctrlModSpaceWidth*2), 52);
  u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpaceWidth*3), 20, ctrlModHeaderStartX+(ctrlModSpaceWidth*3), 52);

  SOUND_CONTROL_MODS mods = getControlModDataForPattern();

  int aPosX = ctrlModHeaderStartCenteredX;
  aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);
    
  int bPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*1);
  bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);
    
  int cPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*2);
  cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);
  
  int dPosX = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*3);
  dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

  u8g2.drawStr(aPosX, ctrlModHeaderY+1, mods.aName.c_str());
  u8g2.drawStr(bPosX, ctrlModHeaderY+1, mods.bName.c_str());
  u8g2.drawStr(cPosX, ctrlModHeaderY+1, mods.cName.c_str());
  u8g2.drawStr(dPosX, ctrlModHeaderY+1, mods.dName.c_str());

  int aValuePos = ctrlModHeaderStartCenteredX;
  aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);
    
  int bValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*1);
  bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);
    
  int cValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*2);
  cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);
  
  int dValuePos = ctrlModHeaderStartCenteredX+(ctrlModSpaceWidth*3);
  dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);
  
  u8g2.drawStr(aValuePos, ctrlModHeaderY+17, mods.aValue.c_str());
  u8g2.drawStr(bValuePos, ctrlModHeaderY+17, mods.bValue.c_str());
  u8g2.drawStr(cValuePos, ctrlModHeaderY+17, mods.cValue.c_str());
  u8g2.drawStr(dValuePos, ctrlModHeaderY+17, mods.dValue.c_str());
}

void drawPageNumIndicators(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  
  int pageNumBasedStartX = 81 - (3 * trackPageNumMap[currTrack.track_type]);
  int pageTabPosY = 56;
  int pageTabFooterNameStartX = 17;

  u8g2.drawLine(0,52,128,52);
  u8g2.drawStr(0,pageTabPosY, getCurrPageNameForTrack().c_str());

  if (trackPageNumMap[currTrack.track_type] == 1) return;

  if (trackPageNumMap[currTrack.track_type] == 5) {
    pageTabFooterNameStartX -= 10;
  } else if (trackPageNumMap[currTrack.track_type] == 4) {
    pageTabFooterNameStartX -= 1;
  }

  int pageBoxStartX = (pageNumBasedStartX + 30) - (trackPageNumMap[currTrack.track_type] * 5);
  pageBoxStartX += 8 / (trackPageNumMap[currTrack.track_type]);

  int pageNumStartX = pageBoxStartX + 3;
  int pageBetweenPaddingtX = 10;

  u8g2.drawBox(pageBoxStartX+(pageBetweenPaddingtX*current_page_selected),55,9,9);

  for (int l = 0; l < trackPageNumMap[currTrack.track_type]; l++)
  {
    if (l == current_page_selected) {
      u8g2.setColorIndex((u_int8_t)0);
      u8g2.drawStr(pageNumStartX+(pageBetweenPaddingtX*l),pageTabPosY, std::to_string(l+1).c_str());
      u8g2.setColorIndex((u_int8_t)1);
    } else {
      u8g2.drawStr(pageNumStartX+(pageBetweenPaddingtX*l),pageTabPosY, std::to_string(l+1).c_str());
    }
  }
}

void drawSequencerScreen(bool queueBlink)
{
  u8g2.clearBuffer();

  std::string bpmStr = strldz(std::to_string((uint8_t)uClock.getTempo()), 3);
  u8g2.drawStr(104, 0, bpmStr.c_str());

  // transport indicator (play, pause, stop)
  uint8_t transportIconStartX = 119;
  if (_seq_state.playback_state == STOPPED) {
    u8g2.drawBox(transportIconStartX,1,5,5);
  } else if (_seq_state.playback_state == RUNNING) {
    u8g2.drawTriangle(transportIconStartX,0,transportIconStartX,6,transportIconStartX+6,3);
  } else if (_seq_state.playback_state == PAUSED) {
    u8g2.drawBox(transportIconStartX,1,2,5);
    u8g2.drawBox(transportIconStartX+3,1,2,5);
  }

  if (current_UI_mode == PERFORM_TAP) {
    u8g2.drawStr(0, 0, "TAP MODE");
    u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

    u8g2.drawStr(41, 15, "TAP A TRACK");

    // middle trk icon
    u8g2.drawFrame(49, 27, 30, 28);
    u8g2.drawFrame(54, 31, 20, 19);
    u8g2.drawFrame(60, 34, 8, 3);


    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_MUTE) {
    u8g2.drawStr(0, 0, "MUTE MODE");
    u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);
    
    u8g2.drawStr(23, 15, "SELECT TRACKS TO MUTE");

    // left trk icon
    u8g2.drawLine(20, 27, 39, 27);
    u8g2.drawLine(39, 27, 39, 55);
    u8g2.drawLine(39, 55, 20, 55);
    u8g2.drawLine(20, 31, 34, 31);
    u8g2.drawLine(34, 31, 34, 50);
    u8g2.drawLine(34, 50, 20, 50);
    u8g2.drawBox(20, 34, 8, 3);

    // middle trk icon
    u8g2.drawFrame(49, 27, 30, 28);
    u8g2.drawFrame(54, 31, 20, 19);
    u8g2.drawBox(60, 34, 8, 3);

    // right trk icon
    u8g2.drawLine(107, 27, 88, 27);
    u8g2.drawLine(88, 27, 88, 55);
    u8g2.drawLine(88, 55, 107, 55);
    u8g2.drawLine(107, 31, 93, 31);
    u8g2.drawLine(93, 31, 93, 50);
    u8g2.drawLine(93, 50, 107, 50);
    u8g2.drawBox(100, 34, 8, 3);

    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_SOLO) {
    u8g2.drawStr(0, 0, "SOLO MODE");
    u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

    u8g2.drawStr(23, 15, "SELECT TRACK TO SOLO");

    // left trk icon
    u8g2.drawLine(20, 27, 39, 27);
    u8g2.drawLine(39, 27, 39, 55);
    u8g2.drawLine(39, 55, 20, 55);
    u8g2.drawLine(20, 31, 34, 31);
    u8g2.drawLine(34, 31, 34, 50);
    u8g2.drawLine(34, 50, 20, 50);

    u8g2.drawLine(20, 34, 28, 34);
    u8g2.drawLine(28, 34, 28, 37);
    u8g2.drawLine(28, 37, 20, 37);

    // middle trk icon
    u8g2.drawFrame(49, 27, 30, 28);
    u8g2.drawFrame(54, 31, 20, 19);
    u8g2.drawBox(60, 34, 8, 3);

    // right trk icon
    u8g2.drawLine(107, 27, 88, 27);
    u8g2.drawLine(88, 27, 88, 55);
    u8g2.drawLine(88, 55, 107, 55);
    u8g2.drawLine(107, 31, 93, 31);
    u8g2.drawLine(93, 31, 93, 50);
    u8g2.drawLine(93, 50, 107, 50);

    u8g2.drawLine(107, 34, 100, 34);
    u8g2.drawLine(100, 34, 100, 37);
    u8g2.drawLine(100, 37, 107, 37);

    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_RATCHET) {
    u8g2.drawStr(0, 0, "RATCHET MODE");
    u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

    if (ratcheting_track > -1) {
      std::string rchTrkStr = "TRK:";
      rchTrkStr += strldz(std::to_string(ratcheting_track+1), 2);
      u8g2.drawStr(0, 12, rchTrkStr.c_str());
    } else if (ratcheting_track == -1) {
      u8g2.drawStr(3, 12, "HOLD A TRACK BUTTON TO RATCHET");
    }

    // draw keyboard
    u8g2.drawFrame(10, 28, 107, 24);
    u8g2.drawLine(25, 40, 25, 51);
    u8g2.drawFrame(19, 28, 13, 12);
    u8g2.drawLine(37, 40, 37, 51);
    u8g2.drawLine(51, 28, 51, 51);
    u8g2.drawFrame(31, 28, 12, 12);
    u8g2.drawLine(65, 40, 65, 51);
    u8g2.drawFrame(59, 28, 13, 12);
    u8g2.drawLine(77, 40, 77, 51);
    u8g2.drawFrame(71, 28, 13, 12);
    u8g2.drawLine(89, 40, 89, 51);
    u8g2.drawFrame(83, 28, 13, 12);
    u8g2.drawLine(104, 28, 104, 51);

    if (ratchet_division > 0) {
      // ratchet_division = 24 = 96/24 = 4 = 1/4
      uint8_t divDenominator = (96/ratchet_division);

      std::string divStr = "1/";
      divStr += std::to_string(divDenominator);

      int xMult = 14;
      int xMultB = 13;

      // even divisions (bottom of keyboard)
      if (ratchet_division == 24) { // 1/4
        u8g2.drawTriangle(14, 60, 17, 55, 20, 60);
        u8g2.drawStr(26, 54, divStr.c_str());
      } else if (ratchet_division == 12) { // 1/8
        u8g2.drawTriangle(14 + (xMult * 1), 60, 17 + (xMult * 1), 55, 20 + (xMult * 1), 60);
        u8g2.drawStr(26 + (xMult * 1), 54, divStr.c_str());
      } else if (ratchet_division == 6) { // 1/16
        u8g2.drawTriangle(14 + (xMult * 2), 60, 17 + (xMult * 2), 55, 20 + (xMult * 2), 60);
        u8g2.drawStr(26 + (xMult * 2), 54, divStr.c_str());
      } else if (ratchet_division == 3) { // 1/32
        u8g2.drawTriangle(14 + (xMult * 3), 60, 17 + (xMult * 3), 55, 20 + (xMult * 3), 60);
        u8g2.drawStr(26 + (xMult * 3), 54, divStr.c_str());
      } else if (ratchet_division == 2) { // 1/48
        u8g2.drawTriangle(14 + (xMult * 4), 60, 17 + (xMult * 4), 55, 20 + (xMult * 4), 60);
        u8g2.drawStr(26 + (xMult * 4), 54, divStr.c_str());
      } else if (ratchet_division == 1) { // 1/96
        u8g2.drawTriangle(14 + (xMult * 5), 60, 17 + (xMult * 5), 55, 20 + (xMult * 5), 60);
        u8g2.drawStr(26 + (xMult * 5), 54, divStr.c_str());
      }
      // odd divisions (top of keyboard)
      else if (ratchet_division == 16) { // 1/6
        u8g2.drawTriangle(22, 21, 24, 25, 27, 21);
        u8g2.drawStr(33, 19, divStr.c_str());
      } else if (ratchet_division == 8) { // 1/12
        u8g2.drawTriangle(22 + (xMultB * 1), 21, 24 + (xMultB * 1), 25, 27 + (xMultB * 1), 21);
        u8g2.drawStr(33 + (xMultB * 1), 19, divStr.c_str());
      } else if (ratchet_division == 4) { // 1/24
        u8g2.drawTriangle(22 + (xMultB * 3), 21, 24 + (xMultB * 3), 25, 27 + (xMultB * 3), 21);
        u8g2.drawStr(33 + (xMultB * 3), 19, divStr.c_str());
      }
    } else {
      u8g2.drawStr(10, 54, "HOLD A RATCHET DIVISION KEY");
    }

    u8g2.sendBuffer();
    return;
  }

  bool bnkBlink = false;
  int bnkNumber = current_selected_bank+1;
  if (_queued_pattern.bank > -1) {
    bnkNumber = _queued_pattern.bank+1;
    bnkBlink = (_queued_pattern.bank != current_selected_bank);
  }

  drawMenuHeader("BNK:", bnkNumber, 0, (queueBlink && bnkBlink));

  bool ptnBlink = false;
  int ptnNumber = current_selected_pattern+1;
  if (_queued_pattern.number > -1) {
    ptnNumber = _queued_pattern.number+1;
    ptnBlink = (_queued_pattern.number != current_selected_pattern);
  }

  if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(26,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("PTN:", ptnNumber, 29, (queueBlink && ptnBlink));
    u8g2.setColorIndex((u_int8_t)1);
  } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    drawMenuHeader("PTN:", ptnNumber, 29, (queueBlink && ptnBlink));
  }

  if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(55,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("TRK:", current_selected_track+1, 58, false);
    u8g2.setColorIndex((u_int8_t)1);
  } 
  // else {
  //   drawMenuHeader("TRK:", current_selected_track+1, 58, false);
  // }

  if (current_UI_mode == PATTERN_WRITE) {
    //PATTERN currPattern = getHeapCurrentSelectedPattern();

    // draw pattern header box
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(0,9,128,9);
    u8g2.setColorIndex((u_int8_t)0);
    std::string patternHeaderStr = "PATTERN EDIT";
    u8g2.drawStr(2, 10, patternHeaderStr.c_str());
    u8g2.setColorIndex((u_int8_t)1);

    // draw control mod area
    drawPatternControlMods();

    u8g2.drawLine(0, 52, 128, 52);
    u8g2.drawStr(0, 56, "MAIN");

  } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    TRACK currTrack = getHeapCurrentSelectedTrack();
    TRACK_TYPE currTrackType = currTrack.track_type;

    // draw track meta type box
    int trackMetaStrX = 2;
    if (currTrackType == SUBTRACTIVE_SYNTH || currTrackType == MIDI_OUT) {
      trackMetaStrX = 4;
    }
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(0,9,128,9);
    u8g2.setColorIndex((u_int8_t)0);
    std::string trackMetaStr = getTrackMetaStr(currTrackType);

    std::string trackInfoStr;
    if (currTrackType == SUBTRACTIVE_SYNTH) {
      trackInfoStr += getTrackTypeNameStr(currTrackType);
    } else if (currTrackType == RAW_SAMPLE) {
      trackInfoStr += usableSampleNames[currTrack.raw_sample_id];
    } else if (currTrackType == RAW_SAMPLE || currTrackType == WAV_SAMPLE) {
      trackInfoStr += usableWavSampleNames[currTrack.wav_sample_id];
    } else if (currTrackType == MIDI_OUT) {
      trackInfoStr += "";
    } else if (currTrackType == CV_GATE || currTrackType == CV_TRIG) {
      trackInfoStr += "";
    }

    std::string combStr = trackMetaStr + " " + trackInfoStr;
    u8g2.drawStr(trackMetaStrX, 10, combStr.c_str());
    u8g2.setColorIndex((u_int8_t)1);

    // draw track description / main icon area
    if (currTrackType == SUBTRACTIVE_SYNTH || currTrackType == DEXED || currTrackType == MIDI_OUT || currTrackType == CV_GATE) {
      if (
        (currTrackType != SUBTRACTIVE_SYNTH) ||
        ((currTrack.track_type == SUBTRACTIVE_SYNTH && current_page_selected != 3) &&
        (currTrack.track_type == SUBTRACTIVE_SYNTH && current_page_selected != 4))
      ) {
        u8g2.drawStr(6, 23, "NOTE");
        u8g2.setFont(bitocra13_c);
        u8g2.drawStr(8, 32, getDisplayNote().c_str());
        u8g2.setFont(bitocra7_c);
      }
    } else if (currTrackType == CV_TRIG) {
      // u8g2.drawLine(3,42,14,42);
      // u8g2.drawLine(14,42,14,28);
      // u8g2.drawLine(14,28,19,28);
      // u8g2.drawLine(19,28,19,42);
      // u8g2.drawLine(19,42,24,42);
    }

    // draw control mod area
    drawControlMods();

    drawPageNumIndicators();
  }

  u8g2.sendBuffer();
}

void drawGenericOverlayFrame(void)
{
  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(2,2,125,62);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.drawFrame(2,2,125,62);
  u8g2.drawLine(2,16,125,16);
}

void drawSetTempoOverlay(void)
{
  drawGenericOverlayFrame();

  std::string tempoStr = "SET TEMPO";
  u8g2.drawStr(44, 6, tempoStr.c_str());
  u8g2.setFont(bitocra13_c);

  int tempoVal = (int)uClock.getTempo();
  std::string tempoValStr = std::to_string(tempoVal);
  int tempoValX = 52;

  u8g2.drawStr(tempoValX + (tempoVal > 99 ? 0 : 4), 31, tempoValStr.c_str());
  u8g2.setFont(small_font);

  u8g2.sendBuffer();
}

void drawSetupScreen()
{
  drawGenericOverlayFrame();

  std::string setupStr = "SAVE PROJECT";
  u8g2.drawStr(37, 6, setupStr.c_str());

  std::string nameFieldStr = "NAME: ";
  nameFieldStr += current_project.name;
  u8g2.drawStr(10, 22, nameFieldStr.c_str());

  u8g2.drawFrame(86, 49, 17, 11);
  u8g2.drawStr(89, 51, "SEL");
  u8g2.drawFrame(106, 49, 17, 11);
  u8g2.drawStr(109, 51, "ESC");
  u8g2.drawStr(10, 52, "CONFIRM?");

  u8g2.sendBuffer();
}

void triggerCvGateNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

  for(int i = 0; i < 128; i++) {
    cvLevels[i] = i * 26;
  }

  uint8_t noteToUse = note;
  uint8_t octaveToUse = keyboardOctave; // +1 ?

  int midiNote = (noteToUse + (12 * (octaveToUse))); // C0 = 12

  if (currTrack.channel == 1) {
    writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS1, 1, 4095); // gate
  } else if (currTrack.channel == 2)  {
    writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS2, 1, 4095); // gate
  } else if (currTrack.channel == 3) {
    writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS3, 1, 4095); // gate
  } else if (currTrack.channel == 4)  {
    writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS4, 1, 4095); // gate
  }
}

void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

  AudioNoInterrupts();
  float foundBaseFreq = noteToFreqArr[note];
  float octaveFreqA = (foundBaseFreq + (currTrack.fine * 0.01)) * (pow(2, keyboardOctave));
  float octaveFreqB = (foundBaseFreq * pow(2.0, (float)currTrack.detune/12.0)) * (pow(2, keyboardOctave));

  comboVoices[t].osca.frequency(octaveFreqA);
  comboVoices[t].oscb.frequency(octaveFreqB);

  comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
  comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

  comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  comboVoices[t].filterEnv.attack(currTrack.filter_attack);
  comboVoices[t].filterEnv.decay(currTrack.filter_decay);
  comboVoices[t].filterEnv.sustain(currTrack.filter_sustain);
  comboVoices[t].filterEnv.release(currTrack.filter_release);
  AudioInterrupts();

  // now triggers env
  comboVoices[t].ampEnv.noteOn();
  comboVoices[t].filterEnv.noteOn();
}

void triggerRawSampleNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];
  int tOffset = t-4;

  if (t < 4) {
  AudioNoInterrupts();
    comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  AudioInterrupts();

    comboVoices[t].ampEnv.noteOn();
    comboVoices[t].rSample.playRaw(samples[currTrack.raw_sample_id]->sampledata, samples[currTrack.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    comboVoices[t].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

    if (loopTypeSelMap[currTrack.looptype] == looptype_none) {
      comboVoices[t].rSample.setPlayStart(play_start::play_start_sample);
      comboVoices[t].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat) {
      float loopFinishToUse = currTrack.loopfinish;

      if (currTrack.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      comboVoices[t].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      comboVoices[t].rSample.setLoopStart(currTrack.loopstart);
      comboVoices[t].rSample.setLoopFinish(loopFinishToUse);
    }
  } else {
  AudioNoInterrupts();
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

    sampleVoices[tOffset].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  AudioInterrupts();

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].rSample.playRaw(samples[currTrack.raw_sample_id]->sampledata, samples[currTrack.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

    if (loopTypeSelMap[currTrack.looptype] == looptype_none) {
      sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat) {
      float loopFinishToUse = currTrack.loopfinish;

      if (currTrack.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      sampleVoices[tOffset].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopStart(currTrack.loopstart);
      sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
    }

  }
}

void triggerWavSampleNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];
  int tOffset = t-4;

  if (t < 4) {
    comboVoices[t].ampEnv.noteOn();
    // comboVoices[t].wSample.play(usableWavSampleNames[currTrack.wav_sample_id], sd1);
  } else {
    sampleVoices[tOffset].ampEnv.noteOn();
    // sampleVoices[tOffset].wSample.play(usableWavSampleNames[currTrack.wav_sample_id], sd1);
  }
}

void triggerDexedNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

  if (t < 4) {
    int midiNote = (note + (12 * (keyboardOctave)));

    comboVoices[t].dexed.keydown(midiNote, 50);
  }
}

void triggerTrackManually(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

  if (currTrack.track_type == RAW_SAMPLE) {
    triggerRawSampleNoteOn(t, note);
  } else if (currTrack.track_type == WAV_SAMPLE) {
    triggerWavSampleNoteOn(t, note);
  } else if (currTrack.track_type == DEXED) {
    Serial.print("dexed note: ");
    Serial.println(note);
    triggerDexedNoteOn(t, note);
  } else if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    triggerSubtractiveSynthNoteOn(t, note);
  } else if (currTrack.track_type == CV_GATE) {
    triggerCvGateNoteOn(t, note);
  }
}

void triggerRatchetingTrack(uint32_t tick)
{
  if (ratcheting_track == -1) {
    return;
  }

  handleRemoveFromRatchetStack();

  if (ratchet_division > -1 && !(tick % ratchet_division)) {  
    Serial.print("in ratchet division! tick: ");
    Serial.print(tick);
    Serial.print(" ratchet_division: ");
    Serial.println(ratchet_division);
      
    handleAddToRatchetStack();
  }
}

void triggerAllStepsForAllTracks(uint32_t tick)
{
  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
    int8_t currTrackStep = _seq_state.current_track_steps[t].current_step - 1; // get zero-based track step number

    TRACK currTrack = currentPattern.tracks[t];
    TRACK_STEP currTrackStepData = currTrack.steps[currTrackStep];

    if (!currTrack.muted && ((currTrackStepData.state == TRACK_STEP_STATE::STATE_ON) || (currTrackStepData.state == TRACK_STEP_STATE::STATE_ACCENTED))) {
      handleAddToStepStack(tick, t, currTrackStep);
    }
  }
}

void handleRawSampleNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  if (track > 3) {
    int tOffset = track-4;
  AudioNoInterrupts();
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

    sampleVoices[tOffset].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
  AudioInterrupts();

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

    if (loopTypeSelMap[trackToUse.looptype] == looptype_none) {
      sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat) {
      float loopFinishToUse = trackToUse.loopfinish;

      // if (trackToUse.chromatic_enabled) {
      //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
      //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
      //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
      //   uint32_t numSamples = 44100 / octaveFreq;

      //   loopFinishToUse = numSamples;
      // }

      sampleVoices[tOffset].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopStart(trackToUse.loopstart);
      sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
    }
  } else {
  AudioNoInterrupts();
    comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
    comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

    comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
    comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
    comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
    comboVoices[track].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
  AudioInterrupts();

    comboVoices[track].ampEnv.noteOn();
    comboVoices[track].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    comboVoices[track].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

    if (loopTypeSelMap[trackToUse.looptype] == looptype_none) {
      comboVoices[track].rSample.setPlayStart(play_start::play_start_sample);
      comboVoices[track].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat) {
      float loopFinishToUse = trackToUse.loopfinish;

      // if (trackToUse.chromatic_enabled) {
      //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
      //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
      //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
      //   uint32_t numSamples = 44100 / octaveFreq;

      //   loopFinishToUse = numSamples;
      // }

      comboVoices[track].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      comboVoices[track].rSample.setLoopStart(trackToUse.loopstart);
      comboVoices[track].rSample.setLoopFinish(loopFinishToUse);
    }
  }
}

void handleWavSampleNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  if (track > 3) {
    int tOffset = track-4;

    sampleVoices[tOffset].ampEnv.noteOn();
    // sampleVoices[tOffset].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  } else {
    comboVoices[track].ampEnv.noteOn();
    // comboVoices[track].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  }
}

void handleDexedNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  if (track < 4) {
    // comboVoices[track].dexed.setSustain(true);
    comboVoices[track].dexed.keydown(50, 50);
  }
}

void handleSubtractiveSynthNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  AudioNoInterrupts();
  float foundBaseFreq = noteToFreqArr[trackToUse.note];
  float octaveFreqA = (foundBaseFreq + (trackToUse.fine * 0.01)) * (pow(2, trackToUse.octave));
  float octaveFreqB = (foundBaseFreq * pow(2.0, (float)trackToUse.detune/12.0)) * (pow(2, trackToUse.octave));

  comboVoices[track].osca.frequency(octaveFreqA);
  comboVoices[track].oscb.frequency(octaveFreqB);

  comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
  comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

  comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
  comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
  comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
  comboVoices[track].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
  comboVoices[track].filterEnv.attack(trackToUse.filter_attack);
  comboVoices[track].filterEnv.decay(trackToUse.filter_decay);
  comboVoices[track].filterEnv.sustain(trackToUse.filter_sustain);
  comboVoices[track].filterEnv.release(trackToUse.filter_release);
  AudioInterrupts();

  // now triggers envs
  comboVoices[track].ampEnv.noteOn();
  comboVoices[track].filterEnv.noteOn();
}

void handleMIDINoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  MIDI.sendNoteOn(64, 100, 1);
}

void handleCvGateNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  Serial.print("cvLevel: ");
  Serial.println("cvLevel: ");

  uint8_t noteToUse = stepToUse.note;
  uint8_t octaveToUse = stepToUse.octave;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::NOTE]) {
    noteToUse = _pattern_mods_mem.tracks[track].steps[step].note;
    //Serial.println(noteToUse);
  }

  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::OCTAVE]) {
    octaveToUse = _pattern_mods_mem.tracks[track].steps[step].octave;
    //Serial.println(noteToUse);
  }

  for(int i = 0; i < 128; i++) {
    cvLevels[i] = i * 26;
  }

  int midiNote = (noteToUse + (12 * (octaveToUse)));

  Serial.print("midiNote: ");
  Serial.print(midiNote);
  Serial.print(" cvLevels[midiNote]: ");
  Serial.println(cvLevels[midiNote]);

  if (trackToUse.channel == 1) {
    writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS1, 1, 4095); // gate
  } else if (trackToUse.channel == 2)  {
    writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS2, 1, 4095); // gate
  } else if (trackToUse.channel == 3) {
    writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS3, 1, 4095); // gate
  } else if (trackToUse.channel == 4)  {
    writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS4, 1, 4095); // gate
  }
}

void handleCvGateNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  for(int i = 0; i < 128; i++) {
    cvLevels[i] = i * 26;
  }

  uint8_t noteToUse = trackToUse.note;
  uint8_t octaveToUse = trackToUse.octave;

  int midiNote = (noteToUse + (12 * (octaveToUse))); // use offset of 32 instead?

  if (trackToUse.channel == 1) {
    writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS1, 1, 4095); // gate
  } else if (trackToUse.channel == 2)  {
    writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS2, 1, 4095); // gate
  } else if(trackToUse.channel == 3) {
    writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS3, 1, 4095); // gate
  } else if (trackToUse.channel == 4)  {
    writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
    writeToDAC(CS4, 1, 4095); // gate
  }
}

void handleRawSampleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  uint8_t noteToUse = stepToUse.note;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::NOTE]) {
    noteToUse = _pattern_mods_mem.tracks[track].steps[step].note;
  }

  // uint8_t octaveToUse = stepToUse.octave;
  // if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[4]) {
  //   octaveToUse = _pattern_mods_mem.tracks[track].steps[step].octave;
  // }

  uint8_t velocityToUse = trackToUse.velocity;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::VELOCITY]) {
    velocityToUse = _pattern_mods_mem.tracks[track].steps[step].velocity;
  } else {
    velocityToUse = stepToUse.velocity;
  }

  uint8_t looptypeToUse = trackToUse.looptype;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LOOPTYPE]) {
    looptypeToUse = _pattern_mods_mem.tracks[track].steps[step].looptype;
  }

  uint32_t loopstartToUse = trackToUse.loopstart;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LOOPSTART]) {
    loopstartToUse = _pattern_mods_mem.tracks[track].steps[step].loopstart;
  }

  uint32_t loopfinishToUse = trackToUse.loopfinish;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LOOPFINISH]) {
    loopfinishToUse = _pattern_mods_mem.tracks[track].steps[step].loopfinish;
  }

  uint8_t playstartToUse = trackToUse.playstart;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::PLAYSTART]) {
    playstartToUse = _pattern_mods_mem.tracks[track].steps[step].playstart;
  }

  float speedToUse = trackToUse.sample_play_rate;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::SAMPLE_PLAY_RATE]) {
    speedToUse = _pattern_mods_mem.tracks[track].steps[step].sample_play_rate;
  }

  if (track > 3) {
    int tOffset = track-4;
  AudioNoInterrupts();
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

    sampleVoices[tOffset].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
    sampleVoices[tOffset].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
    sampleVoices[tOffset].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));

    sampleVoices[tOffset].rSample.setPlaybackRate(speedToUse);
  AudioInterrupts();

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[looptypeToUse]);

    if (loopTypeSelMap[looptypeToUse] == looptype_none) {
      sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[looptypeToUse] == looptype_repeat) {

      // if (trackToUse.chromatic_enabled) {
      //   float foundBaseFreq = noteToFreqArr[noteToUse];
      //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
      //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
      //   uint32_t numSamples = 44100 / octaveFreq;

      //   loopFinishToUse = numSamples;
      // }

      sampleVoices[tOffset].rSample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopStart(loopstartToUse);
      sampleVoices[tOffset].rSample.setLoopFinish(loopfinishToUse);
    }
  } else {
  AudioNoInterrupts();
    comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
    comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

    comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
    comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
    comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
    comboVoices[track].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));

    comboVoices[track].rSample.setPlaybackRate(speedToUse);
  AudioInterrupts();

    comboVoices[track].ampEnv.noteOn();
    comboVoices[track].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    comboVoices[track].rSample.setLoopType(loopTypeSelMap[looptypeToUse]);

    if (loopTypeSelMap[looptypeToUse] == looptype_none) {
      comboVoices[track].rSample.setPlayStart(play_start::play_start_sample);
      comboVoices[track].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[looptypeToUse] == looptype_repeat) {

      // if (trackToUse.chromatic_enabled) {
      //   float foundBaseFreq = noteToFreqArr[noteToUse];
      //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
      //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
      //   uint32_t numSamples = 44100 / octaveFreq;

      //   loopFinishToUse = numSamples;
      // }

      comboVoices[track].rSample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      comboVoices[track].rSample.setLoopStart(loopstartToUse);
      comboVoices[track].rSample.setLoopFinish(loopfinishToUse);
    }
  }
}

void handleWavSampleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);

  if (track > 3) {
    int tOffset = track-4;

    sampleVoices[tOffset].ampEnv.noteOn();
    // sampleVoices[tOffset].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  } else {
    comboVoices[track].ampEnv.noteOn();
    // comboVoices[track].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  }
}

void handleDexedNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  uint8_t noteToUse = stepToUse.note;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::NOTE]) {
    noteToUse = _pattern_mods_mem.tracks[track].steps[step].note;
    //Serial.println(noteToUse);
  }

  uint8_t octaveToUse = stepToUse.octave;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::OCTAVE]) {
    octaveToUse = _pattern_mods_mem.tracks[track].steps[step].octave;
    //Serial.println(octaveToUse);
  }

  if (track < 4) {
    int midiNote = (noteToUse + (12 * (octaveToUse)));

    comboVoices[track].dexed.keydown(midiNote, stepToUse.velocity);
  }
}

void handleSubtractiveSynthNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  uint8_t noteToUse = stepToUse.note;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::NOTE]) {
    noteToUse = _pattern_mods_mem.tracks[track].steps[step].note;
    //Serial.println(noteToUse);
  }

  uint8_t octaveToUse = stepToUse.octave;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::OCTAVE]) {
    octaveToUse = _pattern_mods_mem.tracks[track].steps[step].octave;
    //Serial.println(octaveToUse);
  }
  
  uint8_t velocityToUse = trackToUse.velocity;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::VELOCITY]) {
    velocityToUse = _pattern_mods_mem.tracks[track].steps[step].velocity;
  } else {
    velocityToUse = stepToUse.velocity;
  }

  uint8_t waveformToUse = trackToUse.waveform;
  if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::WAVEFORM]) {
    waveformToUse = _pattern_mods_mem.tracks[track].steps[step].waveform;
  }

  AudioNoInterrupts();
  float foundBaseFreq = noteToFreqArr[noteToUse];
  float octaveFreqA = (foundBaseFreq + (trackToUse.fine * 0.01)) * (pow(2, octaveToUse));
  float octaveFreqB = (foundBaseFreq * pow(2.0, (float)trackToUse.detune/12.0)) * (pow(2, octaveToUse));

  comboVoices[track].osca.begin(waveformToUse);
  comboVoices[track].oscb.begin(waveformToUse);
  
  comboVoices[track].osca.frequency(octaveFreqA);
  comboVoices[track].oscb.frequency(octaveFreqB);

  comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
  comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

  comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
  comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
  comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
  comboVoices[track].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));
  comboVoices[track].filterEnv.attack(trackToUse.filter_attack);
  comboVoices[track].filterEnv.decay(trackToUse.filter_decay);
  comboVoices[track].filterEnv.sustain(trackToUse.filter_sustain);
  comboVoices[track].filterEnv.release(trackToUse.filter_release);
  AudioInterrupts();

  // now triggers envs
  comboVoices[track].ampEnv.noteOn();
  comboVoices[track].filterEnv.noteOn();
}

void handleNoteOnForTrack(int track)
{
  TRACK trackToUse = getHeapTrack(track);

  if (trackToUse.track_type == RAW_SAMPLE) {
    handleRawSampleNoteOnForTrack(track);
  } else if (trackToUse.track_type == WAV_SAMPLE) {
    handleWavSampleNoteOnForTrack(track);
  } else if (trackToUse.track_type == DEXED) {
    handleDexedNoteOnForTrack(track);
  } else if (trackToUse.track_type == SUBTRACTIVE_SYNTH) {
    handleSubtractiveSynthNoteOnForTrack(track);
  } else if (trackToUse.track_type == CV_GATE) {
    handleCvGateNoteOnForTrack(track);
  }
}

void handleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);

  if (trackToUse.muted) {
    return;
  }

  if (_trkNeedsInit[track]) {
    Serial.print("init sounds for track: ");
    Serial.println(track);

    if (trackToUse.track_type == RAW_SAMPLE) {
      //newdigate::flashloader loader;

      samples[trackToUse.raw_sample_id] = loader.loadSample(usableSampleNames[trackToUse.raw_sample_id]);
    }

    initSoundsForTrack(track);
  }

  if (trackToUse.track_type == RAW_SAMPLE) {
    handleRawSampleNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == WAV_SAMPLE) {
    handleWavSampleNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == DEXED) {
    handleDexedNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == SUBTRACTIVE_SYNTH) {
    handleSubtractiveSynthNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == MIDI_OUT) {
    handleMIDINoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == CV_GATE) {
    handleCvGateNoteOnForTrackStep(track, step);
  }
}

void handleNoteOffForTrackStep(int track, int step)
{
  TRACK currTrack = getHeapTrack(track);

  if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    comboVoices[track].ampEnv.noteOff();
    comboVoices[track].filterEnv.noteOff();
  }

  else if (currTrack.track_type == DEXED) {
    comboVoices[track].dexed.notesOff();
    //comboVoices[track].dexed.keyup(1);
  }
  
  // fix
  else if (currTrack.track_type == MIDI_OUT) {
    MIDI.sendNoteOff(64, 100, 1);
  } else if (currTrack.track_type == CV_GATE) {
    if (currTrack.channel == 1) {
      //writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS1, 1, 0); // gate
    } else if (currTrack.channel == 2)  {
      //writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS2, 1, 0); // gate
    } else if(currTrack.channel == 3) {
      //writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS3, 1, 0); // gate
    } else if (currTrack.channel == 4)  {
      //writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS4, 1, 0); // gate
    }
  } 
  
  else {
    if (track > 3) {
      int tOffset = track-4;
      sampleVoices[tOffset].ampEnv.noteOff();
    } else {
      comboVoices[track].ampEnv.noteOff();
    }
  }
}

void handleNoteOffForTrack(int track)
{
  TRACK currTrack = getHeapTrack(track);

  if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    comboVoices[track].ampEnv.noteOff();
    comboVoices[track].filterEnv.noteOff();
  } 

  else if (currTrack.track_type == DEXED) {
    comboVoices[track].dexed.notesOff();
    //comboVoices[track].dexed.keyup(1);
  }
  
  // fix
  else if (currTrack.track_type == MIDI_OUT) {
    MIDI.sendNoteOff(64, 100, 1);
  } else if (currTrack.track_type == CV_GATE) {
    if (currTrack.channel == 1) {
      //writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS1, 1, 0); // gate
    } else if (currTrack.channel == 2)  {
      //writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS2, 1, 0); // gate
    } else if(currTrack.channel == 3) {
      //writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS3, 1, 0); // gate
    } else if (currTrack.channel == 4)  {
      //writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
      writeToDAC(CS4, 1, 0); // gate
    }
  } 
  
  else {
    if (track > 3) {
      int tOffset = track-4;
      sampleVoices[tOffset].ampEnv.noteOff();
    } else {
      comboVoices[track].ampEnv.noteOff();
    }
  }
}

void setDisplayStateForAllStepLEDs(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  const int MAX_TRACK_LEDS_SIZE = 17;
  for (int l = 1; l < MAX_TRACK_LEDS_SIZE; l++) {
    int stepToUse = l;

    // todo: check if current track has last_step > 16
    // if so, use proper offset to get correct step state for current page
    if (current_step_page == 2) {
      stepToUse += 16;
    } else if (current_step_page == 3) {
      stepToUse += 32;
    } else if (current_step_page == 4) {
      stepToUse += 48;
    }

    TRACK_STEP currTrackStepForLED = currTrack.steps[stepToUse-1];
    int8_t curr_led_char = stepCharMap[l];
    int8_t keyLED = getKeyLED(curr_led_char);

    if (stepToUse > currTrack.last_step) {
      setLEDPWM(keyLED, 0);
      continue;
    }

    if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_OFF) {
      if (keyLED < 0) {
        Serial.println("could not find key LED!");
      } else {
        setLEDPWM(keyLED, 0);
      }
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ON) {
      setLEDPWM(keyLED, 512); // 256 might be better
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ACCENTED) {
      setLEDPWM(keyLED, 4095);
    }
  }
}

void setDisplayStateForPatternActiveTracksLEDs(bool enable)
{
  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  const int MAX_PATTERN_TRACK_SIZE = 17;
  for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t-1];
    int8_t curr_led_char = stepCharMap[t];
    int8_t keyLED = getKeyLED(curr_led_char);
    int8_t currTrackStep = _seq_state.current_track_steps[t-1].current_step;
    
    TRACK_STEP currTrackStepForLED = currTrack.steps[currTrackStep-1];

    if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_OFF) {
      if (keyLED < 0) {
        Serial.println("could not find key LED!");
      } else {
        setLEDPWM(keyLED, 0);
      }
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ON || currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ACCENTED) {
      setLEDPWM(keyLED, enable ? 4095 : 0);
    }
  }
}

void displayCurrentlySelectedBank(void)
{
  for (int b = 0; b < MAXIMUM_SEQUENCER_BANKS; b++) {
    if (b == current_selected_bank) {
      setLEDPWM(stepLEDPins[b], 4095);
    } else {
      setLEDPWM(stepLEDPins[b], 512);
    }
  }
}

void displayCurrentlySelectedPattern(void)
{
  for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++) {
    if (_seq_external.banks[current_selected_bank].patterns[p].initialized) {
      if (p == current_selected_pattern) {
        setLEDPWM(stepLEDPins[p], 4095);
      } else {
        setLEDPWM(stepLEDPins[p], 512);
      }
    } else {
      setLEDPWM(stepLEDPins[p], 0);
    }
  }
}

void displayCurrentlySelectedTrack(void)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
    if (_seq_external.banks[current_selected_bank].patterns[current_selected_pattern].tracks[t].initialized) {
      if (t == current_selected_track) {
        setLEDPWM(stepLEDPins[t], 4095);
      } else {
        setLEDPWM(stepLEDPins[t], 512);
      }
    } else {
      setLEDPWM(stepLEDPins[t], 0);
    }
  }
}

void clearAllStepLEDs(void)
{
  for (int s = 0; s < 16; s++) {
    setLEDPWM(stepLEDPins[s], 0);
  }
}

void displayPerformModeLEDs(void)
{
  for (int s = 12; s < 16; s++) {
    setLEDPWM(stepLEDPins[s], 4095);
  }
}

void displayMuteLEDs(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    if (!currPattern.tracks[t].muted) {
      setLEDPWM(stepLEDPins[t], 0);
    } else {
      setLEDPWM(stepLEDPins[t], 4095);
    }
  }
}

void displayInitializedPatternLEDs()
{
  for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++) {
    if (_seq_external.banks[current_selected_bank].patterns[p].initialized) {
      setLEDPWM(stepLEDPins[p], 4095);
    } else {
      setLEDPWM(stepLEDPins[p], 0);
    }
  }
}

void displayInitializedTrackLEDs()
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
    if (_seq_heap.pattern.tracks[t].initialized) {
      setLEDPWM(stepLEDPins[t], 4095);
    } else {
      setLEDPWM(stepLEDPins[t], 0);
    }
  }
}

void handleSoloForTrack(uint8_t track, bool undoSoloing)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    if (undoSoloing && current_tracks_soloed > 0 && t == track) {
      --current_tracks_soloed;
      _seq_heap.pattern.tracks[t].soloing = false;
      _seq_heap.pattern.tracks[t].muted = true;
      Serial.println("here1");
      setLEDPWM(stepLEDPins[t], 0);
    } 
    
    if (undoSoloing && current_tracks_soloed == 0) {
      _seq_heap.pattern.tracks[t].soloing = false;
      _seq_heap.pattern.tracks[t].muted = false;
      Serial.println("here2");
      setLEDPWM(stepLEDPins[t], 0);
    }

    if (undoSoloing) continue;

    if (t == track) {
      ++current_tracks_soloed;
      _seq_heap.pattern.tracks[t].soloing = true;
      _seq_heap.pattern.tracks[t].muted = false;
      setLEDPWM(stepLEDPins[t], 4095);
      Serial.println("here3");
    } else if (!_seq_heap.pattern.tracks[t].soloing) {
      _seq_heap.pattern.tracks[t].soloing = false;
      _seq_heap.pattern.tracks[t].muted = true;
      setLEDPWM(stepLEDPins[t], 0);
      Serial.println("here4");
    }
  }
}

void handleRemoveFromStepStack(uint32_t tick)
{
  for ( int i = 0; i < STEP_STACK_SIZE; i++ ) {
    if (_step_stack[i].length != -1) {
      --_step_stack[i].length;

      if (_step_stack[i].length == 0) {
        handleNoteOffForTrackStep(_step_stack[i].trackNum, _step_stack[i].stepNum);

        // re-initialize stack entry
        _step_stack[i].trackNum = -1;
        _step_stack[i].stepNum = -1;
        _step_stack[i].length = -1;
      }
    }  
  }
}

void handleAddToStepStack(uint32_t tick, int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);

  bool lenStepModEnabled = _pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LENGTH];
  int lenStepMod = _pattern_mods_mem.tracks[track].steps[step].length;

  for ( uint8_t i = 0; i < STEP_STACK_SIZE; i++ ) {
    if (!trackToUse.muted && _step_stack[i].length == -1 ) {
      _step_stack[i].trackNum = track;
      _step_stack[i].stepNum = step;

      _step_stack[i].length = lenStepModEnabled ? lenStepMod : trackToUse.length;

      handleNoteOnForTrackStep(_step_stack[i].trackNum, _step_stack[i].stepNum);

      return;
    }
  }
}

void handleAddToRatchetStack()
{
  for ( uint8_t i = 0; i < STEP_STACK_SIZE; i++ ) {
    if ( _ratchet_stack[i].length == -1 ) {
      _ratchet_stack[i].trackNum = ratcheting_track;
      _ratchet_stack[i].length = 2; // TODO: shorten?

      handleNoteOnForTrack(_ratchet_stack[i].trackNum);

      return;
    }
  }
}

void handleRemoveFromRatchetStack()
{
  for ( int i = 0; i < STEP_STACK_SIZE; i++ ) {
    if (_ratchet_stack[i].length != -1) {
      --_ratchet_stack[i].length;

      if (_ratchet_stack[i].length == 0) {
        handleNoteOffForTrack(_ratchet_stack[i].trackNum);

        // re-initialize stack entry
        _ratchet_stack[i].trackNum = -1;
        _ratchet_stack[i].length = -1;
      }
    }  
  }
}

void clearPageLEDs()
{
  setLEDPWM(17, 0);
  setLEDPWM(18, 0);
  setLEDPWM(19, 0);
  setLEDPWM(20, 0);
}

void displayPageLEDs(int currentBar)
{
  bool blinkCurrentPage = _seq_state.playback_state == RUNNING && currentBar != -1;

  TRACK currTrack = getHeapCurrentSelectedTrack();

  uint16_t pageOneBrightnessMin = (current_step_page == 1) ? 50 : 5;
  uint16_t pageTwoBrightnessMin = (current_step_page == 2) ? 50 : (currTrack.last_step > 16 ? 5 : 0);
  uint16_t pageThreeBrightnessMin = (current_step_page == 3) ? 50 : (currTrack.last_step > 32 ? 5 : 0);
  uint16_t pageFourBrightnessMin = (current_step_page == 4) ? 50 : (currTrack.last_step > 48 ? 5 : 0);

  uint16_t pageOneBrightnessMax = blinkCurrentPage && (currentBar == 1) ? pageOneBrightnessMin + 100 : pageOneBrightnessMin;
  uint16_t pageTwoBrightnessMax = blinkCurrentPage && (currentBar == 2) ? pageTwoBrightnessMin + 100 : pageTwoBrightnessMin;
  uint16_t pageThreeBrightnessMax = blinkCurrentPage && (currentBar == 3) ? pageThreeBrightnessMin + 100 : pageThreeBrightnessMin;
  uint16_t pageFourBrightnessMax = blinkCurrentPage && (currentBar == 4) ? pageFourBrightnessMin + 100 : pageFourBrightnessMin;
  
  setLEDPWM(17, pageOneBrightnessMax);
  setLEDPWM(18, pageTwoBrightnessMax);
  setLEDPWM(19, pageThreeBrightnessMax);
  setLEDPWM(20, pageFourBrightnessMax);
}

void updateCurrentPatternStepState(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currPatternLastStep = currPattern.last_step;

  if (_seq_state.current_step <= currPatternLastStep) {
    if (_seq_state.current_step < currPatternLastStep) {
      if (!((_seq_state.current_step+1) % 16)) { // TODO: make this bar division configurable
        ++_seq_state.current_bar;
      }

      ++_seq_state.current_step; // advance current step for sequencer
    } else {
      _seq_state.current_step = 1; // reset current step
      _seq_state.current_bar = 1; // reset current bar

      if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
        // nullify pattern queue
        dequeue_pattern = true;
      }
    }
  }
}

void updateAllTrackStepStates(void)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    TRACK track = _seq_heap.pattern.tracks[t];
    int trackLastStep = track.last_step;
    int8_t track_current_step = _seq_state.current_track_steps[t].current_step;

    if (track_current_step <= trackLastStep) {
      if (track_current_step < trackLastStep) {
        if (!((track_current_step+1) % 16)) { // TODO: make this bar division configurable
          ++_seq_state.current_track_steps[t].current_bar;
        }

        ++_seq_state.current_track_steps[t].current_step; // advance current step for track
      } else {
        _seq_state.current_track_steps[t].current_step = 1; // reset current step for track
        _seq_state.current_track_steps[t].current_bar = 1; // reset current bar for track
      }
    }
  }
}

void ClockOut16PPQN(uint32_t tick)
{
  handle_sixteenth_step(tick);
}

void handle_sixteenth_step(uint32_t tick)
{
  int8_t currentSelectedPatternCurrentStep = _seq_state.current_step;
  int8_t currentSelectedTrackCurrentStep = _seq_state.current_track_steps[current_selected_track].current_step;
  int8_t currentSelectedTrackCurrentBar = _seq_state.current_track_steps[current_selected_track].current_bar;

  int curr_step_paged = currentSelectedTrackCurrentStep;
  if (current_step_page == 2 && curr_step_paged > 16 && curr_step_paged <= 32) {
    curr_step_paged -= 16;
  } else if (current_step_page == 3 && curr_step_paged > 32 && curr_step_paged <= 48) {
    curr_step_paged -= 32;
  } else if (current_step_page == 4 && curr_step_paged > 48 && curr_step_paged <= 64) {
    curr_step_paged -= 48;
  }

  bool isOnStraightQtrNote = (currentSelectedTrackCurrentStep == 1 || !((currentSelectedTrackCurrentStep-1) % 4));
  
  if (current_UI_mode == TRACK_WRITE && !function_started && isOnStraightQtrNote) {
    displayPageLEDs(currentSelectedTrackCurrentBar);
  }

  if (isOnStraightQtrNote) {
    setLEDPWM(23, 4095); // each straight quarter note start button led ON
  }

  // This method handles advancing the sequencer
  // and displaying the start btn and step btn BPM LEDs
  int8_t curr_step_char = stepCharMap[curr_step_paged];
  int8_t keyLED = getKeyLED(curr_step_char);

  if (current_UI_mode == PATTERN_WRITE) {
    setDisplayStateForPatternActiveTracksLEDs(true);
  }

  // TODO: move out of 16th step !(tick % (6)) condition
  // so we can check for microtiming adjustments at the 96ppqn scale
  triggerAllStepsForAllTracks(tick);

  if (current_UI_mode == PATTERN_SEL) {
    clearAllStepLEDs();
    displayCurrentlySelectedPattern();
  } else if (current_UI_mode == TRACK_SEL) {
    clearAllStepLEDs();
    displayCurrentlySelectedTrack();
  } else if (current_UI_mode == TRACK_WRITE) {
    bool transitionStepLEDs = (
      (current_step_page == 1 && currentSelectedTrackCurrentStep <= 16) || 
      (current_step_page == 2 && currentSelectedTrackCurrentStep > 16 && currentSelectedTrackCurrentStep <= 32) ||
      (current_step_page == 3 && currentSelectedTrackCurrentStep > 32 && currentSelectedTrackCurrentStep <= 48) ||
      (current_step_page == 4 && currentSelectedTrackCurrentStep > 48 && currentSelectedTrackCurrentStep <= 64)
    );

    bool turnOffLastLED = (
      (current_step_page == 1 && currentSelectedTrackCurrentStep == 1) || 
      (current_step_page == 2 && currentSelectedTrackCurrentStep == 16) ||
      (current_step_page == 3 && currentSelectedTrackCurrentStep == 32) ||
      (current_step_page == 4 && currentSelectedTrackCurrentStep == 48)
    );

    if (currentSelectedTrackCurrentStep > 1) {
      if (transitionStepLEDs) {
          uint8_t prevKeyLED = getKeyLED(stepCharMap[curr_step_paged-1]);
          setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
      }
    } else {
      if (turnOffLastLED) {
        TRACK currTrack = getHeapCurrentSelectedTrack();
        int currTrackLastStep = currTrack.last_step;
        if (currTrackLastStep > 16) {
          currTrackLastStep -= 16;
        }

        uint8_t prevKeyLED = getKeyLED(stepCharMap[currTrackLastStep]);
        setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
      }
    }

    if (transitionStepLEDs) {
      setLEDPWM(keyLED, 4095); // turn sixteenth led ON
    }
  }

  updateCurrentPatternStepState();
  updateAllTrackStepStates();
}

void handle_bpm_step(uint32_t tick)
{
  if ((tick % 6) && !(tick % bpm_blink_timer) ) {
    setLEDPWM(23, 0); // turn start button led OFF
  }

  if (!(tick % 6)) {
    if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
      draw_queue_blink = 0;
    }
  }

  triggerRatchetingTrack(tick);
  handleRemoveFromStepStack(tick);

  if ((tick % (6)) && !(tick % bpm_blink_timer) ) {
    if (current_UI_mode == PATTERN_WRITE) {
      setDisplayStateForPatternActiveTracksLEDs(false);
    } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE) {
      setDisplayStateForAllStepLEDs();
      if (!function_started) {
        displayPageLEDs(-1);
      }
    }
  }

  // every 1/4 step log memory usage
  if (!(tick % 24)) {
    // blink queued bank / pattern
    if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
      draw_queue_blink = 1;
    }
  }
}

void rewindAllCurrentStepsForAllTracks(void)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    _seq_state.current_track_steps[t].current_step = 1;
  }
}

void toggleSequencerPlayback(char btn)
{
  int8_t curr_step_char = stepCharMap[_seq_state.current_step-1];
  uint8_t keyLED = getKeyLED(curr_step_char);

  if (_seq_state.playback_state > STOPPED) {
    if (_seq_state.playback_state == RUNNING && btn == 'q') {
      _seq_state.playback_state = PAUSED;
      uClock.pause();
      
      setLEDPWMDouble(23, 0, keyLED, 0);
      displayPageLEDs(-1);

      if (current_UI_mode == UI_MODE::TRACK_WRITE) {
        setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
      } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
        clearAllStepLEDs();
      }
    } else if (_seq_state.playback_state == PAUSED && btn == 'q') {
      // Unpaused, so advance sequencer from last known step
      _seq_state.playback_state = RUNNING;
      uClock.pause();
    } else if (btn == 'w') {
      // Stopped, so reset sequencer to FIRST step in pattern
      _seq_state.current_step = 1;
      _seq_state.current_bar = 1;

      rewindAllCurrentStepsForAllTracks();

      _seq_state.playback_state = STOPPED;
      uClock.stop();

      setLEDPWM(keyLED, 0); // turn off current step LED
      setLEDPWM(23, 0); // turn start button led OFF
      
      if (current_UI_mode == UI_MODE::TRACK_WRITE) {
        setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
      } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
        clearAllStepLEDs();
      }
    }
  } else if (btn == 'q') {
    // Started, so start sequencer from FIRST step in pattern
    //_seq_state.current_step = 1;
    _seq_state.playback_state = RUNNING;
    uClock.start();
  } else if (btn == 'w') {
    // Stopped, so reset sequencer to FIRST step in pattern
    _seq_state.current_step = 1;
    _seq_state.current_bar = 1;

    rewindAllCurrentStepsForAllTracks();

    _seq_state.playback_state = STOPPED;
    uClock.stop();
    
    //setLEDPWM(keyLED, 0); // turn off current step LED
    setLEDPWM(23, 0); // turn start button led OFF
    
    if (current_UI_mode == UI_MODE::TRACK_WRITE) {
      setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
    } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
      clearAllStepLEDs();
    }
  }
}

const char* getKeyStr(char idx) {
  return charCharsMap[idx];
}

int8_t getKeyLED(char idx) {
  if (charLEDMap.count(idx) != 0) {
    return charLEDMap[idx];
  }

  // Serial.print("key LED not found for idx: ");
  // Serial.println(idx);

  return -1;
}

bool btnCharIsATrack(char btnChar) {
  char* validTrackChars = "mnopstuvyz125678";
  if (strchr(validTrackChars, btnChar) != NULL)
  {
    return true;
  }

  return false;
}

uint8_t getKeyStepNum(char idx)
{
  if (charStepMap.count(idx) != 0) {
    return charStepMap[idx];
  }

  return 1; // default first step num
}

void toggleSelectedStep(uint8_t step)
{
  int stepToUse = step;

  // todo: check if current track has last_step > 16
  // if so, use proper offset to get correct step state for current page
  if (current_step_page == 2) {
    stepToUse += 16;
  } else if (current_step_page == 3) {
    stepToUse += 32;
  } else if (current_step_page == 4) {
    stepToUse += 48;
  }

  uint8_t adjStep = stepToUse-1; // get zero based step num

  Serial.print("adjStep: ");
  Serial.println(adjStep);

  TRACK currTrack = getHeapCurrentSelectedTrack();
  TRACK_STEP_STATE currStepState = currTrack.steps[adjStep].state;

  Serial.print("currStepState: ");
  Serial.println(currStepState == TRACK_STEP_STATE::STATE_ACCENTED ? "accented" : (currStepState == TRACK_STEP_STATE::STATE_ON ? "on" : "off"));

  // TODO: implement accent state for MIDI, CV/Trig, Sample, Synth track types?
  if (currStepState == TRACK_STEP_STATE::STATE_OFF) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::STATE_ON;
    // copy track properties to steps
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].note = currTrack.note;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].octave = currTrack.octave;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].velocity = currTrack.velocity;
  } else if (currStepState == TRACK_STEP_STATE::STATE_ON) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::STATE_ACCENTED;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].velocity = 100; // TODO: use a "global accent" value here
  } else if (currStepState == TRACK_STEP_STATE::STATE_ACCENTED) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::STATE_OFF;
  }

  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern].tracks[current_selected_track].steps[adjStep] = _seq_heap.pattern.tracks[current_selected_track].steps[adjStep];
}

void handleEncoderSetTempo()
{
  int main_encoder_idx = 0;
  encoder_currValues[main_encoder_idx] = encoder_getValue(encoder_addrs[main_encoder_idx]);

  if(encoder_currValues[main_encoder_idx] != encoder_lastValues[main_encoder_idx]) {
    int diff = encoder_currValues[main_encoder_idx] - encoder_lastValues[main_encoder_idx];

    float currTempo = uClock.getTempo();
    float newTempo = currTempo + diff;

    if (!(newTempo < 30 || newTempo > 300)) {
      if ((newTempo - currTempo >= 1) || (currTempo - newTempo) >= 1) {
        uClock.setTempo(newTempo);
        current_project.tempo = newTempo;
        drawSetTempoOverlay();
      }
    }

    encoder_lastValues[main_encoder_idx] = encoder_currValues[main_encoder_idx];
  }
}

void updateTrackLastStep(int diff);
void updateTrackLastStep(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currLastStep = currTrack.last_step;
  int newLastStep = currTrack.last_step + diff;

  // make sure track last step doesn't exceed pattern's
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  // TODO: try to re-align current playing track step with pattern step if able

  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > currPattern.last_step) {
    newLastStep = currPattern.last_step;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

    displayPageLEDs(-1);
    setDisplayStateForAllStepLEDs();
    drawSequencerScreen(false);
  }
}

void updateTrackLength(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  // length adj
  int currLen = currTrack.length;
  int newLen = currLen + diff;

  if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
    currLen = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].length;
    newLen = currLen + diff;
  }

  if (!(newLen < 0 && newLen > 64) && (newLen != currLen)) {
    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::LENGTH] = true;
      _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].length = newLen;
    } else {
      _seq_heap.pattern.tracks[current_selected_track].length = newLen;
    }

    drawSequencerScreen(false);
  }
}

void updateSubtractiveSynthWaveform(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currWaveform = waveformFindMap[(int)currTrack.waveform];
  if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
    currWaveform = waveformFindMap[(int)_pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].waveform];
  }

  int newWaveform = currWaveform + diff;

  if (newWaveform < 0) {
    newWaveform = 5;
  } else if (newWaveform > 5) {
    newWaveform = 0;
  }

  int waveformSel = waveformSelMap[newWaveform];

  if (current_UI_mode == SUBMITTING_STEP_VALUE) {
    _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::WAVEFORM] = true;
    _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].waveform = waveformSel;
  } else {
    _seq_heap.pattern.tracks[current_selected_track].waveform = waveformSel;
  }

  comboVoices[current_selected_track].osca.begin(waveformSel);
  comboVoices[current_selected_track].oscb.begin(waveformSel);

  drawSequencerScreen(false);
}

void updateSubtractiveSynthNoiseAmt(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  float currNoise = currTrack.noise;
  float newNoise = currTrack.noise + (diff * 0.05);

  if (!(newNoise < 0.01 || newNoise > 1.0) && newNoise != currNoise) {
    _seq_heap.pattern.tracks[current_selected_track].noise = newNoise;

    AudioNoInterrupts();
    comboVoices[current_selected_track].noise.amplitude(newNoise);
    AudioInterrupts();

    drawSequencerScreen(false);
  }
}

void updateSubtractiveSynthFilterEnvAttack(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currAtt = currTrack.filter_attack;

  int mult = 1;
  if (abs(diff) > 5) {
    mult = 200;
  } else if (abs(diff) > 1 && abs(diff) < 5) {
    mult = 20;
  }

  float newAtt = currTrack.filter_attack + (diff * mult);
  
  // real max attack = 11880
  if (!(newAtt < 0 || newAtt > 1000) && newAtt != currAtt) {
    _seq_heap.pattern.tracks[current_selected_track].filter_attack = newAtt;

    AudioNoInterrupts();
    comboVoices[current_selected_track].filterEnv.attack(newAtt);
    AudioInterrupts();

    drawSequencerScreen(false);
  }
}

void updateTrackAmpEnvAttack(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currAtt = currTrack.amp_attack;

  int mult = 1;
  if (abs(diff) > 5) {
    mult = 100;
  } else if (abs(diff) > 1 && abs(diff) < 5) {
    mult = 10;
  }

  float newAtt = currTrack.amp_attack + (diff * mult);

  if (!(newAtt < 1 || newAtt > 500) && newAtt != currAtt) {
    _seq_heap.pattern.tracks[current_selected_track].amp_attack = newAtt;

    AudioNoInterrupts();
    comboVoices[current_selected_track].ampEnv.attack(newAtt);
    AudioInterrupts();

    drawSequencerScreen(false);
  }
}

void updateComboTrackLevel(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currLvl = currTrack.level;
  float newLvl = currTrack.level + (diff * 0.01);

  if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl) {
    _seq_heap.pattern.tracks[current_selected_track].level = newLvl;

    AudioNoInterrupts();
    comboVoices[current_selected_track].leftSubMix.gain(0, newLvl);
    comboVoices[current_selected_track].leftSubMix.gain(1, newLvl);
    comboVoices[current_selected_track].rightSubMix.gain(0, newLvl);
    comboVoices[current_selected_track].rightSubMix.gain(1, newLvl);
    AudioInterrupts();

    drawSequencerScreen(false);
  }
}

void handleEncoderSubtractiveSynthModA(int diff)
{
  switch (current_page_selected)
  {
  case 0:
    updateTrackLastStep(diff);
    break;
  case 1:
    updateSubtractiveSynthWaveform(diff);
    break;
  case 2:
    updateSubtractiveSynthNoiseAmt(diff);
    break;
  case 3:
    updateSubtractiveSynthFilterEnvAttack(diff);
    break;
  case 4:
    updateTrackAmpEnvAttack(diff);
    break;
  case 5:
    updateComboTrackLevel(diff);
    break;
  default:
    break;
  }
}

void handleEncoderSubtractiveSynthModB(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    updateTrackLength(diff);
  } else if (current_page_selected == 1) {
    float currDetune = currTrack.detune;
    float newDetune = currDetune + diff;

    if (!(newDetune < -24 || newDetune > 24) && newDetune != currDetune) {
      uint8_t note_to_use = currTrack.note;
      if (keyboardNotesHeld != 0) {
        note_to_use = note_on_keyboard;
      }

      _seq_heap.pattern.tracks[current_selected_track].detune = newDetune;

      // if seq is running, don't adjust detune in realtime:
      if (_seq_state.playback_state != RUNNING) {          
        AudioNoInterrupts();

        
        float foundBaseFreq = noteToFreqArr[note_to_use];
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)newDetune/12.0)) * (pow(2, keyboardOctave));
        comboVoices[current_selected_track].oscb.frequency(octaveFreqB);

        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 2) {
    float currCutoff = currTrack.cutoff;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newCutoff = currTrack.cutoff + (diff * mult);

    if (!(newCutoff < 1 || newCutoff > 3000) && newCutoff != currCutoff) {
      _seq_heap.pattern.tracks[current_selected_track].cutoff = newCutoff;

      AudioNoInterrupts();
      comboVoices[current_selected_track].lfilter.frequency(newCutoff);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    float currDecay = currTrack.filter_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.filter_decay + (diff * mult);

    if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].filter_decay = newDecay;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.decay(newDecay);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 4) {
    float currDecay = currTrack.amp_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.amp_decay + (diff * mult);

    // real max decay = 11880
    if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].amp_decay = newDecay;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.decay(newDecay);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 5) {
    float currPan = currTrack.pan;
    float newPan = currTrack.pan + (diff * 0.1);

    if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
      _seq_heap.pattern.tracks[current_selected_track].pan = newPan;

      float newGainL = 1.0;
      if (newPan < 0) {
        newGainL += newPan;
      }

      float newGainR = 1.0;
      if (newPan > 0) {
        newGainR -= newPan;
      }

      AudioNoInterrupts();
      // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
      // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
      comboVoices[current_selected_track].leftCtrl.gain(newGainR);
      comboVoices[current_selected_track].rightCtrl.gain(newGainL);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  }
}

void handleEncoderSubtractiveSynthModC(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    float currVel = currTrack.velocity;
    float newVel = currVel + diff;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      TRACK_STEP currStep = getHeapCurrentSelectedTrackStep();

      currVel = currStep.velocity;
      newVel = currVel + diff;
    }

    if (!(newVel < 1 || newVel > 100) && newVel != currVel) {
      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::VELOCITY] = true;
        _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].velocity = newVel;
      } else {
        _seq_heap.pattern.tracks[current_selected_track].velocity = newVel;
      }

      // AudioNoInterrupts();
      // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
      // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
      // AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 1) {
    float currFine = currTrack.fine;
    float newFine = currFine + diff;

    if (!(newFine < -50.0 || newFine > 50.0) && newFine != currFine) {
      _seq_heap.pattern.tracks[current_selected_track].fine = newFine;
      TRACK_STEP currStep = _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];

      // TODO: also use step note
      uint8_t note_to_use = currTrack.note;
      if (keyboardNotesHeld != 0) {
        note_to_use = note_on_keyboard;
      } else {
        //note_to_use = currStep.note;
      }

      // if seq running, don't adjust fine freq in realtime:
      if (_seq_state.playback_state != RUNNING) {
        float foundBaseFreq = noteToFreqArr[note_to_use];
        float octaveFreqA = (foundBaseFreq + (currTrack.fine * 0.01)) * (pow(2, keyboardOctave));
      
        AudioNoInterrupts();

        comboVoices[current_selected_track].osca.frequency(octaveFreqA);

        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 2) {
    float currRes = currTrack.res;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 10;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 5;
    }

    float newRes = currTrack.res + (diff * mult * 0.01);

    if (!(newRes < -0.01 || newRes > 1.9) && newRes != currRes) {
      _seq_heap.pattern.tracks[current_selected_track].res = newRes;

      AudioNoInterrupts();
      comboVoices[current_selected_track].lfilter.resonance(newRes);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    float curSus = currTrack.filter_sustain;
    float newSus = currTrack.filter_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].filter_sustain = newSus;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.sustain(newSus);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 4) {
    float curSus = currTrack.amp_sustain;
    float newSus = currTrack.amp_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].amp_sustain = newSus;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.sustain(newSus);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 5) {
    //
  }
}

void handleEncoderSubtractiveSynthModD(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) { // first page
    // probability
  } else if (current_page_selected == 1) { // width
    float currWidth = currTrack.width;
    float newWidth = currWidth + (diff * 0.01);

    if (!(newWidth < 0.01 || newWidth > 1.0) && newWidth != currWidth) {
      _seq_heap.pattern.tracks[current_selected_track].width = newWidth;
      comboVoices[current_selected_track].osca.pulseWidth(newWidth);
      comboVoices[current_selected_track].oscb.pulseWidth(newWidth);

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 2) {
    float currFilterAmt = currTrack.filterenvamt;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 150;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newFilterAmt = currTrack.filterenvamt + (diff * mult * 0.005);

    if (!(newFilterAmt < -1.0 || newFilterAmt > 1.0) && newFilterAmt != currFilterAmt) {
      _seq_heap.pattern.tracks[current_selected_track].filterenvamt = newFilterAmt;

      AudioNoInterrupts();
      comboVoices[current_selected_track].dc.amplitude(newFilterAmt);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    float curRel = currTrack.filter_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.filter_release + (diff * mult);

    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].filter_release = newRel;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.release(newRel);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 4) {
    float curRel = currTrack.amp_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.amp_release + (diff * mult);

    // real max release = 11880
    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].amp_release = newRel;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.release(newRel);
      AudioInterrupts();

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 5) {
      // n/a
  }
}

void handleEncoderDexedModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currLastStep = currTrack.last_step;
  int newLastStep = currTrack.last_step + diff;

  // make sure track last step doesn't exceed pattern's
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  // TODO: try to re-align current playing track step with pattern step if able
  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > currPattern.last_step) {
    newLastStep = currPattern.last_step;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

    displayPageLEDs(-1);
    setDisplayStateForAllStepLEDs();
    drawSequencerScreen(false);
  }
}

void handleEncoderDexedModB(int diff)
{
  updateTrackLength(diff);
}

void handleEncoderDexedModC(int diff)
{
  //
}

void handleEncoderDexedModD(int diff)
{
  //
}

void handleEncoderRawSampleModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    int currLastStep = currTrack.last_step;
    int newLastStep = currTrack.last_step + diff;

    // make sure track last step doesn't exceed pattern's
    PATTERN currPattern = getHeapCurrentSelectedPattern();

    // TODO: try to re-align current playing track step with pattern step if able
    if (newLastStep < 1) {
      newLastStep = 1;
    } else if (newLastStep > currPattern.last_step) {
      newLastStep = currPattern.last_step;
    }

    if (newLastStep != currLastStep) {
      _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

      displayPageLEDs(-1);
      setDisplayStateForAllStepLEDs();
      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 1) {
    int currLoopType = currTrack.looptype;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      currLoopType = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].looptype;
    }

    int newLoopType = (currLoopType + diff);

    if (newLoopType < 0) {
      newLoopType = 1;
    } else if (newLoopType > 1) {
      newLoopType = 0;
    }

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::LOOPTYPE] = true;
      _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].looptype = newLoopType;
    } else {
      _seq_heap.pattern.tracks[current_selected_track].looptype = newLoopType;
    }

    drawSequencerScreen(false);
  } else if (current_page_selected == 2) {
    float currAtt = currTrack.amp_attack;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newAtt = currTrack.amp_attack + (diff * mult);

    // real max attack = 11880
    if (!(newAtt < 1 || newAtt > 1000) && newAtt != currAtt) {
      _seq_heap.pattern.tracks[current_selected_track].amp_attack = newAtt;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.attack(newAtt);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.attack(newAtt);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    float currLvl = currTrack.level;
    float newLvl = currTrack.level + (diff * 0.01);

    if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl) {
      _seq_heap.pattern.tracks[current_selected_track].level = newLvl;

      Serial.print("current_selected_track: ");
      Serial.print(current_selected_track);
      Serial.print(" newLvl: ");
      Serial.println(newLvl);

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].leftSubMix.gain(0, newLvl);
        sampleVoices[current_selected_track-4].leftSubMix.gain(1, newLvl);
        sampleVoices[current_selected_track-4].rightSubMix.gain(0, newLvl);
        sampleVoices[current_selected_track-4].rightSubMix.gain(1, newLvl);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].leftSubMix.gain(0, newLvl);
        comboVoices[current_selected_track].leftSubMix.gain(1, newLvl);
        comboVoices[current_selected_track].rightSubMix.gain(0, newLvl);
        comboVoices[current_selected_track].rightSubMix.gain(1, newLvl);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  }
}

void handleEncoderRawSampleModB(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    int newSampleId = currTrack.raw_sample_id + diff;

    if (newSampleId < 0) {
      newSampleId = (rawSamplesAvailable-1);
    } else if (newSampleId > (rawSamplesAvailable-1)) {
      newSampleId = 0;
    }

    _seq_heap.pattern.tracks[current_selected_track].raw_sample_id = newSampleId;

    drawSequencerScreen(false);
  } else if (current_page_selected == 1) {
    uint32_t currLoopStart = currTrack.loopstart;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      currLoopStart = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopstart;
    }

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    uint32_t newLoopStart = currLoopStart + (diff * mult);

    if (!(newLoopStart < 0 || newLoopStart > 10000) && newLoopStart != currLoopStart) {

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::LOOPSTART] = true;
        _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopstart = newLoopStart;
      } else {
        _seq_heap.pattern.tracks[current_selected_track].loopstart = newLoopStart;
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 2) {
    float currDecay = currTrack.amp_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.amp_decay + (diff * mult);

    // real max decay = 11880
    if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].amp_decay = newDecay;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track].ampEnv.decay(newDecay);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.decay(newDecay);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  }else if (current_page_selected == 3) {
    float currPan = currTrack.pan;
    float newPan = currTrack.pan + (diff * 0.1);

    if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
      _seq_heap.pattern.tracks[current_selected_track].pan = newPan;

      float newGainL = 1.0;
      if (newPan < 0) {
        newGainL += newPan;
      }

      float newGainR = 1.0;
      if (newPan > 0) {
        newGainR -= newPan;
      }

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        // sampleVoices[current_selected_track-4].leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
        // sampleVoices[current_selected_track-4].leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
        sampleVoices[current_selected_track-4].leftCtrl.gain(newGainR);
        sampleVoices[current_selected_track-4].rightCtrl.gain(newGainL);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
        // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
        comboVoices[current_selected_track].leftCtrl.gain(newGainR);
        comboVoices[current_selected_track].rightCtrl.gain(newGainL);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  }
}

void handleEncoderRawSampleModC(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    // sample speed adj
    float currSpeed = currTrack.sample_play_rate;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      currSpeed = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].sample_play_rate;
    }

    float newSpeed = currSpeed + (diff * 0.1);

    if (!(newSpeed < -1.1 || newSpeed > 10.1) && newSpeed != currSpeed) {
      if ((currSpeed > 0.0 && newSpeed < 0.1) || (currSpeed == -1.0 && newSpeed < -1.0)) {
        newSpeed = -1.0;
      } else if (currSpeed <= -1.0 && newSpeed > -1.0) {
        newSpeed = 0.1;
      }

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::SAMPLE_PLAY_RATE] = true;
        _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].sample_play_rate = newSpeed;
      } else {
        _seq_heap.pattern.tracks[current_selected_track].sample_play_rate = newSpeed;
      }
      
      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].rSample.setPlaybackRate(newSpeed);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].rSample.setPlaybackRate(newSpeed);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 1) {
    uint32_t currLoopFinish = currTrack.loopfinish;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      currLoopFinish = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopfinish;
    }

    Serial.print("currLoopFinish: ");
    Serial.println(currLoopFinish);

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    uint32_t newLoopFinish = currLoopFinish + (diff * mult);

    Serial.print("newLoopFinish: ");
    Serial.println(newLoopFinish);

    if (!(newLoopFinish < 0 || newLoopFinish > 10000) && newLoopFinish != currLoopFinish) {

      if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
        _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::LOOPFINISH] = true;
        _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].loopfinish = newLoopFinish;
      } else {
        _seq_heap.pattern.tracks[current_selected_track].loopfinish = newLoopFinish;
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 2) {
    float curSus = currTrack.amp_sustain;
    float newSus = currTrack.amp_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].amp_sustain = newSus;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.sustain(newSus);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.sustain(newSus);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    //
  }
}

void handleEncoderRawSampleModD(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    uint8_t currBitrate = currTrack.bitrate;
    uint8_t newBitrate = currBitrate + diff;

    if (!(newBitrate < 1 || newBitrate > 16) && newBitrate != currBitrate) {
      _seq_heap.pattern.tracks[current_selected_track].bitrate = newBitrate;

      if (current_selected_track > 3) {
        AudioNoInterrupts();

        // sampleVoices[current_selected_track].leftBitCrush.bits(newBitrate);
        // sampleVoices[current_selected_track].rightBitCrush.bits(newBitrate);

        AudioInterrupts();

      } else {
        AudioNoInterrupts();

        // comboVoices[current_selected_track].leftBitCrush.bits(newBitrate);
        // comboVoices[current_selected_track].rightBitCrush.bits(newBitrate);

        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 1) {
    play_start currPlaystart = currTrack.playstart;

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      currPlaystart = _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].playstart;
    }

    int newPlayStart = (playStartFindMap[currPlaystart]) + diff;

    if (newPlayStart < 0) {
      newPlayStart = 1;
    } else if (newPlayStart > 1) {
      newPlayStart = 0;
    }

    play_start playStartSel = playStartSelMap[newPlayStart];

    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::PLAYSTART] = true;
      _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].playstart = playStartSel;
    } else {
      _seq_heap.pattern.tracks[current_selected_track].playstart = playStartSel;
    }

    drawSequencerScreen(false);
  } else if (current_page_selected == 2) {
    float curRel = currTrack.amp_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.amp_release + (diff * mult);

    // real max release = 11880
    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].amp_release = newRel;
    
      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.release(newRel);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.release(newRel);
        AudioInterrupts();
      }

      drawSequencerScreen(false);
    }
  } else if (current_page_selected == 3) {
    // n/a
  }
}

void handleEncoderWavSampleModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currLastStep = currTrack.last_step;
  int newLastStep = currTrack.last_step + diff;

  // make sure track last step doesn't exceed pattern's
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  // TODO: try to re-align current playing track step with pattern step if able
  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > currPattern.last_step) {
    newLastStep = currPattern.last_step;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

    displayPageLEDs(-1);
    setDisplayStateForAllStepLEDs();
    drawSequencerScreen(false);
  }
}

void handleEncoderWavSampleModB(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int newSampleId = currTrack.wav_sample_id + diff;

  if (newSampleId < 0) {
    newSampleId = (wavSamplesAvailable-1);
  } else if (newSampleId > (wavSamplesAvailable-1)) {
    newSampleId = 0;
  }

  _seq_heap.pattern.tracks[current_selected_track].wav_sample_id = newSampleId;

  drawSequencerScreen(false);
}

void handleEncoderWavSampleModC(int diff)
{
  //
}

void handleEncoderWavSampleModD(int diff)
{
  //
}

void handleEncoderCvGateModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currLastStep = currTrack.last_step;
  int newLastStep = currTrack.last_step + diff;

  // make sure track last step doesn't exceed pattern's
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  // TODO: try to re-align current playing track step with pattern step if able
  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > currPattern.last_step) {
    newLastStep = currPattern.last_step;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

    displayPageLEDs(-1);
    setDisplayStateForAllStepLEDs();
    drawSequencerScreen(false);
  }
}

void handleEncoderCvGateModB(int diff)
{
  updateTrackLength(diff);
}

void handleEncoderCvGateModC(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int currOutput = currTrack.channel;
  int newOutput = currTrack.channel + diff;

  // TODO: try to re-align current playing track step with pattern step if able
  if (newOutput < 1) {
    newOutput = 1;
  } else if (newOutput > 4) {
    newOutput = 4;
  }

  if (newOutput != currOutput) {
    _seq_heap.pattern.tracks[current_selected_track].channel = newOutput;

    displayPageLEDs(-1);
    setDisplayStateForAllStepLEDs();
    drawSequencerScreen(false);
  }
}

void handleEncoderCvGateModD(int diff)
{
  //
}

void handleEncoderPatternModA(int diff)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currLastStep = currPattern.last_step;
  int newLastStep = currPattern.last_step + diff;

  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > MAXIMUM_SEQUENCER_STEPS) { // use max step as a #define later
    newLastStep = MAXIMUM_SEQUENCER_STEPS;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.last_step = newLastStep;

    for (int t=0; t<MAXIMUM_SEQUENCER_TRACKS; t++) {
      // set track's last_step to match pattern if track last_step is greater than pattern's
      if (_seq_heap.pattern.tracks[t].last_step > newLastStep) {
        _seq_heap.pattern.tracks[t].last_step = newLastStep;
      }
    }

    drawSequencerScreen(false);
  }
}

void handleEncoderPatternModB(int diff)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currGrooveId = currPattern.groove_id;
  int newGrooveId = currPattern.groove_id + diff;

  if (newGrooveId < -1) {
    newGrooveId = -1;
  } else if (newGrooveId > MAXIMUM_GROOVE_CONFIGS-1) {
    newGrooveId = MAXIMUM_GROOVE_CONFIGS-1;
  }

  if (newGrooveId != currGrooveId) {
    _seq_heap.pattern.groove_id = newGrooveId;

    if (newGrooveId == -1) {
      uClock.setShuffle(false);
    } else if (!uClock.isShuffled()) {
      uClock.setShuffleTemplate(_grooves.configs[newGrooveId].templates[currPattern.groove_amount]);
      uClock.setShuffle(true);
    }

    drawSequencerScreen(false);
  }
}

void handleEncoderPatternModC(int diff)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currGrooveAmt = currPattern.groove_amount;
  int newGrooveAmt = currPattern.groove_amount + diff;

  if (newGrooveAmt < 0) {
    newGrooveAmt = 0;
  } else if (newGrooveAmt > MAXIMUM_GROOVE_OPTIONS-1) {
    newGrooveAmt = MAXIMUM_GROOVE_OPTIONS-1;
  }

  if (newGrooveAmt != currGrooveAmt) {
    _seq_heap.pattern.groove_amount = newGrooveAmt;

    uClock.setShuffleTemplate(_grooves.configs[currPattern.groove_id].templates[newGrooveAmt]);

    drawSequencerScreen(false);
  }
}

void handleEncoderPatternModD(int diff)
{
  //
}

void handleEncoderSetTrackMods(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  const int modCount = 4;
  for (int m = 0; m < modCount; m++)
  {
    int modEncoderIdx = m+1;

    encoder_currValues[modEncoderIdx] = encoder_getValue(encoder_addrs[modEncoderIdx]);

    if(encoder_currValues[modEncoderIdx] != encoder_lastValues[modEncoderIdx]) {
      int diff = encoder_currValues[modEncoderIdx] - encoder_lastValues[modEncoderIdx];

      if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
        if (m == 0) {
          handleEncoderSubtractiveSynthModA(diff);
        } else if (m == 1) {
          handleEncoderSubtractiveSynthModB(diff);
        } else if (m == 2) {
          handleEncoderSubtractiveSynthModC(diff);
        } else if (m == 3) {
          handleEncoderSubtractiveSynthModD(diff);
        }
      } else if (currTrack.track_type == DEXED) {
        if (m == 0) {
          handleEncoderDexedModA(diff);
        } else if (m == 1) {
          handleEncoderDexedModB(diff);
        } else if (m == 2) {
          handleEncoderDexedModC(diff);
        } else if (m == 3) {
          handleEncoderDexedModD(diff);
        }
      } else if (currTrack.track_type == RAW_SAMPLE) {
        if (m == 0) {
          handleEncoderRawSampleModA(diff);
        } else if (m == 1) {
          handleEncoderRawSampleModB(diff);
        } else if (m == 2) {
          handleEncoderRawSampleModC(diff);
        } else if (m == 3) {
          handleEncoderRawSampleModD(diff);
        }
      } else if (currTrack.track_type == WAV_SAMPLE) {
        if (m == 0) {
          handleEncoderWavSampleModA(diff);
        } else if (m == 1) {
          handleEncoderWavSampleModB(diff);
        } else if (m == 2) {
          handleEncoderWavSampleModC(diff);
        } else if (m == 3) {
          handleEncoderWavSampleModD(diff);
        }
      } else if (currTrack.track_type == CV_GATE) {
        if (m == 0) {
          handleEncoderCvGateModA(diff);
        } else if (m == 1) {
          handleEncoderCvGateModB(diff);
        } else if (m == 2) {
          handleEncoderCvGateModC(diff);
        } else if (m == 3) {
          handleEncoderCvGateModD(diff);
        }
      }

      encoder_lastValues[modEncoderIdx] = encoder_currValues[modEncoderIdx];
    }
  }
}

void handleEncoderTraversePages(void)
{
  int main_encoder_idx = 0;
  encoder_currValues[main_encoder_idx] = encoder_getValue(encoder_addrs[main_encoder_idx]);

  if(encoder_currValues[main_encoder_idx] != encoder_lastValues[main_encoder_idx]) {
    int diff = encoder_currValues[main_encoder_idx] - encoder_lastValues[main_encoder_idx];
    int newPage = current_page_selected + diff;

    if (newPage != current_page_selected) {
      TRACK currTrack = getHeapCurrentSelectedTrack();
      int maxPagesForCurrTrack = trackPageNumMap[currTrack.track_type];

      if (maxPagesForCurrTrack == 1) {
        return;
      }

      if (newPage > (maxPagesForCurrTrack-1)) {
        newPage = 0;
      } else if (newPage < 0) {
        newPage = maxPagesForCurrTrack-1;
      }

      current_page_selected = newPage;

      drawSequencerScreen(false);
    }

    encoder_lastValues[main_encoder_idx] = encoder_currValues[main_encoder_idx];
  }
}

void handleEncoderSetPatternMods(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  const int modCount = 4;
  for (int m = 0; m < modCount; m++)
  {
    int modEncoderIdx = m+1;

    encoder_currValues[modEncoderIdx] = encoder_getValue(encoder_addrs[modEncoderIdx]);

    if(encoder_currValues[modEncoderIdx] != encoder_lastValues[modEncoderIdx]) {
      int diff = encoder_currValues[modEncoderIdx] - encoder_lastValues[modEncoderIdx];

      if (m == 0) {
          handleEncoderPatternModA(diff);
        } else if (m == 1) {
          handleEncoderPatternModB(diff);
        } else if (m == 2) {
          handleEncoderPatternModC(diff);
        } else if (m == 3) {
          handleEncoderPatternModD(diff);
        }

      encoder_lastValues[modEncoderIdx] = encoder_currValues[modEncoderIdx];
    }
  }
}

void handleEncoderStates() {
  if (!project_initialized) {
    // Serial.println("Not handling encoder states, project is not initialized!");
    return;
  }

  // slow this down from being called every loop when in set tempo mode
  if (!(elapsed % 25) && current_UI_mode == SET_TEMPO) {
    handleEncoderSetTempo();
  } else if (!(elapsed % 25) && current_UI_mode == PATTERN_WRITE) {
    handleEncoderSetPatternMods();
  } else if (!(elapsed % 25) && (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE)) {
    if (current_UI_mode == TRACK_WRITE) {
      handleEncoderTraversePages();
    }

    handleEncoderSetTrackMods();
  }
}

void handleKeyboardSetRatchets(void)
{
  if (ratcheting_track == -1) {
    ratchet_division = -1;
    return;
  }

  if (ratchetReleaseTime != 0 && ((elapsed - ratchetReleaseTime) >= 25)) {
    ratchetReleaseTime = 0;
    ratchet_division = -1;
  }

  // Get the currently touched pads
  currtouched = cap.touched();

  int8_t invertedNoteNumber = -1;

  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); 
      Serial.println(" touched");
      
      invertedNoteNumber = backwardsNoteNumbers[i];
      ratchet_division = keyed_ratchet_divisions[invertedNoteNumber];

      if (ratchets_held < 3) ++ratchets_held;

      drawSequencerScreen(false);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i);
      Serial.println(" released");
      
      if (ratchets_held > 0) --ratchets_held;

      Serial.print("ratchets_held");
      Serial.println(ratchets_held);

      if (ratchets_held == 0) ratchetReleaseTime = elapsed;

      drawSequencerScreen(false);
    }
  }

  if (!fastBtnPressed && fastTouchRead(32) >= 64) {
    std::string touchedStr = "Touched key: 12";
    Serial.println(touchedStr.c_str());

    fastBtnPressed = true;
    invertedNoteNumber = backwardsNoteNumbers[12];
    ratchet_division = keyed_ratchet_divisions[invertedNoteNumber];

    if (ratchets_held < 3) ++ratchets_held;

    drawSequencerScreen(false);

  } else if (fastBtnPressed && fastTouchRead(32) < 64) {
    std::string releasedStr = "Released key: 12";
    Serial.println(releasedStr.c_str());

    invertedNoteNumber = backwardsNoteNumbers[12];

    fastBtnPressed = false;
    if (ratchets_held > 0) --ratchets_held;

      Serial.print("ratchets_held");
      Serial.println(ratchets_held);
      
      if (ratchets_held == 0) ratchetReleaseTime = elapsed;

    drawSequencerScreen(false);
  }

  // reset our state
  lasttouched = currtouched;
}

void handleKeyboardStates(void) {
  if (!project_initialized) {
    return;
  }

  // TODO: remove
  if (_queued_pattern.bank > -1 || _queued_pattern.number > -1) {
    return;
  }

  if (current_UI_mode == PERFORM_RATCHET) {
    handleKeyboardSetRatchets();
    return;
  }

  // Get the currently touched pads
  currtouched = cap.touched();

  int8_t invertedNoteNumber = -1;
  bool released = false;

  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      invertedNoteNumber = backwardsNoteNumbers[i];

      note_on_keyboard = invertedNoteNumber;
      if (keyboardNotesHeld < 5) keyboardNotesHeld++;

      Serial.print(i); Serial.println(" touched");

      if (current_UI_mode != SUBMITTING_STEP_VALUE) {
        triggerTrackManually(current_selected_track, invertedNoteNumber);
      }
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      invertedNoteNumber = backwardsNoteNumbers[i]; // need this?

      if (keyboardNotesHeld > 0) keyboardNotesHeld--;

      Serial.print(i);
      Serial.println(" released");

      released = true;

      if (current_UI_mode != SUBMITTING_STEP_VALUE) {
        TRACK currSelTrack = getHeapCurrentSelectedTrack();

        if (keyboardNotesHeld == 0) {
          if (current_selected_track > 3) {
            sampleVoices[current_selected_track-4].ampEnv.noteOff();
          } else {
            comboVoices[current_selected_track].ampEnv.noteOff();
            if (currSelTrack.track_type == TRACK_TYPE::SUBTRACTIVE_SYNTH) {
              comboVoices[current_selected_track].filterEnv.noteOff();
            }
          }

          if (currSelTrack.track_type == MIDI_OUT) {
            MIDI.sendNoteOff(64, 100, 1);
          } else if (currSelTrack.track_type == CV_GATE) {
            if (currSelTrack.channel == 1) {
              //writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
              writeToDAC(CS1, 1, 0); // gate
            } else if (currSelTrack.channel == 2)  {
              //writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
              writeToDAC(CS2, 1, 0); // gate
            } else if (currSelTrack.channel == 3) {
              //writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
              writeToDAC(CS3, 1, 0); // gate
            } else if (currSelTrack.channel == 4)  {
              //writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
              writeToDAC(CS4, 1, 0); // gate
            }
          } 
        }
      }
    }
  }

  if (!fastBtnPressed && fastTouchRead(32) >= 64) {
    invertedNoteNumber = backwardsNoteNumbers[12];
    fastBtnPressed = true;

    note_on_keyboard = invertedNoteNumber;

    if (keyboardNotesHeld < 5) keyboardNotesHeld++;

    std::string touchedStr = "Touched key: 12";
    Serial.println(touchedStr.c_str());

    if (current_UI_mode != SUBMITTING_STEP_VALUE) {
      triggerTrackManually(current_selected_track, invertedNoteNumber);
    }

  } else if (fastBtnPressed && fastTouchRead(32) < 64) {
    invertedNoteNumber = backwardsNoteNumbers[12];

    fastBtnPressed = false;
    released = true;
  
    note_on_keyboard = invertedNoteNumber;

    if (keyboardNotesHeld > 0) keyboardNotesHeld--;

    std::string releasedStr = "Released key: 12";
    Serial.println(releasedStr.c_str());
  }

  // released a key button, so modify a step's note number if also holding a step button
  if (released && invertedNoteNumber > -1) {
    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::NOTE] = true;
      _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].note = invertedNoteNumber;
      _pattern_mods_mem.tracks[current_selected_track].step_mod_flags[current_selected_step].flags[MOD_ATTRS::OCTAVE] = true;
      _pattern_mods_mem.tracks[current_selected_track].steps[current_selected_step].octave = keyboardOctave;

      drawSequencerScreen(false);
    } else if (current_UI_mode == TRACK_SEL) {
      _seq_heap.pattern.tracks[current_selected_track].note = invertedNoteNumber;
      _seq_heap.pattern.tracks[current_selected_track].octave = keyboardOctave;

      drawSequencerScreen(false);
    } else {
      TRACK currSelTrack = getHeapCurrentSelectedTrack();

      if (keyboardNotesHeld == 0) {
        if (current_selected_track > 3) {
          sampleVoices[current_selected_track-4].ampEnv.noteOff();
        } else {
          comboVoices[current_selected_track].ampEnv.noteOff();
          if (currSelTrack.track_type == TRACK_TYPE::SUBTRACTIVE_SYNTH) {
            comboVoices[current_selected_track].filterEnv.noteOff();
          }
        }

        if (currSelTrack.track_type == MIDI_OUT) {
          MIDI.sendNoteOff(64, 100, 1);
        } else if (currSelTrack.track_type == CV_GATE) {
          if (currSelTrack.channel == 1) {
            //writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
            writeToDAC(CS1, 1, 0); // gate
          } else if (currSelTrack.channel == 2)  {
            //writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
            writeToDAC(CS2, 1, 0); // gate
          } else if (currSelTrack.channel == 3) {
            //writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
            writeToDAC(CS3, 1, 0); // gate
          } else if (currSelTrack.channel == 4)  {
            //writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
            writeToDAC(CS4, 1, 0); // gate
          }
        } 
      }
    }
  }

  // reset our state
  lasttouched = currtouched;
}

bool copy_btn_held = false;

void handleSwitchStates(bool discard) {
  // Fills kpd.key[ ] array with up-to 10 active keys.
  // Returns true if there are ANY active keys.
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED: 
            {
              Serial.print("button pressed: ");
              Serial.println(kpd.key[i].kchar);

              if (current_UI_mode != PROJECT_INITIALIZE  && current_UI_mode != PROJECT_BUSY && !project_initialized) {
                drawErrorMessage("PROJECT NOT INITIALIZED!");

                return;
              } else if (current_UI_mode == PROJECT_INITIALIZE && !project_initialized) {
                if (kpd.key[i].kchar == SELECT_BTN_CHAR) { // select
                  initProject();
                }
              } else if (current_UI_mode == PROJECT_BUSY && !project_initialized) {
                return;
              }

              if (!function_started) {
                // track select
                if ((current_UI_mode != COPY_SEL) && kpd.key[i].kchar == 'c') {
                  Serial.println("enter track select mode!");
                  current_UI_mode = TRACK_SEL;

                  Serial.printf("pattern_copy_available %d track_copy_available %d step_copy_available %d", pattern_copy_available, track_copy_available, step_copy_available);
                  Serial.println(" -- Resetting copy flags if we've left a copy mode!");

                  pattern_copy_available = false;
                  track_copy_available = false;
                  step_copy_available = false;

                  clearAllStepLEDs();
                  displayCurrentlySelectedTrack();
                } else if (current_UI_mode == TRACK_SEL && btnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selTrack = getKeyStepNum(kpd.key[i].kchar)-1; // zero-based
                  current_selected_track = selTrack;

                  // initialize track
                  _seq_heap.pattern.tracks[current_selected_track].initialized = true;

                  Serial.print("marking pressed track selection (zero-based): ");
                  Serial.println(selTrack);

                  track_held_for_selection = selTrack;

                  previous_UI_mode = TRACK_SEL;
                  current_UI_mode = TRACK_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedTrack();
                } else if (current_UI_mode == TRACK_SEL && kpd.key[i].kchar == SOUND_BTN_CHAR) {
                  TRACK_TYPE currType = _seq_heap.pattern.tracks[current_selected_track].track_type;
                  
                  Serial.print("changing ");
                  Serial.print(current_selected_track+1);
                  Serial.print(" track sound for pattern ");
                  Serial.print(current_selected_pattern+1);
                  Serial.print(" to sound type: ");
                  Serial.println(currType);

                  TRACK_TYPE newType = current_selected_track > 3 ? RAW_SAMPLE : SUBTRACTIVE_SYNTH;
                  if (currType == SUBTRACTIVE_SYNTH) {
                    newType = RAW_SAMPLE;
                  } else if (currType == RAW_SAMPLE) {
                    newType = DEXED;
                  } else if (currType == DEXED) {
                    newType = WAV_SAMPLE;
                  } else if (currType == WAV_SAMPLE) {
                    newType = MIDI_OUT;
                  } else if (currType == MIDI_OUT) {
                    newType = CV_GATE;
                  } else if (currType == CV_GATE) {
                    newType = CV_TRIG;
                  }

                  // reset page to 0
                  current_page_selected = 0;

                  changeTrackSoundType(current_selected_track, newType);

                  drawSequencerScreen(false);
                }

                // pattern select
                else if (!(current_UI_mode == COPY_SEL) && kpd.key[i].kchar == PATTERN_BTN_CHAR) {
                  Serial.println("enter pattern select mode!");
                  current_UI_mode = PATTERN_SEL;

                  Serial.printf("pattern_copy_available %d track_copy_available %d step_copy_available %d", pattern_copy_available, track_copy_available, step_copy_available);
                  Serial.println(" -- Resetting copy flags if we've left a copy mode!");

                  pattern_copy_available = false;
                  track_copy_available = false;
                  step_copy_available = false;

                  clearAllStepLEDs();
                  displayCurrentlySelectedPattern();
                } else if (current_UI_mode == PATTERN_SEL && btnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selPattern = getKeyStepNum(kpd.key[i].kchar)-1; // zero-based

                  if (_seq_state.playback_state == RUNNING) {
                    _queued_pattern.bank = current_selected_bank;
                    _queued_pattern.number = selPattern;

                    previous_UI_mode = PATTERN_SEL;
                    current_UI_mode = PATTERN_WRITE;
                  } else {
                    swapSequencerMemoryForPattern(current_selected_bank, selPattern);

                    Serial.print("marking pressed pattern selection (zero-based): ");
                    Serial.println(selPattern);

                    patt_held_for_selection = selPattern;

                    previous_UI_mode = PATTERN_SEL;
                    current_UI_mode = PATTERN_SEL;

                    clearAllStepLEDs();
                    displayCurrentlySelectedPattern();
                  }
                }

                // paste
                else if (current_UI_mode == COPY_STEP && step_copy_available && (btnCharIsATrack(kpd.key[i].kchar) || kpd.key[i].kchar == ESCAPE_BTN_CHAR)) { // paste step
                  if (kpd.key[i].kchar == ESCAPE_BTN_CHAR) {
                    step_copy_available = false;
                    current_UI_mode = previous_UI_mode;
                    clearAllStepLEDs();
                    if (current_UI_mode == TRACK_WRITE) setDisplayStateForAllStepLEDs();
                    drawSequencerScreen();
                    return;
                  }

                  uint8_t stepToUse = getKeyStepNum(kpd.key[i].kchar);

                  if (current_step_page == 2) {
                    stepToUse += 16;
                  } else if (current_step_page == 3) {
                    stepToUse += 32;
                  } else if (current_step_page == 4) {
                    stepToUse += 48;
                  }

                  // save pasted step to extmem and reload the current pattern into heap
                  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern].tracks[current_selected_track].steps[stepToUse-1] = _step_copy_buf;
                  _seq_heap.pattern = _seq_external.banks[current_selected_bank].patterns[current_selected_pattern];
                  
                  step_copy_available = false;
                  current_UI_mode = PASTE_STEP;

                  Serial.println("drawing paste step confirm overlay!");
                  drawPasteConfirmOverlay("STEP", stepToUse);
                } else if (current_UI_mode == COPY_PATTERN && pattern_copy_available && (btnCharIsATrack(kpd.key[i].kchar) || kpd.key[i].kchar == ESCAPE_BTN_CHAR)) { // paste pattern
                  if (kpd.key[i].kchar == ESCAPE_BTN_CHAR) {
                    pattern_copy_available = false;
                    current_UI_mode = previous_UI_mode;
                    clearAllStepLEDs();
                    if (current_UI_mode == TRACK_WRITE) setDisplayStateForAllStepLEDs();
                    drawSequencerScreen();
                    return;
                  }

                  Serial.println("pasting selected pattern to target pattern!");

                  uint8_t targetPattern = getKeyStepNum(kpd.key[i].kchar)-1;

                  // save pasted pattern to extmem
                  _seq_external.banks[current_selected_bank].patterns[targetPattern] = _pattern_copy_buf;

                  // and reload the current pattern into heap if target pattern is current selected pattern
                  if (current_selected_pattern == targetPattern) {
                    _seq_heap.pattern = _seq_external.banks[current_selected_bank].patterns[current_selected_pattern];
                  }
                  
                  pattern_copy_available = false;
                  current_UI_mode = PASTE_PATTERN;

                  Serial.println("drawing paste pattern confirm overlay!");
                  drawPasteConfirmOverlay("PATTERN", getKeyStepNum(kpd.key[i].kchar));

                  // transition to target pattern as well?
                  if (_seq_state.playback_state == RUNNING) {
                    _queued_pattern.bank = current_selected_bank;
                    _queued_pattern.number = targetPattern;

                    // previous_UI_mode = PATTERN_SEL;
                    // current_UI_mode = PATTERN_WRITE;
                  } else {
                    current_selected_pattern = targetPattern;
                    swapSequencerMemoryForPattern(current_selected_bank, targetPattern); // TODO: rework this to enable pasting patterns across banks too
                  }
                } else if (current_UI_mode == COPY_TRACK && track_copy_available && (btnCharIsATrack(kpd.key[i].kchar) || kpd.key[i].kchar == ESCAPE_BTN_CHAR)) { // paste track
                  if (kpd.key[i].kchar == ESCAPE_BTN_CHAR) {
                    track_copy_available = false;
                    current_UI_mode = previous_UI_mode;
                    clearAllStepLEDs();
                    if (current_UI_mode == TRACK_WRITE) setDisplayStateForAllStepLEDs();
                    drawSequencerScreen();
                    return;
                  }

                  Serial.println("pasting selected track to target track!");

                  uint8_t targetTrack = getKeyStepNum(kpd.key[i].kchar)-1;

                  _seq_heap.pattern.tracks[targetTrack] = _track_copy_buf;
                  
                  track_copy_available = false;
                  current_UI_mode = PASTE_TRACK;

                  Serial.println("drawing paste track confirm overlay!");
                  drawPasteConfirmOverlay("TRACK", getKeyStepNum(kpd.key[i].kchar));

                  // transition to target track as well?
                  current_selected_track = targetTrack;
                }

                // copy
                else if (current_UI_mode == COPY_SEL && (!step_copy_available && !track_copy_available && !pattern_copy_available) && (btnCharIsATrack(kpd.key[i].kchar) || kpd.key[i].kchar == PATTERN_BTN_CHAR || kpd.key[i].kchar == TRACK_BTN_CHAR)) {
                  if (!step_copy_available && btnCharIsATrack(kpd.key[i].kchar)) { // copy step
                    current_UI_mode = COPY_STEP;

                    uint8_t stepToUse = getKeyStepNum(kpd.key[i].kchar);
                    if (current_step_page == 2) {
                      stepToUse += 16;
                    } else if (current_step_page == 3) {
                      stepToUse += 32;
                    } else if (current_step_page == 4) {
                      stepToUse += 48;
                    }

                    _step_copy_buf = _seq_heap.pattern.tracks[current_selected_track].steps[stepToUse-1];
                    
                    step_copy_available = true;

                    Serial.println("drawing copy step confirm overlay!");
                    drawCopyConfirmOverlay("STEP", stepToUse);

                  } else if (kpd.key[i].kchar == PATTERN_BTN_CHAR) {
                    current_UI_mode = COPY_PATTERN;

                    Serial.println("drawing copy pattern sel overlay!");
                    drawCopySelOverlay("PATTERN");

                    displayInitializedPatternLEDs();
                  } else if (kpd.key[i].kchar == TRACK_BTN_CHAR) {
                    current_UI_mode = COPY_TRACK;

                    Serial.println("drawing copy track sel overlay!");
                    drawCopySelOverlay("TRACK");

                    displayInitializedTrackLEDs();
                  }
                } else if (current_UI_mode == COPY_PATTERN && !pattern_copy_available && btnCharIsATrack(kpd.key[i].kchar)) {
                  Serial.print("write selected pattern: ");
                  Serial.print(getKeyStepNum(kpd.key[i].kchar));
                  Serial.println(" to copy buffer");

                  // make sure if copying current pattern that the latest state exists in extmem
                  if (current_selected_pattern == getKeyStepNum(kpd.key[i].kchar)-1) {
                    _seq_external.banks[current_selected_bank].patterns[current_selected_pattern] = _seq_heap.pattern;
                  }

                  _pattern_copy_buf = _seq_external.banks[current_selected_bank].patterns[getKeyStepNum(kpd.key[i].kchar)-1];
                  
                  pattern_copy_available = true;

                  drawCopyConfirmOverlay("PATTERN", getKeyStepNum(kpd.key[i].kchar));
                } else if (current_UI_mode == COPY_TRACK && !track_copy_available && btnCharIsATrack(kpd.key[i].kchar)) {
                  Serial.print("write selected track: ");
                  Serial.print(getKeyStepNum(kpd.key[i].kchar));
                  Serial.println(" to copy buffer");

                  _track_copy_buf = _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1];
                  
                  track_copy_available = true;

                  drawCopyConfirmOverlay("TRACK", getKeyStepNum(kpd.key[i].kchar));
                }

                // start/pause or stop
                else if (kpd.key[i].kchar == 'q' || kpd.key[i].kchar == 'w') {
                  toggleSequencerPlayback(kpd.key[i].kchar);
                  drawSequencerScreen(false);
                }

                // page
                else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                  Serial.print("btn: ");
                  Serial.println(kpd.key[i].kchar);

                  TRACK currTrack = getHeapCurrentSelectedTrack();

                  int maxPages = 1; 
                  if (currTrack.last_step > 16 && currTrack.last_step <= 32) {
                    maxPages = 2;
                  } else if (currTrack.last_step > 32 && currTrack.last_step <= 48) {
                    maxPages = 3;
                  } else if (currTrack.last_step > 48) {
                    maxPages = 4;
                  }

                  if (kpd.key[i].kchar == '3') {
                    current_step_page = min(maxPages, current_step_page+1);

                  } else if (kpd.key[i].kchar == '9') {
                    current_step_page = max(1, current_step_page-1);
                  }

                  displayPageLEDs(-1);
                  setDisplayStateForAllStepLEDs();

                  Serial.print("Updated page to: ");
                  Serial.println(current_step_page);
                }

                // perform
                else if (kpd.key[i].kchar == 'a') {
                  Serial.println("enter perform select mode!");
                  current_UI_mode = PERFORM_SEL;

                  Serial.printf("pattern_copy_available %s track_copy_available %s step_copy_available %s", pattern_copy_available, track_copy_available, step_copy_available);
                  Serial.println(" -- Resetting copy flags if we've left a copy mode!");

                  pattern_copy_available = false;
                  track_copy_available = false;
                  step_copy_available = false;

                  clearAllStepLEDs();
                  displayPerformModeLEDs();
                } else if (current_UI_mode == PERFORM_SEL && btnCharIsATrack(kpd.key[i].kchar) && getKeyStepNum(kpd.key[i].kchar) >= 13) {
                  perf_held_for_selection = getKeyStepNum(kpd.key[i].kchar)-1;

                  Serial.print("perf_held_for_selection: ");
                  Serial.println(perf_held_for_selection);

                  if (getKeyStepNum(kpd.key[i].kchar) == 13) { // enable tap mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_TAP;

                    clearAllStepLEDs();
                    drawSequencerScreen(false);
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 14) { // enable mute mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_MUTE;

                    displayMuteLEDs();
                    drawSequencerScreen(false);
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 15) { // enable solo mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_SOLO;

                    //displayMuteLEDs();
                    clearAllStepLEDs();
                    drawSequencerScreen(false);
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 16) { // enable ratchet mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_RATCHET;

                    //displayMuteLEDs();
                    clearAllStepLEDs();
                    drawSequencerScreen(false);
                  }
                } else if (current_UI_mode == PERFORM_TAP && btnCharIsATrack(kpd.key[i].kchar)) { // handle taps
                  triggerTrackManually(getKeyStepNum(kpd.key[i].kchar)-1, _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].note);
                } else if (current_UI_mode == PERFORM_MUTE && btnCharIsATrack(kpd.key[i].kchar)) { // handle mutes
                  bool currMuteState = _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].muted;
                  _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].muted = (currMuteState ? false : true);

                  displayMuteLEDs();
                } else if (current_UI_mode == PERFORM_SOLO && btnCharIsATrack(kpd.key[i].kchar)) { // handle solos
                  bool undoSoloing = _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].soloing;
                  
                  handleSoloForTrack(getKeyStepNum(kpd.key[i].kchar)-1, undoSoloing);
                }
              }
              // function handling
              else {
                Serial.println("triggering function!");
                // change track sound type
                if ((current_UI_mode == TRACK_WRITE || current_UI_mode == PATTERN_WRITE) && kpd.key[i].kchar == SOUND_BTN_CHAR) {
                  Serial.println("draw setup screen!");

                  previous_UI_mode = current_UI_mode;
                  current_UI_mode = CHANGE_SETUP;

                  drawSetupScreen();
                } else if ((current_UI_mode == TRACK_WRITE || current_UI_mode == PATTERN_WRITE) && kpd.key[i].kchar == 'b') {
                  previous_UI_mode = current_UI_mode;
                  current_UI_mode = BANK_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedBank();
                } else if (current_UI_mode == BANK_SEL && btnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selBank = getKeyStepNum(kpd.key[i].kchar)-1; // zero-based

                  if (_seq_state.playback_state == RUNNING) {
                    _queued_pattern.bank = selBank;
                    _queued_pattern.number = current_selected_pattern;

                    current_UI_mode = previous_UI_mode;
                    previous_UI_mode = BANK_SEL;
                  } else {
                    swapSequencerMemoryForPattern(selBank, current_selected_pattern);

                    Serial.print("marking pressed bank selection (zero-based): ");
                    Serial.println(selBank);

                    bnk_held_for_selection = selBank;

                    previous_UI_mode = BANK_SEL;
                    current_UI_mode = BANK_SEL;

                    clearAllStepLEDs();
                    displayCurrentlySelectedBank();
                  }
                } else if (current_UI_mode == TRACK_WRITE && kpd.key[i].kchar == 'c') {
                  // TODO: impl AB layer switching mechanic
                }

                // octave
                else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                  Serial.print("btn: ");
                  Serial.println(kpd.key[i].kchar);

                  // 1-7 octave range e.g. [1,2,3,4,5,6,7]
                  // LEDs 17-20
                  if (kpd.key[i].kchar == '3') {
                    keyboardOctave = min(7, keyboardOctave+1);

                  } else if (kpd.key[i].kchar == '9') {
                    keyboardOctave = max(1, keyboardOctave-1);
                  }

                  displayCurrentOctaveLEDs();
                  Serial.print("Updated octave to: ");
                  Serial.println(keyboardOctave);
                }
              }

              break;
            }
          case HOLD:
            {
              if (!project_initialized) {
                return;
              }

              bool allowedModeToParamLockFrom = (current_UI_mode == TRACK_WRITE);
              bool allowedModeToCopySelectFrom = (current_UI_mode == PATTERN_WRITE || current_UI_mode == TRACK_WRITE);
              bool allowedModeToSetTempoFrom = (
                current_UI_mode == PATTERN_WRITE || 
                current_UI_mode == TRACK_WRITE || 
                current_UI_mode == PERFORM_TAP ||
                current_UI_mode == PERFORM_MUTE ||
                current_UI_mode == PERFORM_SOLO ||
                current_UI_mode == PERFORM_RATCHET
              );

              // param lock step
              if (allowedModeToParamLockFrom && btnCharIsATrack(kpd.key[i].kchar)) {
                // editing a step value / parameter locking this step
                previous_UI_mode = TRACK_WRITE;
                current_UI_mode = SUBMITTING_STEP_VALUE;

                uint8_t stepToUse = getKeyStepNum(kpd.key[i].kchar);
                if (current_step_page == 2) {
                  stepToUse += 16;
                } else if (current_step_page == 3) {
                  stepToUse += 32;
                } else if (current_step_page == 4) {
                  stepToUse += 48;
                }

                current_selected_step = stepToUse-1;

                TRACK_STEP heldStep = _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];

                // only toggle held step ON if initially in the OFF position,
                // so that holding / param locking doesn't turn the step off
                if (heldStep.state == OFF) {
                  Serial.println("toggling held step!");    

                  uint8_t stepToToggle = getKeyStepNum(kpd.key[i].kchar);

                  toggleSelectedStep(stepToToggle);
                  setDisplayStateForAllStepLEDs(); 
                }

                drawSequencerScreen(false);
              }

              // copy
              else if (allowedModeToCopySelectFrom && kpd.key[i].kchar == COPY_BTN_CHAR) {
                Serial.print("current mode: ");
                Serial.println(current_UI_mode);

                Serial.println("enter copy select mode!");

                if (current_UI_mode != COPY_SEL) {
                  previous_UI_mode = current_UI_mode;
                }

                current_UI_mode = COPY_SEL;
              }

              // tempo set
              else if (allowedModeToSetTempoFrom && kpd.key[i].kchar == TEMPO_BTN_CHAR) {
                Serial.println("enter tempo set mode!");
                current_UI_mode = SET_TEMPO;

                drawSetTempoOverlay();
              }

              // perform ratchet
              else if (current_UI_mode == PERFORM_RATCHET && btnCharIsATrack(kpd.key[i].kchar)) { // handle ratchets
                if (_seq_state.playback_state == RUNNING) {
                  ratcheting_track = getKeyStepNum(kpd.key[i].kchar)-1;
                  drawSequencerScreen(false);
                }
              }

              // function
              else if (kpd.key[i].kchar == FUNCTION_BTN_CHAR) {
                Serial.println("starting function!");
                function_started = true;

                displayCurrentOctaveLEDs();
              }

              break;
            }
          case RELEASED:
            {
              if (!project_initialized) {
                return;
              }

              // track select / write release
              if (current_UI_mode == TRACK_SEL && kpd.key[i].kchar == 'c' && track_held_for_selection == -1) {      
                current_UI_mode = TRACK_WRITE; // force track write mode when leaving track / track select action
                previous_UI_mode = TRACK_WRITE;

                displayPageLEDs(-1);
                setDisplayStateForAllStepLEDs();
                drawSequencerScreen(false);
              } else if (current_UI_mode == TRACK_SEL && btnCharIsATrack(kpd.key[i].kchar) && ((getKeyStepNum(kpd.key[i].kchar)-1) == track_held_for_selection)) {   
                Serial.println("unmarking track as held for selection!");   

                current_selected_track = getKeyStepNum(kpd.key[i].kchar)-1;
                track_held_for_selection = -1;
                clearAllStepLEDs();

                current_page_selected = 0;

                current_UI_mode = TRACK_WRITE; // force track write mode when leaving track / track select action
                previous_UI_mode = TRACK_WRITE;

                displayPageLEDs(-1);
                setDisplayStateForAllStepLEDs();
                drawSequencerScreen(false);
              } else if (!function_started && !copy_btn_held && current_UI_mode == TRACK_WRITE && btnCharIsATrack(kpd.key[i].kchar) && track_held_for_selection == -1) {   
                Serial.println("toggling step!");         
                // toggle the step state on release
                uint8_t stepToToggle = getKeyStepNum(kpd.key[i].kchar);

                toggleSelectedStep(stepToToggle);
                setDisplayStateForAllStepLEDs();
              } 

              // pattern select / write release              
              else if (current_UI_mode == PATTERN_SEL && kpd.key[i].kchar == 'b' && patt_held_for_selection == -1) {      
                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving patt / patt select action
                previous_UI_mode = PATTERN_WRITE;

                clearPageLEDs();
                clearAllStepLEDs();
                drawSequencerScreen(false);
              } else if (current_UI_mode == PATTERN_SEL && btnCharIsATrack(kpd.key[i].kchar) && ((getKeyStepNum(kpd.key[i].kchar)-1) == patt_held_for_selection)) {   
                Serial.println("unmarking pattern as held for selection!");   

                current_selected_pattern = getKeyStepNum(kpd.key[i].kchar)-1;
                patt_held_for_selection = -1;
                clearAllStepLEDs();

                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving patt / patt select action
                previous_UI_mode = PATTERN_WRITE;

                drawSequencerScreen(false);
              }

              // bank select release              
              else if (current_UI_mode == BANK_SEL && kpd.key[i].kchar == 'b' && bnk_held_for_selection == -1) {      
                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving bank / bank select action
                previous_UI_mode = PATTERN_WRITE;

                clearPageLEDs();
                clearAllStepLEDs();
                drawSequencerScreen(false);
              } else if (current_UI_mode == BANK_SEL && btnCharIsATrack(kpd.key[i].kchar) && ((getKeyStepNum(kpd.key[i].kchar)-1) == bnk_held_for_selection)) {   
                Serial.println("unmarking bank as held for selection!");   

                current_selected_bank = getKeyStepNum(kpd.key[i].kchar)-1;
                bnk_held_for_selection = -1;
                clearAllStepLEDs();

                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving bank / bank select action
                previous_UI_mode = PATTERN_WRITE;

                drawSequencerScreen(false);
              }

              // copy release
              else if (current_UI_mode == COPY_SEL && kpd.key[i].kchar == COPY_BTN_CHAR) {
                Serial.print(" reverting copy mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                drawSequencerScreen(false);
              } 
              
              else if (btnCharIsATrack(kpd.key[i].kchar) && (current_UI_mode == COPY_PATTERN || current_UI_mode == COPY_TRACK || current_UI_mode == COPY_STEP)) {
                if (current_UI_mode == COPY_PATTERN && pattern_copy_available) {
                  Serial.println("try to apply copied pattern now!");
                } else if (current_UI_mode == COPY_TRACK && track_copy_available) {
                  Serial.println("try to apply copied track now!");
                } else if (current_UI_mode == COPY_STEP && step_copy_available) {
                  Serial.println("try to apply copied step now!");
                }
              } 
              
              else if (btnCharIsATrack(kpd.key[i].kchar) && current_UI_mode == PASTE_STEP) {
                Serial.print(" reverting paste step confirm mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                clearAllStepLEDs();
                if (current_UI_mode == TRACK_WRITE) {
                  setDisplayStateForAllStepLEDs();
                }
                drawSequencerScreen(false);
              } else if ((btnCharIsATrack(kpd.key[i].kchar)) && (current_UI_mode == PASTE_PATTERN || current_UI_mode == PASTE_TRACK)) {
                Serial.print(" reverting paste pattern/track confirm mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                clearAllStepLEDs();
                if (current_UI_mode == TRACK_WRITE) {
                  setDisplayStateForAllStepLEDs();
                }
                drawSequencerScreen(false);
              }
              
              // tempo set release
              else if (current_UI_mode == SET_TEMPO) {
                Serial.print(" reverting set tempo mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                drawSequencerScreen(false);
              }

              // set octave release
              else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                // current_UI_mode = previous_UI_mode;
                // function_started = false;
                
                // revert LEDs to page display
                // displayPageLEDs();
              }

              // setup screen release
              else if ( current_UI_mode == CHANGE_SETUP) {
                if (kpd.key[i].kchar == SELECT_BTN_CHAR) { // select
                  saveProject();

                  auto leavingUI = current_UI_mode;
                  auto newUI = previous_UI_mode;

                  current_UI_mode = newUI;
                  previous_UI_mode = leavingUI;

                  Serial.println("leaving function!");

                  function_started = false;

                  drawSequencerScreen(false);
                } else if (kpd.key[i].kchar == ESCAPE_BTN_CHAR) { // esc
                  auto leavingUI = current_UI_mode;
                  auto newUI = previous_UI_mode;

                  current_UI_mode = newUI;
                  previous_UI_mode = leavingUI;

                  Serial.print("current UI: ");
                  Serial.println(current_UI_mode);

                  Serial.println("leaving function!");

                  function_started = false;

                  drawSequencerScreen(false);
                }
              }

              // param lock release
              else if (current_UI_mode == SUBMITTING_STEP_VALUE && btnCharIsATrack(kpd.key[i].kchar)) {
                Serial.print(" reverting submitting step value / param lock mode, re-entering: ");
                Serial.println(previous_UI_mode == TRACK_WRITE ? "track write mode!" : "??? mode!");

                // revert
                current_UI_mode = previous_UI_mode;
                previous_UI_mode = SUBMITTING_STEP_VALUE;
                current_selected_step = -1; // unselect the step

                drawSequencerScreen(false);
              }

              // perform mode select release
              else if (kpd.key[i].kchar == 'a' && perf_held_for_selection == -1) {
                // revert
                current_UI_mode = PATTERN_WRITE;
                previous_UI_mode = PERFORM_SEL;

                clearAllStepLEDs();
                drawSequencerScreen(false);
              } else if (current_UI_mode == PERFORM_SEL && (kpd.key[i].kchar == 'a' || kpd.key[i].kchar == 'w' || btnCharIsATrack(kpd.key[i].kchar)) && ((getKeyStepNum(kpd.key[i].kchar)-1) == perf_held_for_selection)) {
                Serial.println("reverting perf_held_for_selection");

                perf_held_for_selection = -1;
              } else if (current_UI_mode == PERFORM_TAP && btnCharIsATrack(kpd.key[i].kchar)) {
                Serial.println("handling note Off for tapped track");

                handleNoteOffForTrack(getKeyStepNum(kpd.key[i].kchar)-1);
              } else if (current_UI_mode == PERFORM_RATCHET && btnCharIsATrack(kpd.key[i].kchar)) {
                Serial.println("release track ratchet");

                ratcheting_track = -1;
                ratchet_division = -1;

                drawSequencerScreen(false);
              }

              // function stop
              else if (function_started && kpd.key[i].kchar == FUNCTION_BTN_CHAR) {
                Serial.println("leaving function!");
                function_started = false;

                if (current_UI_mode == TRACK_WRITE) {
                  displayPageLEDs(-1);
                }
              }

              break;
            }
          case IDLE:
            {
              //
              break;
            }
        }
      }
    }
  }
}

void encoder_set(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop) {
  Wire1.beginTransmission(addr);
  Wire1.write((uint8_t)(rval & 0xff)); Wire1.write((uint8_t)(rval >> 8));
  Wire1.write(0); Wire1.write(rloop);
  Wire1.write((uint8_t)(rmin & 0xff)); Wire1.write((uint8_t)(rmin >> 8));
  Wire1.write((uint8_t)(rmax & 0xff)); Wire1.write((uint8_t)(rmax >> 8));
  Wire1.write((uint8_t)(rstep & 0xff)); Wire1.write((uint8_t)(rstep >> 8));
  Wire1.endTransmission();
}

void encoder_setValue(int addr, int16_t rval) {
  Wire1.beginTransmission(addr);
  Wire1.write((uint8_t)(rval & 0xff)); Wire1.write((uint8_t)(rval >> 8));
  Wire1.endTransmission();
}

void initEncoders() {
  for (int i=0; i<5; i++) {
    encoder_set(encoder_addrs[i], -3000, 3000, 1, 0, 0);
  }
}

int16_t encoder_getValue(int addr) {
  Wire1.requestFrom(addr, 2);
  return((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
}

void u8g2_prepare(void) {
  u8g2.setFont(small_font);
  u8g2.setFontRefHeightAll();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void setLEDPWM (uint8_t lednum, uint16_t pwm)
{
    tlc.setPWM(lednum, pwm);
    tlc.write();  
}

void setLEDPWMDouble (uint8_t lednum1, uint16_t pwm1, uint8_t lednum2, uint16_t pwm2)
{
    tlc.setPWM(lednum1, pwm1);
    tlc.setPWM(lednum2, pwm2);
    tlc.write();  
}

void writeToDAC(int chip, int chan, int val){
  byte lVal = val;
  byte hVal = (val >> 8);

	SPI1.setDataMode(SPI_MODE0);
  digitalWrite(chip, LOW);

  if (val == 0x000) {
    // use low gain
    if(chan == 0){ 
      SPI1.transfer(hVal | chan_a_low_gain);
    }else{
      SPI1.transfer(hVal | chan_b_low_gain);
    }
  } else {
    // use high gain
    if(chan == 0){ 
      SPI1.transfer(hVal | chan_a_high_gain);
    }else{
      SPI1.transfer(hVal | chan_b_high_gain);
    }
  }

  SPI1.transfer(lVal);
  digitalWrite(chip, HIGH);
}

void initDACs()
{
  // initialize DAC outputs at 0V
  writeToDAC(CS1, 0, 0x000);
  writeToDAC(CS1, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS2, 0, 0x000);
  writeToDAC(CS2, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS3, 0, 0x000);
  writeToDAC(CS3, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS4, 0, 0x000);
  writeToDAC(CS4, 1, 0x000);
}

void handleHeadphoneAdjustment(void)
{
  if (analog.hasChanged()) {
    float newValue = (float)analog.getValue() / (float)1023;

    if (abs(hp_vol_cur - newValue) >= 0.05) {
      
      // Serial.print("hp_vol_cur: ");
      // Serial.println(hp_vol_cur);

      hp_vol_cur = newValue;

      // Serial.print("newValue: ");
      // Serial.println(newValue);

      sgtl5000_1.volume(hp_vol_cur);
    }
  }
}

void drawHatchedBackground() {    
  int leftBoundX = 0;
  int rightBoundX = 128;
  int topBoundX = 0;
  int bottomBoundX = 64;

  int pixelRows = 64;
  int pixelCols = 128;

  // initialize row template
  int t[128];
  int spacing = 4;

  // fill up template
  for (int i=0; i<128; i++) {
      if (i == 0 || (i % spacing == 0)) {
          t[i] = 1;
      } else {
          t[i] = 0;
      }
  }

  for (int r = 0; r < pixelRows; r++) {
    if (r != 0) {
      //Rotate the given array one time toward right    
      for(int i = 0; i < 1; i++){    
        int j;
        int last;  
          
        //Stores the last element of array    
        last = t[127];
        
        for(j = 127; j > 0; j--){
          //Shift element of array by one    
          t[j] = t[j-1];    
        }    
        //Last element of array will be added to the start of array.    
        t[0] = last;    
      } 
    }
    
    for (int c = 0; c < pixelCols; c++) {
      if (t[c] == 1) {
        u8g2.drawPixel(leftBoundX + c, topBoundX + r);
      }
    }
  }
}

void displayCurrentOctaveLEDs()
{
  switch (keyboardOctave)
  {
  case 1:
    setLEDPWM(17, 100);
    setLEDPWM(18, 25);
    setLEDPWM(19, 0);
    setLEDPWM(20, 0);
    break;
  
  case 2:
    setLEDPWM(17, 25);
    setLEDPWM(18, 25);
    setLEDPWM(19, 0);
    setLEDPWM(20, 0);
    break;
  
  case 3:
    Serial.println("here");
    setLEDPWM(17, 0);
    setLEDPWM(18, 25);
    setLEDPWM(19, 0);
    setLEDPWM(20, 0);
    break;
  
  case 4:
    setLEDPWM(17, 0);
    setLEDPWM(18, 0);
    setLEDPWM(19, 0);
    setLEDPWM(20, 0);
    break;
  
  case 5:
    setLEDPWM(17, 0);
    setLEDPWM(18, 0);
    setLEDPWM(19, 25);
    setLEDPWM(20, 0);
    break;
  
  case 6:
    setLEDPWM(17, 0);
    setLEDPWM(18, 0);
    setLEDPWM(19, 25);
    setLEDPWM(20, 25);
    break;
  
  case 7:
    setLEDPWM(17, 0);
    setLEDPWM(18, 0);
    setLEDPWM(19, 25);
    setLEDPWM(20, 100);
    break;
  
  default:
    break;
  }
}