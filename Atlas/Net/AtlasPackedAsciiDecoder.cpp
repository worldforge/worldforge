/*
	AtlasProtocolDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com
*/

#include "AtlasObject.h"
#include "AtlasProtocol.h"
#include "AtlasPackedAsciiDecoder.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void APackedAsciiDecoder::newStream()
{
	// reset decoder
	state = 1;
	nestd = 0;
	token = 0;
}

void APackedAsciiDecoder::feedStream(char* data)
{
	if (strlen(buffer)+strlen(data) > 2048) {
		token = -1;
		return;
	}
	printf("STREAM=%s\n", data);
	strcat(buffer,data);
}

int APackedAsciiDecoder::getToken()
{
	int tmp = token;
	token = 0;
	return tmp;
}

int APackedAsciiDecoder::hasTokens()
{
	char	*pos, *pos1;
	int	ndx;
	int	chk;
	char	typ;

	printf("BEG TOKEN=%i\n", token);

	if (token == -1) return-1; // buffer overflow !!!

	do {
		chk = 0;

		printf("BEG STATE=%i\n", state);
		printf("BEG BUFFR=%s\n", buffer);

		// this is where we are gonna put the state machine
		switch (state) {
		case 1:// start of message
			while (buffer[0] != 0 && buffer[0] != '{')
				strcpy(buffer,buffer+1);
			if (buffer[0] == 0) break;// not found
			pos = strchr(buffer, '=');
			if (pos == NULL) break;
			// extract name
			ndx = pos - buffer;
			memset(name, 0, sizeof(name));
			strncpy(name,buffer+1,ndx-1);
			// strip up to token
			strcpy(buffer,pos+1);
			// change states
			token = AProtocol::atlasMSGBEG;
			state = 2;
			chk = 1;
			break;

		case 2:
			if (buffer[0] == 0) break; // wait for more data
			if ((pos = strchr("[(%$#])}",buffer[0])) == NULL)
			{
				// bad protcol character
				token = AProtocol::atlasERRTOK;
				state = 1;
				break;
			}
			typ = buffer[0];
			if (typ == ')') {
				// end of list
				strcpy(buffer,buffer+1);
				token = AProtocol::atlasATREND;
				state = 2;
				break;
			}
			if (typ == ']') {
				// end of map
				strcpy(buffer,buffer+1);
				token = AProtocol::atlasATREND;
				state = 2;
				break;
			}
			if (typ == '}') {
				// end of message
				strcpy(buffer,buffer+1);
				token = AProtocol::atlasMSGEND;
				state = 1;
				break;
			}
			// must be an attribute == wait until we got the name
			if ((pos=strchr(buffer,'=')) == NULL) break;
			// get the name out before proc
			ndx = pos - buffer;
			memset(name, 0, sizeof(name));
			strncpy(name,buffer+1,ndx-1);
			// strip up to token
			strcpy(buffer,pos+1);
			// got an attribute name
			if (typ == '(') type = AProtocol::atlasLST;
			if (typ == '[') type = AProtocol::atlasMAP;
			if (typ == '%') type = AProtocol::atlasINT;
			if (typ == '#') type = AProtocol::atlasFLT;
			if (typ == '$') type = AProtocol::atlasSTR;
			// change states, wait for value
			token = AProtocol::atlasATRBEG;
			if (type==AProtocol::atlasLST || type==AProtocol::atlasMAP) {
				state = 2;
			} else {
				state = 3;
			}
			chk = 1;
			break;

		case 3:
			if (buffer[0] == 0) break; // wait for more data
			pos = strchr(buffer,'[');
			pos1 = strchr(buffer,'(');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,'%');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,'#');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,'$');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,']');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,')');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			pos1 = strchr(buffer,'}');
			if (pos == NULL || (pos1 != NULL && pos1 < pos)) pos = pos1;
			if (pos == NULL) {
				// no end marker yet
				break;
			}
			// got an end marker, pull the data
			if (sval != NULL) { free(sval);sval = NULL; }
			sval = (char*)malloc((pos-buffer)+2);
			memset(sval, 0, (pos-buffer)+2);
			strncpy(sval,buffer,(pos-buffer));
			if (type == AProtocol::atlasINT) ival = atoi(sval);
			if (type == AProtocol::atlasFLT) fval = atof(sval);
			// strip up to token
			strcpy(buffer,pos);
			token = AProtocol::atlasATRVAL;
			state = 4;
			chk = 1;
			break;

		case 4:	// special state to end attributes
			token = AProtocol::atlasATREND;
			state = 2;
			chk = 1;
			break;

		default:
			// invalid state
			token =-1;
			state =1;
			break;
		}

	} while (chk == 1 && token == 0);

	// see if we have a token to return
	printf("END TOKEN=%i\n", token);
	printf("END STATE=%i\n", state);
	printf("END BUFFR=%s\n\n\n", buffer);
	if (token != 0) {
		return 1;
	}
	return 0;
}




