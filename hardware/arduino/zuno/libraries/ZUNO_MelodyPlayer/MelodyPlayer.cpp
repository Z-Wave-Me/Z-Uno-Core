#include "MelodyPlayer.h"

//Array of notes frequencies (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
#define BASE_OCTAVE 5
word  NOTES_FREQ[] = { 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1720, 1865, 1976 };

//Enum of notes
enum {
//System "notes"
  note_tempo = 253,
  note_shift = 252,
  
  note_end = 254,
  note_parse_error = 255,
  note_pause = 0,

//Main notes
  note_c = 1,
  note_cis = 2,
  note_d = 3,
  note_dis = 4,
  note_e = 5,
  note_f = 6,
  note_fis = 7,
  note_g = 8,
  note_gis = 9,
  note_a = 10,
  note_ais = 11,
  note_b = 12
};

MelodyPlayer::MelodyPlayer(RenderedNote_t * playback){
    this->playback = playback;
    cursor = 0;
    playback_size = 0;
}
void MelodyPlayer::parseSymbol(){
    while(*melody_smbl == ' ')
        melody_smbl++;
    if(*melody_smbl == '|'){
        current_bar++;
        melody_smbl++;
    }
    pe.bar      = current_bar;
    while(*melody_smbl == ' ')
        melody_smbl++;
    current_note = 0;
    switch(*melody_smbl){
      case 'p':
      case 'P':
        current_note = note_pause;
        break;
      case 'a':
      case 'A':
        current_note = note_a;
        break;
      case 'b':
      case 'B':
      case 'h':
      case 'H':
        current_note = note_b;
        break;
      case 'c':
      case 'C':
        current_note = note_c;
        break;
      case 'd':
      case 'D':
        current_note = note_d;
        break;
      case 'e':
      case 'E':
        current_note = note_e;
        break;
      case 'f':
      case 'F':
        current_note = note_f;
        break;
      case 'g':
      case 'G':
        current_note = note_g;
        break;
      case 't':
      case 'T':
        current_note = note_tempo;
        break;
      case 's':
      case 'S':
        current_note = note_shift;
        break;
      default:
        pe.err_type = PARSE_ERR_BAD_SYMBOL;
        pe.bar      = current_bar;
        return;
        
    }
    melody_smbl++;
    if(*melody_smbl == '#'){
        if(current_note == note_tempo || 
           current_note == note_pause ||
           current_note == note_e ||
           current_note == note_b){
            pe.err_type = PARSE_ERR_IMPOSSIBLE_ALT;
            return;
        }
        melody_smbl++;
        current_note++;
    } 
    if(current_note == note_tempo || current_note == note_shift ){
        if(*melody_smbl != '='){
            pe.err_type = PARSE_ERR_INITILIZATION_SYNTAX;
            return;
        }
        melody_smbl++;
    }
}
void MelodyPlayer::parseValue(){
    bool sign = false;
    current_value = 0;
    if(*melody_smbl == '-'){
        sign = true;
        melody_smbl++;
    }
    while (*melody_smbl >= '0' && *melody_smbl <= '9') {
        current_value *= 10;
        current_value += (*melody_smbl - '0');
        melody_smbl++;
    }
    if(sign)
        current_value = -current_value;
}

void MelodyPlayer::parseInterval(){
    if (*melody_smbl != '@') {
        pe.err_type = PARSE_ERR_INTERVAL_SYNTAX;
        return;
    }
    melody_smbl++;
    parseValue();
    current_interval_nom   = current_value;
    if (*melody_smbl != '/') {
        pe.err_type = PARSE_ERR_INTERVAL_SYNTAX+2;
        return;
    }
    melody_smbl++;
    parseValue();
    current_interval_denom = current_value;
}
void MelodyPlayer::calcFreq(){
    playback[playback_size].note = current_note;
    math_tmp = 0;
    if(current_note != note_pause) {
        math_tmp = (current_note-1);
        math_tmp += 12*current_octave;
        math_tmp += current_tone_shift;
        current_note   = math_tmp % 12;
        current_octave = math_tmp / 12;
        math_tmp = NOTES_FREQ[current_note];
        if(current_octave > BASE_OCTAVE)
            math_tmp <<= (current_octave - BASE_OCTAVE);
        else 
            math_tmp >>= (BASE_OCTAVE - current_octave);
    }
    playback[playback_size].freq = math_tmp;
}
void MelodyPlayer::calcTime(){
    math_tmp = 60000;
    math_tmp /= current_tempo;
    math_tmp <<= 2;
    math_tmp *= current_interval_nom;
    math_tmp /= current_interval_denom;
    playback[playback_size].time = math_tmp;
}
bool MelodyPlayer::parse(char * melodytext){
    current_tempo = base_tempo;
    current_tone_shift = 0;
    current_note = 0;
    current_bar = 0;

    melody_smbl = melodytext;
    pe.err_type = PARSE_ERR_OK;
    cursor = 0; 
    playback_size = 0;
    while(*melody_smbl != '\0'){
        parseSymbol();
        if(pe.err_type != PARSE_ERR_OK){
            pe.pos = melody_smbl-melodytext;
            return false;
        }
        if(current_note != note_pause){
            parseValue();
            current_octave = current_value;
        }
        switch(current_note){
            case note_tempo:
                current_tempo = current_value;
                continue;  
            case note_shift: 
                current_tone_shift = current_value;
                continue;
            default:
                parseInterval();
                if(pe.err_type != PARSE_ERR_OK){
                    pe.pos = melody_smbl-melodytext;
                    return false;
                }
                break;
        }

        calcFreq();
        calcTime();
        playback_size++;
    }

    cursor = 0;
    return true;
}
RenderedNote_t * MelodyPlayer::nextNote(){
    if(cursor == playback_size)
        return NULL;
    return &playback[cursor++];
}