const int switchPins[4] = {5,4,12,15};

const int relayPins[4] = {16,14,13,2};

void setup()
{
    for(int i=0;i<4;i++)
    {
        pinMode(switchPins[i], INPUT_PULLUP);

        pinMode(relayPins[i], OUTPUT);

        digitalWrite(relayPins[i], HIGH);   // Relay OFF (Active LOW)
    }
}

void loop()
{
    for(int i=0;i<4;i++)
    {
        if(digitalRead(switchPins[i])==LOW)
            digitalWrite(relayPins[i], LOW);
        else
            digitalWrite(relayPins[i], HIGH);
    }
}