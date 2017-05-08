#include "ZMEKeypad.h"


ZMEKeypad::ZMEKeypad(BYTE * colums, BYTE numcols, BYTE * rows, BYTE numrows)
{
	column_vec 	=	colums;
	n_col       =   numcols;
	row_vec		=	rows;
	n_row		=	numrows;
	mon_keys    =   0;

  hold_time     = HOLD_DEFAULT_TIME;
  debounce_time = DEBOUNCE_DEFAULT_TIME;


}

void ZMEKeypad::begin()
{
  BYTE i;

   for (i=0; i<n_row; i++) {
      m_pinMode(row_vec[i],INPUT_PULLUP);

  } 
  for (i=0; i<n_col; i++) {
      m_pinMode(column_vec[i],OUTPUT);
      m_pinSet(column_vec[i], LOW);
  }
  mon_keys = 0;
}
BYTE ZMEKeypad::scanKeys(BYTE * key_actions)
{
	  BYTE c,r,i;
    BYTE ret = 0;
    DWORD scan_time = millis();

    // Re-intialize the row pins. Allows sharing these pins with other hardware.
    for (r=0; r<n_row; r++) {
      m_pinMode(row_vec[r],INPUT_PULLUP);

    }	

    for (c=0; c<n_col; c++) {
      m_pinSet(column_vec[c], LOW);  // Begin column pulse output.
  	
  	   
  	  for (r=0; r<n_row; r++) {

  	  	BYTE key_num = r*n_row + c;

  	  	BYTE mon_key_index = 0xFF;

  	  	// Did we scan this key нуе?
  	  	for(i=0;i<mon_keys;i++)
  	  	{
  	  		if(key_states[i].key_num == key_num)
  	  			mon_key_index = i;
  	  	}

  	  	if(!m_pinGet(row_vec[r]))
        {
        	if(mon_key_index != 0xFF)
        	{
        		WORD kt = (scan_time - key_states[mon_key_index].time)/10;
        		switch(key_states[mon_key_index].key_state)
        		{
        			case KEYPAD_KEYSTATE_DEBOUNCE:
        				if(kt > debounce_time)
        					key_states[mon_key_index].key_state = KEYPAD_KEYSTATE_PRESSED;
        				break;
        			case KEYPAD_KEYSTATE_PRESSED:
        				if(kt > hold_time)
        					key_states[mon_key_index].key_state = KEYPAD_KEYSTATE_HOLDED;
        				break;
        			case KEYPAD_KEYSTATE_HOLDED:
        				key_actions[ret] = key_num | 0x80;
        				ret++;
        				break;
        		}
        	}	
        	else
        	{
        		if(mon_keys >= MAX_SCAN_KEYS)
        			break;
        		// Add this key to keypad state, return to the start condition
        		key_states[mon_keys].key_num   =    key_num;
        		key_states[mon_keys].key_state =	KEYPAD_KEYSTATE_DEBOUNCE;
        		key_states[mon_keys].time      =	scan_time;
            mon_keys++;
        	}
        	

        }
        else
        {
        	if(mon_key_index != 0xFF)
        	{
        		if(key_states[mon_key_index].key_state == KEYPAD_KEYSTATE_PRESSED)
        		{
        			key_actions[ret] = key_num;
        			ret++;
        		}	
        		
        		// Remove key from list
        		for(i=mon_key_index;i<(mon_keys-1);i++)
        		{
        			key_states[i].key_num   =   key_states[i+1].key_num;
        			key_states[i].key_state =	key_states[i+1].key_state;
        			key_states[i].time      =	key_states[i+1].time;
        		}
        		mon_keys--;
        	}	


        }	
       
      }
      m_pinSet(column_vec[c], HIGH);  // Begin column pulse output.

  	}
    
  	return ret;

}

