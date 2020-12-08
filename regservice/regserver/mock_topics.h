
typedef struct {} user_t;
typedef struct {} topic_t;

#ifdef __cplusplus 
extern "C" {
#endif

void topic_leave2(topic_t *topic, user_t *leaving_user);

void topic_remove2(topic_t *topic, user_t* remover);

#ifdef __cplusplus 
}
#endif
