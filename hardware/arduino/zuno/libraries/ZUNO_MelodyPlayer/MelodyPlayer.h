#pragma once
#include <Arduino.h>

typedef struct RenderedNote_s{
    byte note;
    word freq;
    word time;
}RenderedNote_t;

typedef struct ParseError_s{
    word pos;
    byte bar;
    byte err_type;
}ParseError_t;

enum{
    PARSE_ERR_OK,
    PARSE_ERR_BAD_SYMBOL,
    PARSE_ERR_IMPOSSIBLE_ALT,
    PARSE_ERR_OCTAVE_SYNTAX,
    PARSE_ERR_INTERVAL_SYNTAX,
    PARSE_ERR_INITILIZATION_SYNTAX
};
class MelodyPlayer{

public:
    MelodyPlayer(RenderedNote_t * playback);
    bool parse(char * melodytext);
    ParseError_t * getParseError(){return &pe;};
    RenderedNote_t * nextNote();
    void resetCursor(){ cursor = 0; };
    void stop(){ playback_size = 0; cursor = 0; };

    void setBaseTempo(byte tempo){base_tempo = tempo;};
    void setBaseToneShift(byte shift){base_tone_shift = shift;};
private:

    void parseSymbol();
    void parseValue();
    void parseInterval();
    void calcFreq();
    void calcTime();

private:
    RenderedNote_t * playback;
    word cursor;
    word playback_size;
    char * melody_smbl;
    byte base_tempo;
    byte base_tone_shift;
    byte current_tempo;
    char current_tone_shift;
    byte current_note;
    byte current_octave;
    byte current_interval_nom;
    byte current_interval_denom;
    byte current_bar;
    int  current_value;
    word math_tmp;
    ParseError_t pe;

};