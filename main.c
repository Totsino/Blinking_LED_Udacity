#include "Std_Types.h"
#include "Led.h"
#include "tm4c123gh6pm_registers.h"
#include "gpio.h"
#include "Port.h"

void Clock_init(void);

void Clock_init(void){
  SYSCTL_REGCGC2_REG|=(0x3F);
  volatile unsigned long delay = 0;
  delay = SYSCTL_REGCGC2_REG;

}



int main(){
while(1){
 Clock_init();
 Port_Init(&Port_Configuration);
 Dio_Init(&Dio_Configuration);
 LED_setOn();


}


}
