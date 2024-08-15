/*
Open DC UPS Monitor (ODUMon) v.1.2
Any Arduino or Arduino compatible board should be able to run this sketch.

Open DC UPS Monitor is an Arduino-based voltage monitor for
unmanaged DC UPS devices powering
unattended CLI Linux computers with DC-DC power supplies.
Dimitar D. Mitov, 2012 - 2014.
This file is in the public domain.

An interfacing script for the computer powered through ODUMon
is also available and recommended.

Based on code from Arduino Playground:
http://arduino.cc/playground/Main/DirectMathVoltmeter

Created and tested with Olimexino-32U4 - an Arduino Leonardo implementation:
https://www.olimex.com/Products/Duino/AVR/OLIMEXINO-32U4/
Do not connect more than 5 Volts directly to your Arduino pins!

LED INDICATIONS:
================
COMPUTER POWER RELAY ON - GREEN LED IS CONTINUOUSLY ON
COMPUTER POWER RELAY OFF - GREEN LED IS OFF
*/

//// SETTINGS START HERE ////

// UPS PIN:
// +V from the UPS is connected through voltage divider to analog pin 0.
const int UPS_Voltage_Pin = A0;

// COMPUTER RELAY PIN:
// Digital pin 7 is used for the computer relay signal.
// Pin 7 is also connected to the built-in green LED of Olimexino-32U4.
// This could be usefull for testing without the external green LED.
// The external 12V encapsulated green LED of ODUMon is 
// connected directly to the computer power relay.
const int Computer_Relay_Pin = 7;

// VOLTAGE REFERENCE:
const float Reference_Voltage = 5.0;

// RESISTOR VALUES:
// Measure your resistors with a multimeter and put here their exact values.
// The variables are of type "float" and they can accept an arbitrary decimal.
// Resistors here are for maximumal input of 15 V on a 5V Arduino board:
const float Resistor_1 = 2000; // 2K resistor
const float Resistor_2 = 1000; // 1K resistor

// VOLTAGES:
const float Maximal_Computer_Power_Supply_Voltage = 14.80;
const float Minimal_Online_Voltage = 13.60;
// Shutdown voltage should be more than
// the minimal voltage of the computer power supply
// for the computer to be properly powered during a normal shutdown.
const float Computer_Shutdown_Voltage = 11.25;
const float Minimal_Computer_Power_Supply_Voltage = 11.00;

// CUTOFF DELAY:
const int Computer_Power_Cutoff_Delay = 180000; // 3 minutes

//// SETTINGS END HERE ////

// INITIALISATIONS OF VARIABLES:
// Last state variable:
char Last_State;

// Voltage divider formula:
const float Denominator = Resistor_2 / (Resistor_1 + Resistor_2);

// Voltage variables:
float Voltage = 0.0;
float Maximal_Battery_Voltage_Under_Load = 0.0;
float First_Shutdown_Voltage_Measurement = 0.0;
float Second_Shutdown_Voltage_Measurement = 0.0;

// One percent of usable battery capacity (based on voltage measurement):
float One_Percent_of_Usable_Capacity = 0.0;

void setup() {
    // Voltage Reference:
    analogReference(DEFAULT);

    // Computer relay pin as output:
    pinMode(Computer_Relay_Pin, OUTPUT);

    // UPS voltage pin as input:
    pinMode(UPS_Voltage_Pin, INPUT);

    // Begin serial communication:
    Serial.begin(9600);
}

void loop() {
    voltage_measurement:
    // VOLTAGE MEASUREMENT:
    // Read the raw data from the voltage divider:
    float Analog_Read_Value = analogRead(UPS_Voltage_Pin);

    // Calculate the input voltage:
    float Input_Voltage = (Analog_Read_Value / 1024) * Reference_Voltage;

    // Calculate the real voltage before the voltage divider:
    Voltage = Input_Voltage / Denominator;

    // OVERVOLTAGE:
    if (Voltage > Maximal_Computer_Power_Supply_Voltage) {
        // Computer relay OFF:
        digitalWrite(Computer_Relay_Pin, LOW);
        delay(150);

        // Skip the rest, go to end and start again.
        goto bailout;
    }

    // UNDERVOLTAGE:
    if (Voltage < Minimal_Computer_Power_Supply_Voltage) {
        // Computer relay OFF:
        digitalWrite(Computer_Relay_Pin, LOW);
        delay(150);

        // Skip the rest, go to end and start again.
        goto bailout;
    }

    // ON LINE VOLTAGE:
    if (Voltage > Minimal_Online_Voltage &&
    Voltage < Maximal_Computer_Power_Supply_Voltage) {
        // Computer relay ON:
        digitalWrite(Computer_Relay_Pin, HIGH);
        delay(150);

        // Serial console message formatted for
        // the interfacing script on the ODUMon-powered computer.
        Serial.print("ODUMon|OL|");
        Serial.print(Voltage);
        Serial.println("|END");

        char Last_State = 'ONLINE';

        // 1 sec. delay - i.e. read the UPS voltage every second:
        delay(1000);
    }

    // ON BATTERY VOLTAGE - FIRST MEASUREMENT:
    if (Voltage < Minimal_Online_Voltage && 
    Voltage > Computer_Shutdown_Voltage && Last_State == 'ONLINE') {
        Maximal_Battery_Voltage_Under_Load = Voltage;

        // UPS Battery Usable Charge (based on voltage measurement):
        float One_Percent_of_Usable_Capacity = 
        (Maximal_Battery_Voltage_Under_Load - Computer_Shutdown_Voltage) / 100;

        // Computer relay ON:
        digitalWrite(Computer_Relay_Pin, HIGH);
        delay(150);

        // Serial console message formatted for
        // the interfacing script on the ODUMon-powered computer.
        Serial.print("ODUMon|OB|");
        Serial.print(Voltage);
        Serial.println("|100|END");

        char Last_State = 'BATTERY';
        // 1 sec. delay - i.e. read the UPS voltage every second:
        delay(1000);
    }

    // ON BATTERY VOLTAGE -
    // SECOND AND EVERY OTHER MEASUREMENT UNTIL
    // COMPUTER SHUTDOWN VOLTAGE IS REACHED:
    if (Voltage < Minimal_Online_Voltage &&
    Voltage > Computer_Shutdown_Voltage && Last_State == 'BATTERY') {
        // Computer relay ON:
        digitalWrite(Computer_Relay_Pin, HIGH);
        delay(150);

        float Remaining_Usable_Battery_Capacity = 
        (Voltage - Computer_Shutdown_Voltage) / One_Percent_of_Usable_Capacity;

        // Serial console message formatted for
        // the interfacing script on the ODUMon-powered computer.
        Serial.print("ODUMon|OB|");
        Serial.print(Voltage);
        Serial.print("|");
        Serial.print(Remaining_Usable_Battery_Capacity);
        Serial.println("|END");

        char Last_State = 'BATTERY';

        // 1 sec. delay - i.e. read the UPS voltage every second:
        delay(1000);
    }

    // COMPUTER SHUTDOWN VOLTAGE BEFORE SHUTDOWN COMMAND -
    // FIRST MEASUREMENT:
    if (Voltage <= Computer_Shutdown_Voltage && Last_State == 'BATTERY') {
        float First_Shutdown_Voltage_Measurement = Voltage;
        // Half a second delay before the next measurement:
        delay(500);
        goto voltage_measurement;
    }

    // COMPUTER SHUTDOWN VOLTAGE BEFORE SHUTDOWN COMMAND -
    // SECOND MEASUREMENT:
    if (Voltage <= Computer_Shutdown_Voltage && 
    First_Shutdown_Voltage_Measurement <= Computer_Shutdown_Voltage && 
    Last_State == 'BATTERY') {
        float Second_Shutdown_Voltage_Measurement = Voltage;
        // Half a second delay before the next measurement:
        delay(500);
        goto voltage_measurement;
    }

    // COMPUTER SHUTDOWN VOLTAGE -
    // THIRD AND FINAL MEASUREMENT AND SHUTDOWN COMMAND:
    if (Voltage <= Computer_Shutdown_Voltage && 
        First_Shutdown_Voltage_Measurement <= Computer_Shutdown_Voltage && 
        Second_Shutdown_Voltage_Measurement <= Computer_Shutdown_Voltage && 
        Last_State == 'BATTERY') {
        // Computer relay ON:
        digitalWrite(Computer_Relay_Pin, HIGH);
        delay(150);

        // Serial console message formatted for
        // the interfacing script on the ODUMon-powered computer.
        Serial.println("ODUMon|CR|END");

        // Wait a predefined ammount of time before cutting off the DC power.
        // This will allow the computer to shutdown gracefully.
        delay(Computer_Power_Cutoff_Delay);
        digitalWrite(Computer_Relay_Pin, LOW);
        delay(150);

        char Last_State = 'SHUTDOWN';
        float First_Shutdown_Voltage_Measurement = 0.0;
        float Second_Shutdown_Voltage_Measurement = 0.0;

        // 1 sec. delay - i.e. read the UPS voltage every second:
        delay(1000);
        goto voltage_measurement;
    }

    // COMPUTER SHUTDOWN VOLTAGE - AFTER SHUTDOWN:
    if (Voltage <= Computer_Shutdown_Voltage && Last_State == 'SHUTDOWN'){
        // Computer relay OFF:
        digitalWrite(Computer_Relay_Pin, LOW);
        delay(150);

        // 1 sec. delay - i.e. read the UPS voltage every second:
        delay(1000);
        goto voltage_measurement;
    }

    bailout:
    // There must be something after a labeled point in the program.
    delay(0);
}
// EOF
