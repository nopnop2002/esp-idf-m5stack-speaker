#define CMD_LEFT  		100
#define CMD_LONG_LEFT	150
#define CMD_MIDDLE  	200
#define CMD_LONG_MIDDLE	250
#define CMD_RIGHT  		300
#define CMD_LONG_RIGHT	350
#define CMD_HALT 		900

typedef struct {
    uint16_t command;
    TaskHandle_t taskHandle;
} CMD_t;

