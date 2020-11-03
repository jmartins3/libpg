#pragma once

// 
// server codes
#define ERR_TOPIC_DUPLICATE 432
#define STATUS_OK			201

void gs_new_game(session_t session, const char *game);
