/*
 * Copyright (C) 2019 Marco Lochen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define SCK_PIN        30
#define SDIO_PIN       31

// all the addresses that the sensor supports
#define CONFIGURATION  0x40
#define STATUS         0x41
#define DELTA_Y        0x42
#define DELTA_X        0x43
#define SQUAL          0x44
#define MAXIMUM_PIXEL  0x45
#define MINIMUM_PIXEL  0x46
#define PIXEL_SUM      0x47
#define PIXEL_DATA     0x48
#define SHUTTER_UPPER  0x49
#define SHUTTER_LOWER  0x4A
#define FRAME_PERIOD   0x4B

uint8_t pixel_buffer[324];
uint16_t i = 0;

void setup() {
  Serial.begin(38400);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(SDIO_PIN, INPUT_PULLUP);
  digitalWrite(SCK_PIN, HIGH);
  delay(100);
  write_data(CONFIGURATION, 0x01);
  delay(100);
}

void loop() {
  uint8_t data = read_data(PIXEL_DATA);
  if (data & 0x80)
  {
    /* beginning of new frame */
    i = 0;
  }
  if (data & 0x40)
  {
    /* data is valid */
    pixel_buffer[i] = data & 0x3F;
    i++;
  }
  if (i == 324)
  {
    send_data();
    i = 0;
  }
  delayMicroseconds(1);
}

void send_hex(uint8_t data)
{
  uint8_t temp;
  uint8_t offset;
  temp = data >> 4;
  offset = (temp < 10) ? '0' : 'A' - 10;
  Serial.write(temp + offset);
  temp = data & 0x0F;
  offset = (temp < 10) ? '0' : 'A' - 10;
  Serial.write(temp + offset);
}

void send_data(void)
{
  for (uint16_t i = 0; i < 324; i++)
  {
    send_hex(pixel_buffer[i]);
  }
  Serial.print("\n");
  Serial.flush();
}

uint8_t read_data(uint8_t address)
{
  pinMode(SDIO_PIN, OUTPUT);
  write_byte(address);
  pinMode(SDIO_PIN, INPUT_PULLUP);
  delayMicroseconds(110);
  return read_byte();
}

void write_data(uint8_t address, uint8_t data)
{
  pinMode(SDIO_PIN, OUTPUT);
  write_byte(address | 0x80);
  write_byte(data);
  pinMode(SDIO_PIN, INPUT_PULLUP);
}

uint8_t read_byte(void)
{
  uint8_t data = 0;
  for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
  {
    digitalWrite(SCK_PIN, LOW);
    delayMicroseconds(1);
    digitalWrite(SCK_PIN, HIGH);
    if (digitalRead(SDIO_PIN) == HIGH)
    {
      data |= mask;
    }
    delayMicroseconds(1);
  }
  return data;
}

void write_byte(uint8_t data)
{
  for(uint8_t mask = 0x80; mask != 0; mask >>= 1)
  {
    digitalWrite(SCK_PIN, LOW);
    digitalWrite(SDIO_PIN, (data & mask) ? HIGH : LOW);
    delayMicroseconds(1);
    digitalWrite(SCK_PIN, HIGH);
    delayMicroseconds(1);
  }
}
