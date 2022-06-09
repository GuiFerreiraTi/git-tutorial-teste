#include "app/framework/include/af.h"

/*** Input Pins ***/
#define RESET 								BUTTON0
/*** Output Pins ***/
#define LED_STATUS 							BOARDLED1
#define LED_OUTPUT							BOARDLED2
/*** ZigBee Constants ***/
#define DEVICE_ENDPOINT (1)
#define GATEWAY_ENDPOINT (8)
/*** Timers and Delays ***/
#define SEC_RST_ZIG 						3		//seconds to reset zigbee
#define PERIODICALLY_REPORT_STATE			300000  //Periodically report
/*** LED ***/
#define LED_LONG_ON_DELAY					3000
#define LED_LONG_BLINK_PERIOD_MS			700
#define LED_SHORT_BLINK_PERIOD_MS 			200
#define LED_BLINK_TIMES_WHEN_START			3
#define LED_BLINK_TIMES_FAST				(30000 / LED_SHORT_BLINK_PERIOD_MS)
#define	LED_BLINK_TIMES_MACRO(times)		(times * 2)
/*** Resets ***/
#define SEC_RST_ZIG 						3
/*** State ***/
#define ON 									0x10
#define OFF 								0xEF


EmberEventControl inputActionEventControl;
EmberEventControl ledEventControl;
EmberEventControl reportStatusEventControl;
EmberEventControl outputControllerEventControl;
/*** Status vars ***/
static uint8_t statusOnOff;
/*** Control vars ***/
/*** Button ***/
static bool timerIniciado = false;
static bool buttonPressed = false;
/*** LED ***/
static bool ledLeavingNetwork = false;
static bool ledStopSearchNetwork = false;
static bool ledOnAction = false;
static bool estavaAtivo = false;
static bool TIMER = false;
static bool LigarLed = false;

// booleano estavaAtivo false
static uint8_t ledStartBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_WHEN_START);
static uint8_t ledFastBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_FAST);


/*** Reset ***/
void resetZigbee(void);
/*** Send Data ***/
void sendStateFunction(void);
/*** Output Controllers ***/
void outputController(void);


void inputActionEventFunction(void)
{

	emberEventControlSetInactive(inputActionEventControl);
	//halCommonDelayMilliseconds(2);
	emberAfCorePrintln(">>>>>>>> RESET COUNTER: %d", timerIniciado);
	emberAfCorePrintln(">>>>>>>> RESET BUTTON: %d", buttonPressed);

	if(buttonPressed){
		//emberEventControlSetDelayMS(inputActionEventControl, 3*1000);

		if(timerIniciado != true) {
			//Start 3 seconds timer
			timerIniciado = true;
			TIMER = true;
			//emberEventControlSetDelayMS(inputActionEventControl, 3*1000);
			emberEventControlSetDelayMS(outputControllerEventControl, 3*1000);
			emberEventControlSetActive(outputControllerEventControl);
        }
		else {
			timerIniciado = false;
			TIMER = false;
		     }

	 }else {
			//emberEventControlSetDelayMS(inputActionEventControl, 3*1000);
			//emberEventControlSetActive(outputControllerEventControl);
			//TIMER = false;
      }

	if(emberAfNetworkState() == EMBER_JOINED_NETWORK_NO_PARENT)
		emberAfPluginConnectionManagerRejoinEventHandler();
}

void ledEventFunction(void)
{
	//se estava ativo -> desativar, estava ativo falso
    if(estavaAtivo == true){

    	 halSetLed(LED_OUTPUT);
    	 emberEventControlSetDelayMS(ledEventControl, 300);
	     estavaAtivo = false;
    }
     else if(LigarLed == true){	//se nao estava ativo -> ativar, esta ativo

    	  halClearLed(LED_OUTPUT);//muda estado do led
  		  emberEventControlSetDelayMS(ledEventControl, 300);
	      estavaAtivo = true;
     } //emberEventControlSetActive(ledEventControl); //led event function
     else {

   	      halClearLed(LED_OUTPUT);

     }
}

void outputControllerEventFunction(void)
{

	if(TIMER == false) {
		TIMER = true;
        emberEventControlSetDelayMS(outputControllerEventControl, 3000);
        halClearLed(LED_STATUS);
        LigarLed = false;

	} else {
        //emberEventControlSetDelayMS(outputControllerEventControl, 3000);
        halSetLed(LED_STATUS);
        LigarLed = true;
        emberEventControlSetActive(ledEventControl);
	}
}