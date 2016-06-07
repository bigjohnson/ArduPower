/*			          *
 *  Arduino Power Ethernet Server *
 *	    11 May 2011           *
 *         Basic Arduino          *
 *     power ethernet server      *
 *   a command line interface     *
 *        by Alberto Panu         *
 *      alberto[at]panu.it        *
 *       based on work of         *
 *         Steve Lentz            *
 *    stlentz[at]gmail[dot]com    *

    copyright Alberto Panu 2012
    
    Ardupower http://www.panu.it/ardupower/
    
    Your comments and suggestion are welcomed to alberto[at]panu.it
    
    Released under GPL licence V3 see http://www.gnu.org/licenses/gpl.html and file gpl.txt

 Quick Start Instructions:
  1) Compile and upload sketch.
  2) Power of the Arduino.
  3) Connect the defParamSwitch pin (search on code the pin number) to ground (0V).
  4) Power on the Arduino.
  5) Wait a minute.
  6) Power off the Arduino.
  7) Disconnect the defParamSwitch from ground.
  8) Pover on the Arduino and connect the ethernet cable, make sure that link led is on.
  9) Telnet to the default Ethernet address 192.168.1.2, you may need some client tcp setup.
 10) On some Telnet clients, hit return to wake up connection.
 11) Enter the default password password.
 12) When connected, type ? <cr> for help.
 13) Try a simple command such as 'show'.
 14) If you need a different tcp address change it with command tcpsetup.
 15) Reconnect to ArduPower with new tcp address.
 16) Change the password with command newpw.
 
     If you lose password or ip you could reset to the default password "password" and ip address "192.168.1.2"
     shorting defParamSwitch pin ad power on.
 
 =====================================================================================
 
     WARNNG the first time you load the sketch on a new Arduino board reset to default 
     password and IP shorting defParamSwitch pin to ground!

 =====================================================================================

Other notes
Tested on Arduino Uno with Ethernet Shield.
Should work on compatible boards.
Tested with Win XP putty and Windows telnet, OS X, and Debian Telnet clients.
I am an entirely self-taught C programmer; if you
  don't like my code, too bad ;-). 

V0.1 
- First release, compatible with Arduino 1.0.6
  
V0.2 changes:
- Compatible with Arduino 1
- Put message strings in program memory
- Insert telnet commands for hide password typing
- Removed some bugs

v0.3 changes:
- Remove some telnet commands password related bug
- Removed MyEthernet library, used new Ethernet fuctions

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

// No Debug
#define DEBUG 0

// All debug
//#define DEBUG 2

//YOU MUST DEFINE DEBUG if you don't wont debug define it at 0

//#include <Dhcp.h>
//#include <Dns.h>
#include <Ethernet.h>
//#include <EthernetClient.h>
#include <EthernetServer.h>
//#include <EthernetUdp.h>

#include <EEPROM.h>

// Mac addres variable
// ---------- CHANGE IT WITH YOUR CARD MAC ADDRESS --------

byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x36, 0xE8 };

// Default values for tcp/ip parameters

byte DefaultIp[] = { 192, 168, 1, 2 };
byte DefaultSubnet[] = { 255, 255, 255, 0 };
byte DefaultGateway[] = { 192, 168, 1, 1 };

#define defaultpassword "password"
#define defaultpasswordlenght 8

// Tcp parameters variables
byte ip[4];
byte gateway[4];
byte subnet[4];

// Other global variables
#define textBuffSize 60 //length of longest command string plus two spaces for CR + LF
char textBuff[textBuffSize]; //someplace to put received text
int charsReceived = 0;

// Authentication variables
#define pwdLenght           11  // Maxiumum password lenght + end of string.
#define minpwdlenght         5  // Minimal password lenght
#define pwdCommandLenght     6  // newpw= string lenght
// There are two password location for check password consistency
#define pwd1Location       288  // First password location in EEPROM
#define pwd2Location       304  // Seond password location in EEPROM

// Command lenghts
#define renameCommandLenght 8
#define portNameLenght 41   // maximum power line name
#define tcpSetupCommandLenght 9

// EEProm addresses
#define portNameAddressBase 24
#define ipEprom 272
#define subnetEprom 276
#define gatewayEprom 280

// used for see if a client send a valid password and is logged in
boolean logged = false;

//we'll use a flag separate from client.connected so we can recognize when a new connection has been created
boolean connectFlag = false; 

// Timeout variable
unsigned long timeOfLastActivity; //time in milliseconds of last activity

// Command and password timeout for disconnection
unsigned long allowedConnectTime = 300000; //five minutes
unsigned long allowedPasswordTime = 30000; 

// frduino pin usage
#define defParamSwitch 9 // the input pin that reset the password and the parameters when put it to low
#define statusLed 8   // the output led pin that become high when tcp/ip telnet server is ready
#define firstpin 2    // the first io pin
#define lastpin 7     // the last io pin

// Telnet server listens on port 23
EthernetServer server(23);

// Client needs to have global scope so it can be called
// from functions outside of loop, but we don't know
// what client is yet, so creating an empty object
EthernetClient client = 0; 
	     
// used for remember wich is the active client and disconnect the other
// the system support only one open connection a time.
byte clientID;

byte newpos;

// Help message in prongram memory
#define he_00 "Examples of supported commands:\r\n\r\n"
#define he_01 "  pow1=on           -set   power line 1 on, valid lines are 1 to 6\r\n"
#define he_02 "  pow1=off          -set   power line 1 off, valid lines are 1 to 6\r\n"
#define he_03 "  all=on            -set   all power lines on\r\n"
#define he_04 "  all=off           -set   all power lines off\r\n"
#define he_05 "  newpw=password    -set   new pasword, min 5 and max 10 characters,\r\n"
#define he_06 "                           passwords too long will be truncated\r\n"
#define he_07 "  rename1=name      -set   new name for a main line, valid lines are 1 to 6\r\n"
#define he_08 "                           max mane lenght 40 characters,\r\n"
#define he_09 "                           names too long will be truncated\r\n"
#define he_10 "  tcpsetup=         -set   tcp/ip parameters in the form of\r\n"
#define he_11 "                           IP.IP.IP.IP,SUB.SUB.SUB.SUB,GW.GW.GW.GW\r\n"
#define he_12 "                           example: 192.168.1.10,255.255.255.0,192.168.1.1\r\n"
#define he_13 "                           if the new ip is valid the new settings\r\n"
#define he_14 "                           will be applied immediatly\r\n"
#define he_15 "  show              -get   all port status\r\n"
#define he_16 "  ipstatus          -get   the tcp/ip parameters\r\n"
#define he_17 "  exit              -close connection\r\n"
#define he_18 "  ? or h            -print this help message\r\n\r\n"
#define he_19 "Shell timeout is five Arduno's minutes\r\n\r\n"
#define he_20 "WARNING: ALL COMMANDS ARE EXECUTED IMMEDIATELY, WITHOUT A CONFIRMATION REQUEST!"

// =========================== setup: arduino's setup function >>
void setup()
{
  
  #if DEBUG > 0
    Serial.begin(115200);
  #endif
  
  // pins 10-13 are used by the Ethernet Shield
  
  // setting password and tcp parameters defParamSwitch pin as input
  pinMode(defParamSwitch, INPUT_PULLUP);
  
  // setting status led as output and power off it
  pinMode(statusLed, OUTPUT);
  digitalWrite( statusLed, LOW );
  
  // setting pins firstpin to lastpin as outputs and restore eeprom saved status
  for(byte  i = firstpin; i <= lastpin; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, EEPROM.read(i));
  }
  
  // if the defParamSwitch is LOW restore password and tcp parameters todefaul values
  if ( digitalRead( defParamSwitch ) == LOW ) {
    
    writeEEPROMOctet(ipEprom, DefaultIp);
    writeEEPROMOctet(gatewayEprom, DefaultGateway);
    writeEEPROMOctet(subnetEprom, DefaultSubnet);
    
    char resetpw[] = defaultpassword;
    
    for ( byte i = 0; i <= defaultpasswordlenght; i++ ) {
      EEPROM.write(pwd1Location + i, resetpw[i]);
      EEPROM.write(pwd2Location + i, resetpw[i]);
    }
   
  }
  
  for (int i= 0; i <= 3; i++ ) {
    ip[i] = EEPROM.read ( ipEprom + i );
    gateway[i] = EEPROM.read ( gatewayEprom + i );
    subnet[i] = EEPROM.read ( subnetEprom + i );
  }
  
  //Ethernet.begin(mac, ip, gateway, subnet);
  //server.begin();
  startServer();
  digitalWrite( statusLed, HIGH );
}

// =========================== end setup <<

// =========================== loop: main arduino loop >>
void loop()
{
  
  // look to see if a new connection is created,
  // print welcome message, set connected flag
  if (server.available() && !connectFlag) {
    connectFlag = true;
    client = server.available();
    clientID = client.getSocketNumber();
            
      client.println(F("\nArdupower Ethernet power switch server."));

      client.write(255);
      client.write(251);
      client.write(1);
      
      client.write(255);
      client.write(251);
      client.write(3);
      
      client.write(255);
      client.write(253);
      client.write(3);
      
      unsigned long time = millis();
      while ( millis( ) < (time + 500)) {
        char available = client.available() ;
        if ( available ) {
          for (int fai= 1; fai <= available; fai++ ) {
          char pippo = client.read();
          }
          //char away = client.read();
        }
      }    
          
    //client.println(F("\nArdupower Ethernet power switch server."));
    if ( passwordConsistence() ) {
      client.println(F("Please enter password."));
      //stop local client telnet echo
/*      client.write(255);
      client.write(251);
      client.write(1);
      
      client.flush();
            
      client.write(255);
      client.write(251);
      client.write(3);
      
      client.flush();
      
      client.write(255);
      client.write(253);
      client.write(3);
      
      client.flush();
      */
      
    } else {
     logged = true; 
    }
    printPrompt();
  } else if (server.available()) {
    EthernetClient badclient = server.available();
    byte tempid = badclient.getSocketNumber();
    if ( tempid != clientID) {
      badclient.println(F("\nArdupower Ethernet power switch server."));
      badclient.println(F("Sorry, only one client at time allowed!"));
      badclient.flush();
      badclient.stop();
    }
  }

  // check to see if text received
  if (client.connected() && client.available()) getReceivedText();

  // check to see if connection has timed out
  if(connectFlag) checkConnectionTimeout();

  if (connectFlag && !client.connected()) {
    client.flush();
    client.stop();
    logged = false;
    connectFlag = false;
  }

}

// =========================== end loop <<

// =========================== startServer: Start the telnet server >>
void startServer() {
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}
// =========================== end startServer <<

// =========================== writeEEPROMOctet: write a tcp/ip address and subnet mask octet to eeprom >>
void writeEEPROMOctet ( int address, byte *values ) {
  for (byte i = 0; i <= 3; i++) {
    EEPROM.write( address + i, values[i] );
    // client.print(address+i);
    // client.print(" | ");
    // client.println((int) values[i]);
  }
}
// =========================== end writeEEPROMOctet <<

// =========================== readIpV4: read a tcp/ip address and subnet mask from eeprom and prin it to client >>
void readIpV4(unsigned int address) {
  
  for (byte i= 0; i <= 3; i++ ) {
    /*byte octet = EEPROM.read ( address + i );
    if ( octet / 100 ) client.print(octet / 100);
    octet = octet % 100;
    if ( octet / 10 ) client.print(octet / 10);
    octet = ( octet % 10);
    if ( octet / 1 ) client.print(octet);*/
    client.print((int) EEPROM.read ( address + (int) i ));
    if ( i != 3 ) {
      client.print(".");
    }
    
  }
  
}
// =========================== end readIpV4: <<

// =========================== passwordConsistence: This function check the password consistence 
//                             comparing two password saved in different EEPROM locations. >>
boolean passwordConsistence() {
  byte j = 0;
  //passwordok = true;
  char pwd1;
  do {
    pwd1 = EEPROM.read(j + pwd1Location);
    char pwd2 = EEPROM.read(j + pwd2Location);
    if ( pwd1 != pwd2) {
      return false;
      break;
    }
    j++;
  } while ( pwd1 != 0);
  return true;
}
// =========================== passwordConsistence <<

// =========================== printPrompt: print the prompt and manage the session data >>
void printPrompt()
{
  timeOfLastActivity = millis();
  client.flush();
  charsReceived = 0; //count of characters received
  for (byte count=0; count < textBuffSize; count++) {
     textBuff[count]='\0';
  }
  client.print("\n>");
}
// =========================== end printPrompt <<

// =========================== checkConnectionTimeout: check the timeouts and if need disconnect the client >>
void checkConnectionTimeout()
{
  unsigned long myallowedConnectTime;
  if (logged) {
    myallowedConnectTime = allowedConnectTime;
  } else {
    myallowedConnectTime = allowedPasswordTime;
  }
  
  if(millis() < timeOfLastActivity) {
    timeOfLastActivity = millis();
  }
  
  if(millis() - timeOfLastActivity > myallowedConnectTime) {
    client.println();
    client.println(F("Timeout disconnect."));
    client.flush();
    client.stop();
    logged = false;
    connectFlag = false;
  }
}
// =========================== end checkConnectionTimeout <<

// =========================== getReceivedText: get and check the client's recived text >>
void getReceivedText()
{
  char c;
  int charsWaiting;

  // copy waiting characters into textBuff
  //until textBuff full, CR received, or no more characters
  charsWaiting = client.available();
  do {
    c = client.read();
    
    #if DEBUG >= 2
      Serial.println("getReceivedText");
      Serial.println(c);
    #endif
    
    if (c != 0 && c != 0xa ) {
    if (logged) {
      client.write(c);
    } else {
      client.print(F("*"));
    }
    
      textBuff[charsReceived] = c;
      charsReceived++;
    }
   #if DEBUG >= 2
      Serial.println("charsReceived");
      Serial.println(charsReceived);
      Serial.print(textBuff[charsReceived]);
      Serial.println();
      Serial.println(textBuff);
   #endif
    
    
    charsWaiting--;
    

  }
  while(charsReceived <= textBuffSize && c != 0x0d && charsWaiting > 0);

    
  //if CR found go look at received text and execute command
  if(c == 0x0d) {
    // if the client is logged in show the prompt an wait for commands,
    // if the client is not logged in check the password recived
      if ( logged ) {
        client.println("");
        parseReceivedText();
        // after completing command, print a new prompt
        printPrompt();
    } else {
        checkPassword();
    }
  }

  // if textBuff full without reaching a CR, print an error message
  if(charsReceived >= textBuffSize) {
    client.println();
    printErrorMessage();
    printPrompt();
  }
  // if textBuff not full and no CR, do nothing else;
  // go back to loop until more characters are received
}

// =========================== end getReceivedText <<

// =========================== checkPassword: check the password recived from client >>
void checkPassword() {
  
  //printHelpMessage();
  
  logged = true;
  byte j = 0;
  char pwchar;
  do {
    pwchar = EEPROM.read(j + pwd1Location);
    if ( pwchar != textBuff[j] && pwchar != 0) {
      client.println();
      client.println(F("Bad password retry!"));
      closeConnection();
      return;
    }
    j++;
  } while (pwchar);
  client.println("");
  client.println(F("Wellcome!"));
  client.println("");
  readPowerStatus();
  client.println(F("? for help."));
  printPrompt();
}
// =========================== end checkPassword <<

// =========================== parseReceivedText: parse the recived text and go to command execution >>
void parseReceivedText()
{
  // look at first character and decide what to do
  //client.println("");
  #if DEBUG >= 2
    Serial.println("parseReceivedText");     
    Serial.println(textBuff);
  #endif
   
  switch (textBuff[0]) {
    /*case 'a' : doAnalogCommand();	  break;
    case 'd' : doDigitalCommand();	 break;
    case 'p' : setPinMode();		 break;*/
    case 'e' : checkCloseConnection();    break;
    case 'p' : doPowerCommand();          break;
    case 'a' : doPowerAll();              break;
    case 'n' : doSetPassword();           break;
    case 's' : readPowerStatus();         break;
    case 'i' : ipInfo();                  break;
    case 'r' : renameLine();              break;
    case 't' : tcpSetup();                break;
    case '?' : printHelpMessage();	  break;
    case 'h' : printHelpMessage();	  break;
    case 0x0d :				  break;  //ignore a carriage return
    default: printErrorMessage();	  break;
  }
}
// =========================== end parseReceivedText <<

// =========================== renameLine: change a port description text >>
void renameLine() {

  byte pin = checkPin(6);
  
  if ( textBuff[renameCommandLenght + 1] && 
       pin && 
       textBuff[1] == 'e' &&
       textBuff[2] == 'n' &&
       textBuff[3] == 'a' &&
       textBuff[4] == 'm' &&
       textBuff[5] == 'e' &&
       textBuff[7] == '='
       ) {
    char textName[portNameLenght];
    int i=0;
    client.print(F("Rename Line ")); 
    client.println(pin);
    while (i < ( portNameLenght - 1 ) && textBuff[ i + renameCommandLenght ] != 0x0d) {
      char pwChar = textBuff[ i + renameCommandLenght ];
      //client.print(pwChar);
      textName[i]=pwChar;
      i++;
    }
    //if (i >= minpwdlenght ) {
      textName[i]=0;
      
      // Write password to two location for consistence
      // if there are some problem in writing whe see the difference
      int adress = portNameAddressBase + ( ( portNameLenght * pin) - portNameLenght);
      for (byte j = 0; j <= i; j++) {
        EEPROM.write(j + adress, textName[j]);
      }
      
      client.print(F("The new power line "));
      client.print(pin);
      client.print(F(" name is \""));
      client.print(textName);
      client.println(F("\""));
    /*}else {
      client.println("Error password too short!");
    }  */ 
 
  } else {
     printErrorMessage();
  }
  
}
// =========================== end renameLine <<

// =========================== printStatus: print the actual port status >>
void printStatus(byte status) {
   if (status)
      {
      client.print(F("ON  "));
   } else {
      client.print(F("OFF "));
   }
}

void readPowerStatus() {
  client.println(F("Power status:"));
  client.println("");
 for (byte  i = lastpin; i >= firstpin; i--)  {
   client.print("    ");
   client.print(8-i);
   client.print(F(" "));
   byte status = EEPROM.read(i);
   printStatus(status);
   client.print(F("==> "));
  readPortName(i); 
  client.print(F(" <== "));
  printStatus(status);
  client.println("");
  client.println("");
 }
}
// =========================== end printStatus <<

// =========================== readPortName: read a port description from eeprom >>
void readPortName(byte port) {
  byte j = 0;
  char nameChar;
  do {
    int adress = portNameAddressBase + ( ( portNameLenght * (8-port)) - portNameLenght) + j;
    nameChar = EEPROM.read(adress);
    client.print(nameChar);
    j++;
  } while  (j <= portNameLenght && nameChar != 0);
}
// =========================== readPortName <<

// =========================== doSetPassword: set the new password >>
void doSetPassword() {
  
  if ( textBuff[pwdCommandLenght + 1] && 
       textBuff[1] == 'e' &&
       textBuff[2] == 'w' &&
       textBuff[3] == 'p' &&
       textBuff[4] == 'w' &&
       textBuff[5] == '='
     ) {
    char textPwd[pwdLenght];
    byte i=0;
    while (i < ( pwdLenght - 1 ) && textBuff[ i + pwdCommandLenght ] != 0x0d) {
      char pwChar = textBuff[ i + pwdCommandLenght ];
      textPwd[i]=pwChar;
      i++;
    }
    if (i >= minpwdlenght ) {
      textPwd[i]=0;
      
      // Write password to two location for consistence
      // if there are some problem in writing whe see the difference
      for (byte j = 0; j <= i; j++) {
        EEPROM.write(j + pwd1Location, textPwd[j]);
      }
      
      for (byte j = 0; j <= i; j++) {
        EEPROM.write(j + pwd2Location, textPwd[j]);
      }
      
      if ( passwordConsistence() ) {
      client.print(F("The new password is \""));
      client.print(textPwd);
      client.println("\"");
      } else {
       client.println(F("Error in changing password!")); 
      }
    } else {
      client.println(F("Error password too short!"));
    }   
  } else {
    printErrorMessage();
  }
//=========================== end doSetPassword <<

//=========================== doPowerAll: power onn all lines >>
}

void doPowerAll() {
  if ( textBuff[1] == 'l' &&
       textBuff[2] == 'l' &&
       textBuff[3] == '=' && 
       ( ( textBuff[6] == 0x0d && textBuff[5] == 'n' ) || 
       ( textBuff[7] == 0x0d && textBuff[6] == 'f' && textBuff[5] == 'f') )
       ) {
  boolean powerstate;
  switch (textBuff[5]) {
   case 'n' : powerstate = HIGH;   break;
   case 'f' : powerstate = LOW;    break;
   default: printErrorMessage(); return;  
  }
  
  for (byte count=firstpin; count <= lastpin; count++) {
    digitalWrite(count, powerstate);
    EEPROM.write(count, powerstate);
  }
  readPowerStatus();
  } else { 
    printErrorMessage();
  }
}
//=========================== end doPowerAll <<

//=========================== doPowerCommand: change the power state of a line >>
void doPowerCommand() {
  byte pin = checkPin(3);
  if ( pin && 
       textBuff[1] == 'o' &&
       textBuff[2] == 'w' &&
       textBuff[4] == '=' && (
       ( textBuff[7] == 0x0d && textBuff[6] == 'n' ) || 
       ( textBuff[8] == 0x0d && textBuff[7] == 'f' && textBuff[6] == 'f') ) ) {
    pin = (lastpin + 1)  - pin;
    changePowerState(pin);
    //readPowerStatus();
  } else {
    printErrorMessage();
  }
    
}
//=========================== end doPowerCommand <<

//=========================== checkPin: check the command pin number >>
byte checkPin(byte position) {
  byte pin;
  pin = parseDigit(textBuff[position]);
  if ( pin <= (lastpin-firstpin) + 1  && pin >= 1 ) {
    return pin;
  } else {
    //printErrorMessage();
    return 0;
  }
}
//=========================== end checkPin <<

//=========================== changePowerState: change a pin state >>
void changePowerState(byte pin) {
  byte EEwrite;
  byte PINwrite;
  switch (textBuff[6]) {
    case 'n' : EEwrite=1; PINwrite=HIGH;        break;
    case 'f' : EEwrite=0; PINwrite=LOW;         break;
    default: printErrorMessage(); return ;      break;
  }
  EEPROM.write(pin,EEwrite);
  digitalWrite(pin, PINwrite);
  client.print(F("Power line "));
  client.print((lastpin + 1) - pin);
  client.print(F(" is "));
  if ( PINwrite == HIGH ) {
    client.print(F("ON "));
  } else {
    client.print(F("OFF "));
  }
  readPortName(pin);
  client.println("");
}
//=========================== end changePowerState <<

//=========================== parseDigit convert asci characters to int >>
byte parseDigit(char c)
{
  byte digit = -1;
  digit = (byte) c - 0x30; // subtracting 0x30 from ASCII code gives value
  if(digit < 0 || digit > 9) digit = -1;
  return digit;
}
//=========================== end parseDigit <<

//=========================== printErrorMessage: print error message >>
void printErrorMessage()
{
  textBuff[charsReceived-1] = 0;
  client.print(F("Unrecognized command:\""));
  client.print(textBuff);
  client.println(F("\".  ? for help."));
}
//=========================== printErrorMessage <<

//=========================== checkCloseConnection: check the close connection command >>
void checkCloseConnection()
  // if we got here, textBuff[0] = 'c', check the next two
  // characters to make sure the command is valid
{
  if (textBuff[1] == 'x' && textBuff[2] == 'i' && textBuff[3] == 't' && textBuff[4] == 0x0d)
    closeConnection();
  else
    printErrorMessage();
}
//=========================== end checkCloseConnection <<

//=========================== closeConnection: close the tcp/ip connection >>
void closeConnection()
{
  client.println(F("\nBye.\n"));
  client.flush();
  client.stop();
  logged = false;
  connectFlag = false;
}
//=========================== end closeConnection <<

//=========================== tcpSetup: change the tcp/ip parameters >>
void tcpSetup() {
  if ( textBuff[1] == 'c' && 
       textBuff[2] == 'p' &&
       textBuff[3] == 's' &&
       textBuff[4] == 'e' &&
       textBuff[5] == 't' &&
       textBuff[6] == 'u' &&
       textBuff[7] == 'p' &&
       textBuff[8] == '=' &&
       textBuff[9] != 0x0d ) {
     byte octet;
     byte ipTmp[4];
     byte gatewayTmp[4];
     byte subnetTmp[4];


     newpos=tcpSetupCommandLenght;
     
     // Read tcp/ip address
     ipTmp[0] = readOctet(tcpSetupCommandLenght, '.');
     if (newpos == 0) {
       return;
     }
     ipTmp[1] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     ipTmp[2] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     ipTmp[3] = readOctet(newpos, ',');
     if (newpos == 0) {
       return;
     }
     
     // Read subnet mask
     subnetTmp[0] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     subnetTmp[1] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     subnetTmp[2] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     subnetTmp[3] = readOctet(newpos, ',');
     if (newpos == 0) {
       return;
     }
     
     // Read default gateway
     gatewayTmp[0] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     gatewayTmp[1] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     gatewayTmp[2] = readOctet(newpos, '.');
     if (newpos == 0) {
       return;
     }
     gatewayTmp[3] = readOctet(newpos, 0x0d);
     if (newpos == 0) {
       return;
     }
     writeEEPROMOctet(ipEprom, ipTmp);
     writeEEPROMOctet(subnetEprom, subnetTmp);
     writeEEPROMOctet(gatewayEprom, gatewayTmp);
     
     client.println("Tcp Setup!");
     client.println(F("New networks parameters will be applied now!"));
     
     for ( byte i = 0; i <= 3; i++ ) {
       ip[i] = ipTmp[i];
       subnet[i] = subnetTmp[i];
       gateway[i] = gatewayTmp[i];
     }
     
     client.stop();
     startServer();
     
   } else {
     printErrorMessage();
   }
}
//=========================== end tcpSetup <<

//=========================== readOctet: read a tcp/ip octet from a string >>
byte readOctet(byte position, char separator) {
  int value=0;
  byte i = 0;
  char number;
  byte unit=0;
  byte dec=0;
  byte cent=0;
  /*number=textBuff[position];
  while ( number != separator ) { 
    client.print(number);
    i++;
    newpos++;
    number=textBuff[position + i];  
  }*/
  do {
    number=textBuff[position + i];
    
    if ( (number < '0' || number > '9') && number != separator ) {
       newpos = 0;
       printErrorMessage();
       return 0;
    }
    
    if ( number != separator ) {
      if ( i == 0 ) {
        if ( number == 0 ) {
          newpos = 0;
          printErrorMessage();
          return 0;
        }
      unit=number-48;
      } else if ( i == 1 ) {
        dec=unit*10;
        unit=number-48;
      } else if ( i == 2 ) {
        cent=dec*10;
        dec=unit*10;
        unit=number-48;
      }
    }
    
    // client.print( (int) i );
    //client.print('|');
    //client.println( number);
    i++;
    newpos++;
    
  } while (i <= 2 && number != separator);
  
  value = unit + dec + cent;
  //client.println( (int) value);
  //if ( i < 2 || (i == 3 && number == separator ) ) {*/
  
  if ( number == separator && i >= 2 ) {
    //newpos++;
    //client.println( "primo" );
    if (value <= 255) {
      return value;
    } else {
      newpos = 0;
      printErrorMessage();
      return 0;
    }
  } else if ( textBuff[position + i] == separator ) {

    newpos++;
    //client.println( "secondo" );
    if (value <= 255) {
      return value;
    } else {
      newpos = 0;
      printErrorMessage();
      return 0;
    }
  }
  
  newpos = 0;
  printErrorMessage();
  return 0;

}
//=========================== end readOctet <<

//=========================== printHelpMessage: print the eeprom stored help message >>
void printHelpMessage()
{
 
 client.println(F(he_00 he_01 he_02 he_03 he_04 he_05 he_06 he_07 he_08 he_09 he_10 he_11 he_12 he_13 he_14 he_15 he_16 he_17 he_18 he_19 he_20));   
   /*
   int j = 0;
     do {
       textPwd[j] = EEPROM.read(j + pwd1Location);
       j++;
     } while ( textPwd[j-1] != 0);
     
     client.print(textPwd);
     client.println("|");
     
     j = 0;
     do {
       textPwd[j] = EEPROM.read(j + pwd2Location);
       
       j++;
     } while ( textPwd[j-1] != 0);
     
     client.print(textPwd);
     client.println("|");
     
     if ( passwordok ) {
       client.println("password is ok");
     } else {
       client.println("password is bad");
     }
     */
}
//=========================== end printHelpMessage <<

//=========================== ipInfo: print the tcp/ip stored parameters >>
void ipInfo() {
  client.println("");
  client.print(F("        IP = "));
  readIpV4(ipEprom);
  client.println("");
  client.print(F("     SUBNET= "));
  readIpV4(subnetEprom);
  client.println("");
  client.print(F("    GATEWAY= "));
  readIpV4(gatewayEprom);
  client.println("");
  //client.println();
  //client.println( ( digitalRead (0) ) );
}
//=========================== end ipInfo <<
