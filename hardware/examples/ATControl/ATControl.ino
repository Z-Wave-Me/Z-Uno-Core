/*
 * Z-Wave slave device fully controlled via Serial (USB/UART) using AT-Commands
 */
 
// Z-Uno can use any serial interface
// You can select
// Serial for USB
// Serial0 for UART0
// Serial1 fot UART1 
#define MY_SERIAL Serial

#define MAX_CHANNELS 10 // The maximum number of channels supported by Z-Uno 

// Commands
enum {
	AT_CMD_CLEAN_CHANNELS,
	AT_CMD_ADD_CHANNEL,
	AT_CMD_ADD_ASSOCIATION,
	AT_CMD_CONFIG_FLAGS,
	AT_CMD_LEARN,
	AT_CMD_CONFIG_COMMIT,
	AT_CMD_CHANNEL_SET,
	AT_CMD_CHANNEL_GET,
	AT_CMD_SEND_REPORT,
	AT_CMD_SEND_ASSOC
};

// Parser state machine
enum {
	AT_STATE_WAITSTART,
	AT_STATE_WAITCMD,
	AT_STATE_WAITPARAM
};

// Global data
// We use global scope to reduce stack usage
byte state = AT_STATE_WAITSTART;      // state machine
byte cmd;                             // current command
long int param[4];                    // command parameters
byte param_count = 0;                 // number of bytes in parameters
byte resp_count = 0;                  // number of bytes in report
char tmp_buff[32];                    // temp buffer
byte tmp_len = 0;                     // temp buffer length
long int channel_value[MAX_CHANNELS]; // channel values to report
word channel_to_update = 0;           // list of channels to update

void setup() {
	MY_SERIAL.begin(115200);
}

// Incoming commands parser
byte parseCmd() {
	tmp_buff[tmp_len] = '\0';
	tmp_len = 0;

	if (strcmp(tmp_buff, "AT_CLEAN_CHANNELS") == 0) {
		cmd = AT_CMD_CLEAN_CHANNELS;
	} else if (strcmp(tmp_buff, "AT_ADD_CHANNEL") == 0) {
		cmd = AT_CMD_ADD_CHANNEL;
	} else if (strcmp(tmp_buff, "AT_ADD_ASSOCIATION") == 0) {
		cmd = AT_CMD_ADD_ASSOCIATION;
	} else if (strcmp(tmp_buff, "AT_CONFIG_FLAGS") == 0) {
		cmd = AT_CMD_CONFIG_FLAGS;
	} else if (strcmp(tmp_buff, "AT_LEARN") == 0) {
		cmd = AT_CMD_LEARN;
	} else if (strcmp(tmp_buff, "AT_CONFIG_COMMIT") == 0) {
		cmd = AT_CMD_CONFIG_COMMIT;
	} else if (strcmp(tmp_buff, "AT_CHANNEL_SET") == 0) {
		cmd = AT_CMD_CHANNEL_SET;
	} else if (strcmp(tmp_buff, "AT_CHANNEL_GET") == 0) {
		cmd = AT_CMD_CHANNEL_GET;
	} else if (strcmp(tmp_buff, "AT_SEND_ASSOC") == 0) {
		cmd = AT_CMD_SEND_ASSOC;
	} else if (strcmp(tmp_buff, "AT_SEND_REPORT") == 0) {
		cmd = AT_CMD_SEND_REPORT;
	} else {
		return FALSE;
	}

	return TRUE;
}

// Print respons as ^...$
void response(char *resp_name) {
	byte i;
	MY_SERIAL.print("^");
	MY_SERIAL.print(resp_name);
	for (i = 0; i < resp_count; i++) {
		MY_SERIAL.print(",");
		MY_SERIAL.print(param[i]);
	}
	MY_SERIAL.println("$");
	resp_count = 0;
}

// Process command
byte processCmd() {
	switch(cmd) {
		case AT_CMD_CLEAN_CHANNELS:
			ZUNO_START_CONFIG();
			response("CLEAN_CHANNELS_OK");
			break;
		case AT_CMD_ADD_CHANNEL:
			ZUNO_ADD_CHANNEL(byte(param[0]), byte(param[1]), byte(param[2]));
			response("ADD_CHANNEL_OK");
			break;
		case AT_CMD_ADD_ASSOCIATION:
			ZUNO_ADD_ASSOCIATION(byte(param[0]));
			response("ADD_ASSOCIATION_OK");
			break;	
		case AT_CMD_CONFIG_FLAGS:
			g_user_sketch->flags = byte(param[0]);
			response("CFG_FLAGS_OK");
			break;
		case AT_CMD_LEARN:
			zunoStartLearn(byte(param[0]));
			response("LEARN_OK");
			break;
		case AT_CMD_CONFIG_COMMIT:
			ZUNO_COMMIT_CONFIG();
			resp_count = 1;
			param[0] = ZUNO_GET_CONFIG_STATE();
			response("COMMIT_CONFIG");
			break;
		case AT_CMD_CHANNEL_SET:
			channel_value[byte(param[0]) - 1] = param[1];
			response("CHANNEL_SET_OK");
			break;
		case AT_CMD_CHANNEL_GET:
			param[1] = channel_value[byte(param[0]) - 1];
			resp_count = 2;
			response("CHANNEL_VALUE");
			break;
		case AT_CMD_SEND_REPORT:
			zunoSendReport(byte(param[0]));
			response("SEND_REPORT_OK");
			break;
		case AT_CMD_SEND_ASSOC:
			zunoSendAssociationCommand(byte(param[0]), byte(param[1]), byte(param[2]), byte(param[3]));
			response("SEND_ASSOCIATION_OK");
			break;	
		default:
			break;
	}	
	return 1;
}

// Fill command parameters
byte parseParam() {
	// Check if value is valid
	for(byte i = 0; i < tmp_len; i++) {
		if (tmp_buff[i] < '0' || tmp_buff[i] > '9') {
			return 0;
		}
	}
	tmp_buff[tmp_len] = '\0';
	tmp_len = 0;
	// Convert string representation to long
	param[param_count] = atol(tmp_buff);
	param_count++;
	return 1;
}

// Respond with error
void parseError() {
	state = AT_STATE_WAITSTART;
	MY_SERIAL.println("^SYNTAX_ERROR$");
}

// Read and parse AT command
// Uses a state machine for parsing
void parseAT() {
	while(MY_SERIAL.available()) {
		char symbol = MY_SERIAL.read();
		switch(state) {
			case AT_STATE_WAITSTART:
				if (symbol == '^') {
					tmp_len = 0;
					param_count = 0;
					resp_count = 0;
					state++;
				}			
				break;
			case AT_STATE_WAITCMD:
				if (symbol == ',') {
					if (!parseCmd()) {
						parseError();
					} else {
						state++;
					}
				} else if (symbol == '$') {
					if (parseCmd()) {
						processCmd();
					} else {
						parseError();
					}
					state = AT_STATE_WAITSTART;
				} else {
					tmp_buff[tmp_len] = symbol;
					tmp_len++;
				}
				break;
			case AT_STATE_WAITPARAM:
				if (symbol == ',') {	
					if (!parseParam()) {
						parseError();
					}
				} else if (symbol == '$') {
					if (parseParam()) {
						processCmd();
					} else {
						parseError();
					}
					state = AT_STATE_WAITSTART;
				} else {
					tmp_buff[tmp_len] = symbol;
					tmp_len++;
				}
				break;
		}
	}
}

// Push unsolicited channel value updates to the serial port
void makeUnsolicitedReports() {
	byte i = 0;
	byte m = 1;
	while (channel_to_update != 0) {
		if (channel_to_update & m) {
			resp_count = 2;
			param[0] = i + 1;
			param[1] = channel_value[i];
			response("CHANNEL_CHANGED");
		}

		i++;
		channel_to_update &= ~(m);
		m <<= 1;
	}
}

// Parse AT commands and push updates in a loop
void loop() {
	parseAT();
	makeUnsolicitedReports();
	delay(100);
}

// Universal handler for all the channels
void zunoCallback(void) {
	// See callback_data variable 
	// We use word params for all 
	// We use zero based index of the channel instead of typical Getter/Setter index of Z-Uno. 
	// See enum ZUNO_CHANNEL*_GETTER/ZUNO_CHANNEL*_SETTER in ZUNO_Definitions.h

	byte index = callback_data->type;

	index >>= 1;
	index --;
	byte bi = index / 8;
	byte mask = 1 << (index & 0x07);

	if (callback_data->type & SETTER_BIT) {
 	 	channel_value[index] = callback_data->param.bParam;

 	 	// Data for unsolicited reports
		channel_to_update |= mask;
		if (bi == 0) {
			channel_to_update |= mask;
		} else {
			channel_to_update |= ((word)mask) << 8;
		}
	} else {
		callback_data->param.dwParam = channel_value[index];
	}
}
