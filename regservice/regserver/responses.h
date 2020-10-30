#pragma once


#include "channel.h"
#include "commands.h"

void send_status_response(channel_t *chn, int status);
 

void send_response(channel_t *chn, Answer *answer);
	 
