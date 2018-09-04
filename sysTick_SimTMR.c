#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

typedef void (*pfunc)(void);

typedef struct soft_tmr_t{
	u32	starttime;
	u32	overtime;
	u8	tmr_id;
	u8	tmr_repeat;
	u8	tmr_busy;	
} soft_tmrTypeDef;

enum RMT_ID{
	TMR_A = 1,
	TMR_B,
	TMR_C,
};

#define TASK_A_OVERTIME		5
#define TASK_B_OVERTIME 	2
#define TASK_C_OVERTIME		8

#define TASK_A_REPEAT		3
#define TASK_B_REPEAT		6
#define TASK_C_REPEAT		1

pfunc	tmr_func;
soft_tmrTypeDef	soft_tmr;
struct tm *sys_time;
time_t my_time;

static inline u32 get_tick(void){
	time(&my_time);
	sys_time = localtime(&my_time);
	return sys_time->tm_min*60 + sys_time->tm_sec;
}

bool tmr_regist(u8 id, u32 overtime, u8 repeat, pfunc ptmr){
	if(true != soft_tmr.tmr_busy){
		printf("timer register\n");
		soft_tmr.tmr_id = id;
		soft_tmr.starttime = get_tick();
		soft_tmr.overtime = overtime;
		soft_tmr.tmr_repeat = repeat;
		soft_tmr.tmr_busy = true;
		tmr_func = ptmr;
		return true;
	}
	return false;
}

void tmr_call(void){
	tmr_func();
	if(soft_tmr.tmr_repeat > 0){
		soft_tmr.tmr_repeat--;
		soft_tmr.starttime = get_tick();		
	}
	else{
		tmr_func = NULL;
		memset(&soft_tmr, 0, sizeof(soft_tmr));
	}
}

void tmr_task_A(void){
	printf("at:%d\ttask A running\n\n",get_tick());
}

void tmr_task_B(void){
	printf("at:%d\ttask B running\n\n",get_tick());
}

int main(void){
	u8 n = 10;
	printf("sec: %d\n",get_tick());
	tmr_regist(TMR_A, TASK_A_OVERTIME, TASK_A_REPEAT, tmr_task_A);
	while(1){
		if(true == soft_tmr.tmr_busy){
			if(get_tick() - soft_tmr.starttime >= soft_tmr.overtime){
				tmr_call();
			}
		}
		else
			break;
	}
	
	printf("sec: %d\n",get_tick());
	tmr_regist(TMR_B, TASK_B_OVERTIME, TASK_B_REPEAT, tmr_task_B);
	while(1){
		if(true == soft_tmr.tmr_busy){
			if(get_tick() - soft_tmr.starttime >= soft_tmr.overtime){
				tmr_call();
			}
		}
		else
			return 0;
	}	
	return 0;
}
