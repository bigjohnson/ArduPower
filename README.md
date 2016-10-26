Note: There was a big bug that prevent command interpretation, on 7 june 2016 I released a new version that work fine.
If you had some problem now you can retry.

ArduPower
=========

Arduino UNO + Ethernet Shield (Wiznet) based project telnet driven power srtip

Arduower 
http://www.panu.it/ardupower/

This project use main line power, assure that you take all precautions for preserve human health during project making, and when you use it.
Sized the components so that there is no overheating that can develop fire.
The author assumes no liability for damage to persons, things, animals and aircraft that were damaged by the use of electronic circuitry and computer program described below. 

Released under GPL licence V3 see http://www.gnu.org/licenses/gpl.html and file gpl.txt

Copyryght Alberto Panu 2012

Tested with Arduino 1.6.9

 Quick Start Instructions:
  1. Compile and upload sketch.
  2. Power of the Arduino.
  3. Connect the defParamSwitch pin (search on code the pin number) to ground (0V).
  4. Power on the Arduino.
  5. Wait a minute.
  6. Power off the Arduino.
  7. Disconnect the defParamSwitch from ground.
  8. Power on the Arduino and connect the ethernet cable, make sure that link led is on.
  9. Telnet to the default Ethernet addres 192.168.1.2, you may need some client tcp setup.
 10. On some Telnet clients, hit return to wake up connection.
 11. Enter the default password password.
 12. When connected, type ? \<cr\> for help.
 13. Try a simple command such as 'show'.
 14. If you need a different tcp address change it with command tcpsetup.
 15. Reconnect to ArduPower with new tcp address.
 16. Change the password with command newpw.
 
     If you lose password or ip you could reset to the default password "password" and ip address "192.168.1.2"
     shorting defParamSwitch pin ad power on.
 
 =====================================================================================
 
     WARNNG the first time you load the sketch on a new Arduino board reset to default 
     password and IP shorting defParamSwitch pin to ground!

 =====================================================================================
     
Your comments and suggestion are welcomed to alberto[at]panu.it

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/1.jpg?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/2.jpg?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/3.jpg?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/4.jpg?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/5.jpg?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/s1.png?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/s2.png?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/s3.png?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/s4.png?raw=true)

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/ArduPower/s5.png?raw=true)
