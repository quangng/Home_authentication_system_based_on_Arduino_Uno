// Do not remove the include below
#include "test_new_lib.h"

//Object declarations
SMSGSM sms;
CallGSM call;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Global variables
uint8_t hwSeriali;
char inSerial[35];
uint8_t i = 0;
uint8_t j = 0;
uint8_t	mesgLen = 0;
uint8_t row = 0;

char newMessage[50];
char newPhoneNumber[15];
char newSenderName[15];
char newDateTime[18];

char oldMessage[50];
char oldPhoneNumber[15];
char oldSenderName[15];
char oldDateTime[18];

// Arduino UNO has only 2KB of valuable SRAM. 
// The application uses so much RAM already, so to prevent stack overflow. 
// A list of maximum 5 unauthorized numbers can be stored.
char unauthorizedNumbers[5][15]; 


char newPosition = -1;
char oldPosition = 0;

char lcdBuffer1[17];
char lcdBuffer2[17];

unsigned long tick = 0;
uint8_t count = 0;
uint8_t index = 0;

void setup() {
	//lcd initialization
	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	lcd.print("Initializing...");

     //Serial connection.
     Serial.begin(9600);
     Serial.println("GSM Shield testing.");

     if (gsm.begin(9600))
          Serial.println("\nstatus=READY");
     else Serial.println("\nstatus=IDLE");
     gsm.SendATCmdWaitResp("AT+CPIN=****", 5000, 2000, "OK", 1); // **** is the PIN code
     gsm.InitParam(PARAM_SET_0);
     gsm.InitParam(PARAM_SET_1);
     Serial.println("GPRS module initialization OK");
     lcd.setCursor(0, 0);
     lcd.print("GPRS shield OK");
     delay(1000);
};

void loop() {
	serialhwread();
	serialswread();

	//Handle incoming call
	handleCall();
	//Handle incoming SMS
	handleSMS();
};


void serialhwread() {
	hwSeriali=0;

	// Get what is received from the serial terminal (console)
	if (Serial.available() > 0) {
		while (Serial.available() > 0) {
			inSerial[hwSeriali]=(Serial.read());
			delay(10);
			hwSeriali++;
		}
		inSerial[hwSeriali]='\0';

		// Check for request from the serial terminal
		if(!strcmp(inSerial,"SERIAL LIST UNAUTHORIZED NUMBERS")) {
			Serial.println("Request: Send a list of unauthorized numbers via serial line");
			for (uint8_t loop = 0; loop < row+1; loop++) {
				Serial.println(unauthorizedNumbers[loop]);
			}
		}
		if (!strcmp(inSerial,"SMS LIST UNAUTHORIZED NUMBERS")) {
			Serial.println("Request: Send a list of unauthorized numbers via SMS");
			char smsMesg[75];
			uint8_t k = 0;
			for (uint8_t i = 0; i < 5; i++) {
				for (uint8_t j = 0; j < 15; j++) {
					if (unauthorizedNumbers[i][j] == '\0')
						break;
					smsMesg[k++] = unauthorizedNumbers[i][j];
				}
			}
			smsMesg[k] = '\0';
			Serial.println(smsMesg);
			if(sms.SendSMS("+358*********", smsMesg)) { //********** is the phone number to which the SMS will be sent
				Serial.println("SMS was sent successfully");
			} else {
				Serial.println("Error: Cannot send SMS");
			}

		} else {
			gsm.SimpleWriteln(inSerial);
		}
		inSerial[0]='\0';
	}
}

void serialswread() {
	gsm.SimpleRead();
}

void handleLCDDisplay(uint8_t message_length, const char *msg) {
	if (message_length <= 16) {
		lcd.clear();
		for (i = 0; i < 16; i++)
			lcdBuffer1[i] = msg[i];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		delay(2000);
	} else if (message_length > 16 && message_length <= 32) {
		lcd.clear();
		for (i = 0; i < 16; i++)
			lcdBuffer1[i] = msg[i];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		i = 0;
		for (uint8_t j = 16; j < message_length; j++)
			lcdBuffer2[i++] = msg[j];
		lcdBuffer2[i] = '\0';
		lcd.setCursor(0, 1);
		lcd.print(lcdBuffer2);
		delay(2000);
	} else if (message_length > 32 && message_length <= 48) {
		lcd.clear();
		for (i = 0; i < 16; i++)
			lcdBuffer1[i] = msg[i];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		i = 0;
		for (uint8_t j = 16; j < 32; j++)
			lcdBuffer2[i++] = msg[j];
		lcdBuffer2[i] = '\0';
		lcd.setCursor(0, 1);
		lcd.print(lcdBuffer2);
		delay(2000);
		lcd.clear();
		i = 0;
		for (uint8_t k = 32; k < message_length; k++)
			lcdBuffer1[i++] = msg[k];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		delay(2000);
	} else {
		lcd.clear();
		for (i = 0; i < 16; i++)
			lcdBuffer1[i] = msg[i];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		i = 0;
		for (uint8_t j = 16; j < 32; j++)
			lcdBuffer2[i++] = msg[j];
		lcdBuffer2[i] = '\0';
		lcd.setCursor(0, 1);
		lcd.print(lcdBuffer2);
		delay(2000);
		lcd.clear();
		i = 0;
		for (uint8_t k = 32; k < 48; k++)
			lcdBuffer1[i++] = msg[k];
		lcdBuffer1[i] = '\0';
		lcd.setCursor(0, 0);
		lcd.print(lcdBuffer1);
		i = 0;
		for (uint8_t l = 48; l < message_length; l++)
			lcdBuffer2[i++] = msg[l];
		lcdBuffer2[i] = '\0';
		lcd.setCursor(0, 1);
		lcd.print(lcdBuffer2);
		delay(2000);
	}
}


void handleCall(void) {
	if (call.CallStatusWithAuth(newPhoneNumber, 6, 6) == CALL_INCOM_VOICE_AUTH) {

		gsm.GetNameInPhoneBook(6, newSenderName);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Authorized");
		lcd.setCursor(0, 1);
		lcd.print(newSenderName);
		delay(2000);

	} else if (call.CallStatusWithAuth(newPhoneNumber, 6, 6) == CALL_INCOM_VOICE_NOT_AUTH){
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Unauthorized");
		lcd.setCursor(0, 1);
		if (strcmp(newPhoneNumber, "") == 0) { //the calling number is an unknown number
			if ((millis() - tick) > 4500 || count == 0) { //time elapsed between 2 successive appearances of +clip is about 3.73 s
				while (strcmp(unauthorizedNumbers[index], "unknown number") != 0) { //unknown number is not stored in this row
					if (strcmp(unauthorizedNumbers[index], "") == 0) {	//check if this row in empty. If empty, store unknown number there
						strcpy(unauthorizedNumbers[index], "unknown number");
						row = index;
						if (row == 5)
							row = 0;
					} else {
						index++;
						if (index == 5)
							index = 0;
					}
				}
				index = 0;
			}
			lcd.print("Unknown number");
		} else {
			if ((millis() - tick) > 4500 || count == 0) {
				while (strcmp(unauthorizedNumbers[index], newPhoneNumber) != 0) { //new number is not stored in this row
					if (strcmp(unauthorizedNumbers[index], "") == 0) {
						strcpy(unauthorizedNumbers[index], newPhoneNumber);
						row = index;
						if (row == 5)
							row = 0;
					} else {
						index++;
						if (index == 5)
							index = 0;
					}
				}
				index = 0;
			}
			lcd.print(newPhoneNumber);
		}
		delay(2000);
		tick = millis();
	} else {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Wait new call");
		delay(500);
	}
}


void handleSMS(void) {
	newPosition = sms.IsSMSPresent(SMS_UNREAD);
	if (newPosition) {
		sms.GetSMS(newPosition, newPhoneNumber, newMessage, newSenderName, newDateTime, 50);
		//Display " *** NEW SMS ***" on the fist line of lcd
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(" ***NEW SMS*** ");
		delay(1000);

		if (strcmp(newMessage, "UPDATE_STATE") == 0) {
			//Display string UPDATE_STATE on the fist line of lcd
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("UPDATE_STATE");
			delay(1000);

			//Display sender, date and time on the first line
			lcd.clear();
			for (j = 0; j < strlen(newSenderName); j++)
				lcdBuffer1[j] = newSenderName[j];
			lcdBuffer1[j] = '\0';
			lcd.setCursor(0, 0);
			lcd.print(lcdBuffer1);

			for (j = 0; j < strlen(newDateTime); j++)
				lcdBuffer2[j] = newDateTime[j];
			lcdBuffer2[j] = '\0';
			lcd.setCursor(0, 1);
			lcd.print(lcdBuffer2);
			delay(5000);

			//Display old SMS in SIM memory from 1 to 20
			oldPosition++;
			if (oldPosition == 21)
				oldPosition = 1;
			sms.GetSMS(oldPosition, oldPhoneNumber, oldMessage, oldSenderName, oldDateTime, 50);
			mesgLen = strlen(oldMessage);
			handleLCDDisplay(mesgLen, oldMessage);
		} else if (strcmp(newMessage, "UNAUTHORIZED_NUMBERS") {
			/*
			 * Adding new feature that was not shown in the demonstration.
			 * Send an SMS from mobile phone to the GPRS shield to request for a list of unauthorized numbers that have called to the shield
			 * Send this list back to the mobile phone
			 */
			Serial.println("Sending a list unauthorized numbers to mobile phone via SMS");
			char smsMesg[75];
			uint8_t k = 0;
			for (uint8_t i = 0; i < 5; i++) {
				for (uint8_t j = 0; j < 15; j++) {
					if (unauthorizedNumbers[i][j] == '\0')
						break;
					smsMesg[k++] = unauthorizedNumbers[i][j];
				}
			}
			smsMesg[k] = '\0';
			Serial.println(smsMesg);
			if(sms.SendSMS("+358*********", smsMesg)) { //********** is the phone number to which the SMS will be sent
				Serial.println("SMS was sent successfully");
			} else {
				Serial.println("Error: Cannot send SMS");
			}
		} else{
			mesgLen = strlen(newMessage);
			handleLCDDisplay(mesgLen, newMessage);
			//Display sender, date and time on the first line
			lcd.clear();
			for (j = 0; j < strlen(newSenderName); j++)
				lcdBuffer1[j] = newSenderName[j];
			lcdBuffer1[j] = '\0';
			lcd.setCursor(0, 0);
			lcd.print(lcdBuffer1);

			for (j = 0; j < strlen(newDateTime); j++)
				lcdBuffer2[j] = newDateTime[j];
			lcdBuffer2[j] = '\0';
			lcd.setCursor(0, 1);
			lcd.print(lcdBuffer2);
			delay(5000);

		}
	}
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Wait new SMS");
	delay(500);
	newPosition = -1;
}































