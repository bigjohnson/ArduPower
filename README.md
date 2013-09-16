ArduPower
=========

Arduino-based project telnet driven power srtip

Arduower 
http://www.panu.it/ardupower/

Released under GPL licence V3 see http://www.gnu.org/licenses/gpl.html and file gpl.txt

Copyryght Alberto Panu 2012

Copy MyEthernet library to Arduino's library folder, then compile the sketch!

The MyEthernet library is from arduino 1.0.1 code base.


 Quick Start Instructions:
 
  1) Copy the MyEthernet library in the Arduino's library folder.
  2) Compile and upload sketch.
  3) Power of the Arduino.
  4) Connect the defParamSwitch pin (search on code the pin number) to ground (0V).
  5) Power on the Arduino.
  6) Wait a minute.
  7) Power off the Arduino.
  8) Disconnect the defParamSwitch from ground.
  9) Pover on the Arduino and connect the ethernet cable, make sure that link led is on.
 10) Telnet to the default Ethernet address 192.168.1.2, you may need some client tcp setup.
 11) On some Telnet clients, hit return to wake up connection.
 12) Enter the default password password.
 13) When connected, type ? <cr> for help.
 14) Try a simple command such as 'show'.
 15) If you need a different tcp address change it with command tcpsetup.
 16) Reconnect to ArduPower with new tcp address.
 16) Change the password with command newpw.
 
     If you lose password or ip you could reset to the default password "password" and ip address "192.168.1.2"
     shorting defParamSwitch pin ad power on.
 
 =====================================================================================
 
     WARNNG the first time you load the sketch on a new Arduino board reset to default 
     password and IP shorting defParamSwitch pin to ground!

     ------->>>>> Use MyEthernet library for client socket number! <<<<<-------

 =====================================================================================
     
Your comments and suggestion are welcomed to alberto[at]panu.it

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
