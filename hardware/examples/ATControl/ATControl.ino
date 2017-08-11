/*
 * Z-Wave slave device fully controlled by Serial
 * via AT-Commands
 */
 
// Z-Uno can use any serial interface
// You can select
// Serial for USB CDC
// Serial0 for UART0
// Serial1 fot UART1 
#define MY_SERIAL Serial
#define BAD_DIGIT 0xFF
#define MAX_CHANNELS 10 // The maximum number of channels equals to this number for Z-Uno 



enum
{
	AT_CMD_CLEAN_CHANNELS,
	AT_CMD_ADD_CHANNEL,
	AT_CMD_ADD_ASSOCIATION,
	AT_CMD_SET_CFGFLAGS,
	AT_CMD_LEARN,
	AT_CMD_CONFIG_COMMIT,
	AT_CMD_REQUESTMAP,
	AT_CMD_CLEANREQUESTS,
	AT_CMD_CHANNELSET,
	AT_CMD_CHANNELGET,
	AT_CMD_SENDREPORT,
	AT_CMD_SENDASSOC
};
enum
{
	AT_STATE_WAITSTART,
	AT_STATE_WAITCMD,
	AT_STATE_WAITPARAM
};

// Global data
// We use them to reduce stack usage...
byte state = AT_STATE_WAITSTART;
byte cmd;
byte param[8];
byte param_count = 0;
byte resp_count = 0;
char tmp_buff[32];
byte tmp_len = 0;
byte req_map[4]={0,0,0,0};
byte channel_value[MAX_CHANNELS][4];
word channel_2_update = 0;

void setup() {
  MY_SERIAL.begin(115200);
}
byte parseCMD(){

	tmp_buff[tmp_len] = '\0';
	tmp_len = 0;	
	if(strcmp(tmp_buff, "AT_CLEAN_CHANNELS") == 0){
		cmd = AT_CMD_CLEAN_CHANNELS;
	}
	else
	if(strcmp(tmp_buff, "AT_ADD_CHANNEL") == 0){
		cmd = AT_CMD_ADD_CHANNEL;
	}	
	else
	if(strcmp(tmp_buff, "AT_ADD_ASSOCIATION") == 0){
		cmd = AT_CMD_ADD_ASSOCIATION;
	}
	else
	if(strcmp(tmp_buff, "AT_CONFIG_FLAGS") == 0){
		cmd = AT_CMD_SET_CFGFLAGS;
	}
	else
	if(strcmp(tmp_buff, "AT_LEARN") == 0){
		cmd = AT_CMD_LEARN;
	}
	else
	if(strcmp(tmp_buff, "AT_CONFIG_COMMIT") == 0){
		cmd = AT_CMD_CONFIG_COMMIT;
	}
	else
	if(strcmp(tmp_buff, "AT_REQ_MAP") == 0){
		cmd = AT_CMD_REQUESTMAP;
	}
	else
	if(strcmp(tmp_buff, "AT_REQ_CLEAN") == 0){
		cmd = AT_CMD_CLEANREQUESTS;
	}
	else
	if(strcmp(tmp_buff, "AT_CMD_CHANNEL_SET") == 0){
		cmd = AT_CMD_CHANNELSET;
	}
	else
	if(strcmp(tmp_buff, "AT_CMD_CHANNEL_GET") == 0){
		cmd = AT_CMD_CHANNELGET;
	}
	else
	if(strcmp(tmp_buff, "AT_CMD_SEND_ASSOC") == 0){
		cmd = AT_CMD_SENDASSOC;
	}
	else
	if(strcmp(tmp_buff, "AT_CMD_SEND_REPORT") == 0){
		cmd = AT_CMD_SENDREPORT;
	}
	else{
		return FALSE;
	}

	return TRUE;
}
void response(char *resp_name){
	byte i;
	MY_SERIAL.print("^");
	MY_SERIAL.print(resp_name);
	for(i=0;i<resp_count;i++){
		MY_SERIAL.print(",");
		if(param[i]<0x10)
			MY_SERIAL.print("0");
		MY_SERIAL.print(param[i], HEX);
	}
	MY_SERIAL.println("$");
	resp_count = 0;
}
byte processCMD(){
	switch(cmd){

		case AT_CMD_CLEAN_CHANNELS:
			ZUNO_START_CONFIG();
			response("CLEAN_CHANNELS_OK");
			break;
		case AT_CMD_ADD_CHANNEL:
			zunoAddChannel(param[0], param[1], param[2]);
			response("ADD_CHANNEL_OK");
			break;
		case AT_CMD_ADD_ASSOCIATION:
			zunoAddAssociation(param[0]);
			response("ADD_ASSOCIATION_OK");
			break;	
		case AT_CMD_SET_CFGFLAGS:
			g_user_sketch->flags = param[0];
			response("CFG_FLAGS_OK");
			break;
		case AT_CMD_LEARN:
			zunoStartLearn(param[0]);
			response("LEARN_OK");
			break;
		case AT_CMD_CONFIG_COMMIT:
			ZUNO_COMMIT_CONFIG();
			resp_count = 1;
			param[0] = ZUNO_GET_CONFIG_STATE();
			response("COMMIT_CONFIG");
			break;
		case AT_CMD_REQUESTMAP:
			resp_count = 4;
			memcpy(param,req_map,4);
			response("REQUEST_MAP");
			break;
		case AT_CMD_CLEANREQUESTS:
			req_map[0] = 0;
			req_map[1] = 0;
			req_map[2] = 0;
			req_map[3] = 0;
			response("REQMAP_CLEAN_OK");
			break;
		case AT_CMD_CHANNELSET:
			memcpy(channel_value[param[0]],param+1, param_count-1);
			response("CHANNEL_SET_OK");
			break;
		case AT_CMD_CHANNELGET:
			resp_count = 4;
			memcpy(param, channel_value[param[0]], resp_count);
			response("CHANNEL_VALUE");
			break;
		case AT_CMD_SENDREPORT:
			zunoSendReport(param[0]+1);
			response("SEND_REPORT_OK");
			break;
		case AT_CMD_SENDASSOC:
			zunoSendAssociationCommand(param[0], param[1], param[2], param[3]);
			response("SEND_ASSOCIATION_OK");
			break;	
		default:
			break;


	}	
	return 1;
}
byte getHexDigit(char c){

	if ((c>='0') && (c<='9'))
		return c -'0';
	if ((c>='a') && (c<='f'))
		return 10 + c - 'a';
	if ((c>='A') && (c<='F'))
		return 10 + c - 'A';
	return BAD_DIGIT;
}
byte parseParam(){

	if(tmp_len != 2)
	{
		tmp_len = 0;	
		return FALSE;
	}
	tmp_len = 0;	

	byte val = getHexDigit(tmp_buff[0]);
	if(val == BAD_DIGIT)
		return FALSE;
	val <<= 4; // high tetrade
	param[param_count] = val;
	val = getHexDigit(tmp_buff[1]);
	if(val == BAD_DIGIT)
		return FALSE;
	param[param_count] += val; 
	param_count++;
	return 1;

}
void parseError(){

	state = AT_STATE_WAITSTART;
	MY_SERIAL.println("^SYNTAX_ERROR$");
}
void parseAT(){
	while(MY_SERIAL.available())
	{
		char symbole = MY_SERIAL.read();
		switch(state)
		{
			case AT_STATE_WAITSTART:
				if(symbole == '^')
				{
					tmp_len = 0;
					param_count = 0;
					resp_count = 0;
					state++;
				}			
				break;
			case AT_STATE_WAITCMD:
				if(symbole == ',')
				{
					if(!parseCMD())
						parseError();
					else
						state++;

				}
				else
				if(symbole == '$')
				{
					if(parseCMD())
						processCMD();
					else
						parseError();
					state = AT_STATE_WAITSTART;
				}
				else
				{
					tmp_buff[tmp_len] =  symbole;
					tmp_len++;
				}
				break;
			case AT_STATE_WAITPARAM:
				{
					if(symbole == ',')
					{	
						if(!parseParam())
							parseError();
					}
					else
					if(symbole == '$')
					{
						if(parseParam())
							processCMD();
						else
							parseError();
						state = AT_STATE_WAITSTART;
					}
					else
					{
						tmp_buff[tmp_len] =  symbole;
						tmp_len++;
					}
				}
				break;
		}


	}
}
void makeUnsolicitedReports(){
	byte i = 0;
	byte m = 1;	
	while(channel_2_update != 0)
	{
		if(channel_2_update & m)
		{
			resp_count = 5;
			param[0] = i;
			memcpy(param+1, channel_value[param[0]], 4);
			response("CHANNEL_CHANGED");
		}

		i++;
		channel_2_update &= ~(m);
		m <<= 1;

	}
}
void loop() {

  parseAT();
  makeUnsolicitedReports();
  delay(100);
}

// Universal handler for all the channels
void zunoCallback(void){

    // See callback_data variable 
    // We use word params for all 
    // We use zero based index of the channel instead of typical 
    // Getter/Setter index of Z-Uno. 
    // See enum ZUNO_CHANNEL*_GETTER/ZUNO_CHANNEL*_SETTER in ZUNO_Definitions.h 
    byte index = callback_data->type;

    index >>= 1;
    index --;
    byte bi = index / 8;
    byte mask = 1 << (index & 0x07);

    if(callback_data->type & SETTER_BIT)
    {
    	req_map[bi] |= mask;
 	 	memcpy(channel_value[index], callback_data->param.buffParam, 4);

 	 	// Data for unsolicited reports
    	channel_2_update |= mask;
    	if(bi == 0)
    		channel_2_update |= mask;
    	else
    		channel_2_update |= ((word)mask) << 8;

    }
    else
    {
    	memcpy(callback_data->param.buffParam, channel_value[index], 4);
    	req_map[bi+2] |= mask;
    }
    
}