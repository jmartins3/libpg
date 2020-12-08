#pragma once

/**-----------------------------------------------------------------
 * Player registration service
 * 
 * "REGIST USER"
 * 
 * Request:
 * REGIST <LF>
 * <student_number>   <username>   <password> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * 
 * "LIST USERS"
 * 
 * Request:
 * LIST_USERS <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <number_1> <name_1> <LF>
 * <number_2> <name_2> <LF>
 * ...
 * <number_2> <name_2> <LF>
 * <LF>
 * 
 * "UNREGIST USER"
 * 
 * Request:
 * UNREGIST <LF>
 * <username>   <password> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * 
 * "CREATE THEME"
 * 
 * Request:
 * CREATE_THEME <LF>
 * <username>   <password> <LF>
 * <new theme> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * 
 * "CREATE TOPIC"
 * <username>   <password> <LF>
 * <theme>  <new topic> <limit> <LF>
 * <msg port> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * 
 * "LIST THEMES"
 * 
 * Request:
 * LIST_THEMES <LF>
 * <username> <passwd> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <theme 1> <LF>
 * <theme 2> <LF>
 * ...
 * <theme n> <LF>
 * <LF>
 * 
 * 
 * "LIST TOPICS FROM A THEME"
 * 
 * Request:
 * LIST_TOPICS <LF>
 * <username> <passwd> <LF>
 * <theme> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <topic 1> <LF>
 * <topic 2> <LF>
 * ...
 * <topic n> <LF>
 * <LF>
 * 
 * "REMOVE THEME"
 * 
 * Request:
 * REMOVE_THEME <LF>
 * <username> <passwd> <LF>
 * <theme> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * "REMOVE TOPIC"
 * 
 * Request:
 * REMOVE_TOPIC <LF>
 * <username> <passwd>
 * <theme> <topic> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * 
 * "DESTROY_TOPIC"
 * <username> <passwd>
 * <theme> <topic> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * Comments:
 * A message is send to the owner of a topic presenting the new joiner
 * ENTER_PARTNER <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 * 
 * "JOIN A TOPIC"
 * 
 * Request:
 * JOIN_TOPIC <LF>
 * <username> <passwd>
 * <theme> <topic> <LF>
 * <msg port> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <total_joiners> <LF>
 * <LF>
 * 
 * Comments:
 * A message is send to the topic  partners informating the topi destruction  
 * TOPIC_DESTROYED <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 * 
 *
 * "LEAVE A TOPIC"
 * 
 * Request:
 * LEAVE_TOPIC <LF>
 * <username> <passwd>
 * <theme>  <topic> <LF>
 * <LF>
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <remaining_joiners> <LF>
 * <LF>
 * 
 * Comments:
 * A message is send to the owner of a topic informing the leaving partner
 * 
 * LEAVE_PARTNER <LF>
 * <username>  <theme>  <topic> <total_joiners><LF>
 * <LF>
 * 
 * "BROADCAST A MESSAGE TO A TOPIC GROUP"
 * 
 * Request:
 * BROADCAST <LF>
 * <username> <passwd>
 * <theme>  <topic> <LF>
 * <line 1>  <LF>
 * <line 2>  <LF>
 * ....
 * <line n>  <LF>
 * <LF>
 *
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 * "SEND A PRIVATE MESSAGE TO A TOPIC PARTNER"
 * 
 * MESSAGE <LF>
 * <username> <passwd> <LF>
 * <theme>  <topic>  <user destination> <LF>
 * <line 1>  <LF>
 * <line 2>  <LF>
 * ....
 * <line n>  <LF>
 * <LF>
 *
 * 
 * Response:
 * <status> <LF> // 200 -ok 400 - Invalid command 500 - Server error
 * <LF>
 * 
 */
 
 // command status

#define BAD_COMMAND 			401
#define BAD_COMMAND_ARGS		402
#define UNKNOWN_USER 			403
#define BAD_USER_AUTH			404
#define COMMAND_ERROR			420
#define STATUS_OK				201
#define SERVER_ERROR 			501
#define UNREACHABLE_TOPIC_OWNER 502

#define ERR_TOPIC_DUPLICATE		412

#define REG_SERVER_PORT 60000
#define MSG_PORT 		60001


	
	

 
