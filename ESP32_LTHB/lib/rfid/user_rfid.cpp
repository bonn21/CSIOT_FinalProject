#include <user_rfid.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 5
#define RST_PIN 4

byte nuidPICC[4];
const int ID_DEC[4] = {131, 174, 143, 21}; // mã
const int ID_HEX[4] = {0x83, 0xAE, 0x8F, 0x15};

MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

int flag_open_door = 0;

void RFID_Init()
{
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
}

void RFID_Check()
{
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
    if (!rfid.PICC_IsNewCardPresent())
        return;
    if (!rfid.PICC_ReadCardSerial())
        return;
    for (byte i = 0; i < 4; i++)
    {
        nuidPICC[i] = rfid.uid.uidByte[i];
    }
    if (nuidPICC[0] == ID_HEX[0] && nuidPICC[1] == ID_HEX[1] && nuidPICC[2] == ID_HEX[2] && nuidPICC[3] == ID_HEX[3])
    {
        flag_open_door = 1;
        Serial.println("Open the door.");
    }
    else
    {
        // count_wrong++;
        flag_open_door = 0;
        Serial.println("Faild!");
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}