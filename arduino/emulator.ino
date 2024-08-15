/*
Emulator for the Open DC UPS Monitor (ODUMon) v.1.1.
Used to develop and test computer hosted interfacing script.

Open DC UPS Monitor is an Arduino-based voltage monitor for
unmanaged DC UPS devices powering
unattended CLI Linux computers with DC-DC power supplies.
Dimitar D. Mitov, 2012 - 2014.
This file is in the public domain.
*/

void setup(){
    Serial.begin(115200); // Begin serial communication.
}

void loop(){
    // On Line:
    Serial.println("ODUMon|OL|12.98|END");
    delay (650);

    // On Line:
    Serial.println("ODUMon|OL|12.98|END");
    delay (650);

    // On Line:
    Serial.println("ODUMon|OL|12.98|END");
    delay (650);

    // On Battery:
    Serial.println("ODUMon|OB|12.10|65.20|END");
    delay (550);

    // On Battery:
    Serial.println("ODUMon|OB|12.10|65.20|END");
    delay (550);

    // On Line:
    Serial.println("ODUMon|OL|12.98|END");
    delay (650);

    // On Battery:
    Serial.println("ODUMon|OB|12.10|65.20|END");
    delay (550);

    // On Battery:
    Serial.println("ODUMon|OB|12.10|65.20|END");
    delay (550);

    // Critical:
    Serial.println("ODUMon|CR|END");
    delay (600);
}
