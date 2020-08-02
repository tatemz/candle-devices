/*
 * Candle Garage Door Opener
 *
 * This is an Arduino Sketch that can be used to control a garage door by
 * connecting the control wires via a simple relay.
 *
 * SETTINGS */

// You can enable and disable the settings below by adding or removing double slashes ( // ) in front of a line.

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the local Candle network? For privacy or security reasons you may prefer a stand-alone device.
#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.
//#define MY_REPEATER_FEATURE                       // Act as a repeater? The devices can pass along messages to each other to increase the range of your network.

/* END OF SETTINGS
*
*
*
*/

// PINS
#define BUTTON_PIN 5                                // The receive (RX) pin for the touchscreen. This connects to the transmit (TX) pin of the touchscreen.
#define RELAY_PIN 6                                 // The receive (TX) pin for the touchscreen. This connects to the transmit (RX) pin of the touchscreen.

// Misc settingss
#define RELAY_DELAY 200                             // Milliseconds used to toggle the garage control wire connection.
#define RADIO_DELAY 100                             // Milliseconds betweeen radio signals during the presentation phase.
#define ALLOW_CONNECTING_TO_NETWORK

// Variables
boolean connectedToNetwork = false;                 // Are we connected to the local MySensors network? Used to display the 'w' connection icon.
int buttonState = 0;                                // The pushbutton status

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif

// Enable and select the attached radio type
#define MY_RADIO_RF24                               // This is a common and simple radio used with MySensors. Downside is that it uses the same frequency space as WiFi.
//#define MY_RADIO_NRF5_ESB                         // This is a new type of device that is arduino and radio all in one. Currently not suitable for beginners yet.
//#define MY_RADIO_RFM69                            // This is an open source radio on the 433mhz frequency. Great range and built-in encryption, but more expensive and little more difficult to connect.
//#define MY_RADIO_RFM95                            // This is a LoRaWan radio, which can have a range of 10km.

// MySensors: Choose your desired radio power level. High power can cause issues on cheap Chinese NRF24 radio's.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors security
#define MY_ENCRYPTION_SIMPLE_PASSWD "dJbvk3_MK6"     // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // The datarate influences range. 1MBPS is the most widely supported. 250KBPS will give you more range.
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.

// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library

void setup()
{
  Serial.begin(115200);                             // Start serial output of data.
  delay(1000);                                      // Wait for serial connection to be initiated
  Serial.println(F("Hello, I am a garage door control device"));
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

#ifdef ALLOW_CONNECTING_TO_NETWORK
  if ( isTransportReady() ) {                       // Check if a network connection has been established
    Serial.println(F("Connected to gateway!"));
    connectedToNetwork = true;
#ifdef HAS_DISPLAY
    // Show connection icon on the display
    oled.setCursor(85, 0);
    oled.print(F("W"));
#endif

  }
  else {
    Serial.println(F("! NO CONNECTION"));
    connectedToNetwork = false;
  }
#endif

}

// The loop function runs over and over again forever
void loop()
{
  digitalWrite(RELAY_PIN, LOW);

  // Handle button press to open garage door
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) {
    toggleGarageDoor();
  }
}


void presentation()
{
  sendSketchInfo(F("Garage Door Opener"), F("1.0")); wait(RADIO_DELAY); // Child 0. Sends the sketch version information to the gateway and Controller
  present(BUTTON_PIN, S_BINARY, F("Toggle the garage door")); wait(RADIO_DELAY); // Child 1
}

void receive(const MyMessage &message)
{
  Serial.print(F("INCOMING MESSAGE for child #")); Serial.println(message.sensor);
  if (message.isAck()) {
    Serial.println(F("-Ack"));
  }
  else if( message.type == V_STATUS ) {
    Serial.print(F("-Requested status: ")); Serial.println(message.getBool());

    if( message.sensor < 10 && message.getBool() ){
      if( message.sensor == BUTTON_PIN ){  // The user wants to toggle the garage door. This only starts when the button is toggled to on.
        toggleGarageDoor();
      }
    }
  }
}

void toggleGarageDoor()
{
  digitalWrite(RELAY_PIN, HIGH);
  delay(RELAY_DELAY);
  Serial.println("I just toggled the door");
}
