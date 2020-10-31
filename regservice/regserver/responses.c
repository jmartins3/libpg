
#include <uv.h>
#include <stdlib.h>
#include "../include/list.h"
#include "commands.h"
#include "responses.h"

typedef struct {
	int error_code;
	char *description;
} error_desc_t;


#define CMD_ERR		COMMAND_ERROR
 
#define UNREACHABLE_TOPIC_OWNER 502
// error codes
#define OPER_OK 0




// user errors
 
 
 

#define USER_NOT_EMPTY			27

// general errors
#define INCONSISTENT_DB			31


error_desc_t errors[] = {
	// commands
	{ STATUS_OK,						"Status ok"							},
	
	{ BAD_COMMAND, 						"Invalid command" 					},
	{ BAD_COMMAND_ARGS, 				"Invalid command arguments" 		},
	
	// themes
	{ CMD_ERR+THEME_INEXISTENT,			"Inexistent theme"					},
	{ CMD_ERR+THEME_DUPLICATE,			"Duplicated theme"					},
	{ CMD_ERR+THEME_NOT_EMPTY,			"Can't remove a theme with topics"	},
	{ CMD_ERR+THEME_NAME_TOO_BIG,		"Theme name too big"				},
	
	// topics
	{ CMD_ERR+TOPIC_INEXISTENT,			"Inexistent topic"					},
	{ CMD_ERR+TOPIC_DUPLICATE,			"Duplicated topic"					},
	{ CMD_ERR+TOPIC_NOT_EMPTY,			"Can't remove a topic with joiners"	},
	{ CMD_ERR+TOPIC_NAME_TOO_BIG,		"Topic name too big"				},
	{ CMD_ERR+TOPIC_OWNER_NOT_LAST,		"Owner must be the last topic user"	},
	{ CMD_ERR+TOPIC_NO_TOPICS,			"There are no topics in theme"		},
	{ CMD_ERR+TOPIC_DUPLICATE_JOINER, 	"user already belongs to topic"		},
	{ CMD_ERR+TOPIC_TOO_MANY_JOINERS, 	"Limit of topic joiners achieved"	},
	{ CMD_ERR+TOPIC_USER_ALONE, 	    "No broadcast when alone on a group"},
	
	// users
	{ UNKNOWN_USER, 					"Unknown user" 						},
	{ BAD_USER_AUTH, 					"Bad user authentication"			},
	{ CMD_ERR+USER_INEXISTENT, 			"Unknown user" 						},
	{ CMD_ERR+USER_DUPLICATE, 			"User already registered"			},
	{ CMD_ERR+TOPIC_USER_NOT_JOINER,	"User must belong to the group"		},
	{ CMD_ERR+USER_NOT_THEME_OWNER, 	"User must be the theme onwer" 		},
	{ CMD_ERR+USER_NOT_TOPIC_OWNER, 	"User must be the topic onwer" 		},
	{ CMD_ERR+USER_NAME_TOO_BIG, 		"User name too big"					},
	{ CMD_ERR+USER_PASS_TOO_BIG,		"User pass too big"					},
	{ CMD_ERR+USER_NOT_EMPTY,			"User belongs or is a topic owner"	},
	{ CMD_ERR+SEND_NOT_A_TOPIC_JOINER,	"Sender is not a topic joiner"	    },
	{ CMD_ERR+DEST_NOT_A_TOPIC_JOINER,	"Destination is not a topic joiner"	},
	
	// internal errors
	{ UNREACHABLE_TOPIC_OWNER,			"Unreachable topic owner"			},
	{ SERVER_ERROR,						"Internal server error"				}
};
#define NERRORS (sizeof(errors)/sizeof(errors[0]))

char *get_description(int error_code) {
	for(int i=0; i < NERRORS; ++i) {
		if (errors[i].error_code == error_code)
		return errors[i].description;
	}
	return "Unkown error";
}

typedef struct content_builder {
	char buf[8192];
    int curr;
} ResponseBuilder;


void response_write_completion(uv_write_t *req, int status) {
  if (status) {
        fprintf(stderr, "write response error %s\n", uv_strerror(status));
        return;
    }
	channel_t *chn = container_of(req, channel_t, writereq);
	
	
	printf("free: command\n"); inc_frees();
	free(chn->cmd);
	chn->cmd= NULL;
 
	printf("response completed!\n");
  
}

static void build_response_status(ResponseBuilder *builder, int status )  {
	builder->curr += sprintf(builder->buf+builder->curr, 
		"%d %s\n", status, get_description(status));
}


static void build_list_response(ResponseBuilder *builder, Cmd *cmd, Answer *answer) {
	names_result_t *names = &answer->names;
	if (names->nresults > 0)
		builder->curr += sprintf(builder->buf+builder->curr, "%s", names->buffer);
}

static void build_join_response(ResponseBuilder *builder, Cmd *_cmd, Answer *answer) {
	CmdJoinTopic *cmd = (CmdJoinTopic *) _cmd;
	builder->curr += sprintf(builder->buf+builder->curr, "%d\n", 
		cmd->njoiners);
}

static void build_leave_response(ResponseBuilder *builder, Cmd *_cmd, Answer *answer) {
	CmdLeaveTopic *cmd = (CmdLeaveTopic *) _cmd;
	builder->curr += sprintf(builder->buf+builder->curr, "%d\n", 
		cmd->njoiners);
}



static void build_response_content(ResponseBuilder *builder, Cmd *cmd, Answer *answer) {
	switch(cmd->type) {
		case ListThemes:
		case ListTopics:
		case ListUsers:
			build_list_response(builder, cmd, answer);
			// free response buffer
			printf("free: buffer_list\n"); inc_frees();
			free(answer->names.buffer);
			break;
		case JoinTopic:
			build_join_response(builder, cmd, answer);
			break;
		case LeaveTopic:
			build_leave_response(builder, cmd, answer);
			break;
		default:
			break;	 
	}
}

static void build_response_terminator(ResponseBuilder *builder )  {
	builder->curr += sprintf(builder->buf+builder->curr, "\n");
}


void send_status_response(channel_t *chn, int status) {

	ResponseBuilder builder = { .curr = 0 };
     
    printf("send response status: %d\n", status);
    build_response_status(&builder, status);
  
  	build_response_terminator(&builder);
  	
	uv_buf_t buf = uv_buf_init(builder.buf, builder.curr);
	 
	uv_write(&chn->writereq, (uv_stream_t *) chn, &buf, 1, response_write_completion);
}

void send_response(channel_t *chn, Answer *answer) {
	ResponseBuilder builder = { .curr = 0 };
	
	printf("send response status: %d\n", answer->status);
    build_response_status(&builder, answer->status);
    
    build_response_content(&builder, chn->cmd, answer);
	
    build_response_terminator(&builder);
    
    printf("Response:\n'%s'\n", builder.buf);
    uv_buf_t buf = uv_buf_init(builder.buf, builder.curr);
	 
	uv_write(&chn->writereq, (uv_stream_t *) chn, &buf, 1, response_write_completion);
    
}

