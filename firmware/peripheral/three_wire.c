/*
 * Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Read and write data using a half duplex three-wire interface used
 * by the Felica Plug.
 *
 * http://www.sony.net/Products/felica/business/tech-support
 */

#include <avr/io.h>
#include <util/delay.h>

#include "three_wire.h"

/*
 * Configure serial IO
 */
void twspi_init(void)
{
  TWSPI_DDR |= _BV(TWSPI_SEL);
  TWSPI_DDR |= _BV(TWSPI_CLK);
  TWSPI_DDR |= _BV(TWSPI_SW);
}

/*
 * Resets all I/O pins to high impedance.
 */
void twspi_disable(void)
{
  TWSPI_DDR &= ~_BV(TWSPI_SEL) & ~_BV(TWSPI_CLK) & ~_BV(TWSPI_SW);
}

/*
 * Suspend RC-S926 by pulling SW signal low.
 */
void rcs926_suspend(void)
{
  TWSPI_PORT &= ~_BV(TWSPI_SW);
}

/*
 * Activate RC-S926 by pulling SW signal high.
 */
void rcs926_resume(void)
{
  TWSPI_PORT |= _BV(TWSPI_SW);
  _delay_us(50);
}

/*
 * Returns true if the Felica plug received data from the initiator.
 */
bool rcs926_data_ready(void)
{
  return (TWSPI_PIN & _BV(TWSPI_IRQ));
}

/*
 * Returns true if an external RF field is detected (pin LOW).
 */
bool rcs926_rf_present(void)
{
  return (TWSPI_PIN & _BV(TWSPI_RFDET)) == 0;
}

/*
 * Wake up the controller on change on RFDEF (RF Detected) via interrupt
 */
void rcs926_wake_up_on_rf(bool enable)
{
  if (enable) {
    // Enable level change on PB5 (PCINT5) to trigger PCINT0
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT5);
  } else {
    PCMSK0 &= ~_BV(PCINT5);
  }
}

/*
 * Wake up the controlleron change on IRQ (Data ready) via interrupt
 */
void rcs926_wake_up_on_irq(bool enable)
{
  if (enable) {
    // Enable level change on PB4 (PCINT4) to trigger PCINT0
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT4);
  } else {
    PCMSK0 &= ~_BV(PCINT4);
  }
}
/*
 * Set SEL pin to low to indicate data transfer from the host and
 * configures the DATA pin as output.
 */
void twspi_begin_send(void)
{
  TWSPI_PORT &= ~_BV(TWSPI_SEL);
  _delay_us(1);
  TWSPI_DDR |= _BV(TWSPI_DATA);
}

/*
 * Configures the DATA pin as input and sets the SEL pin to high to
 * indicate data transfer to the host.
 */
void twspi_end_send(void)
{
  _delay_us(1);
  TWSPI_DDR &= ~_BV(TWSPI_DATA);
  _delay_us(1);
  TWSPI_PORT |= _BV(TWSPI_SEL);
}

/*
 * Sends a single byte to the bus, MSB first.
 * Max specified bus speed is 1 MHz.
 */
void twspi_send(uint8_t c)
{
  uint8_t i = 8;
  do {
    TWSPI_PORT &= ~_BV(TWSPI_CLK);
    if (c & 0x80) {
      TWSPI_PORT |= _BV(TWSPI_DATA);
    } else {
      TWSPI_PORT &= ~_BV(TWSPI_DATA);
    }
    c <<= 1;
    _delay_us(1);
    TWSPI_PORT |= _BV(TWSPI_CLK);
    _delay_us(1);
  } while (--i);
}

/*
 * Sends a memory buffer to the bus.
 */
void twspi_send_buf(const uint8_t *buf, uint8_t len)
{
  do {
    twspi_send(*buf++);
  } while (--len);
}

void twspi_send_buf_p(const prog_char *buf, uint8_t len)
{
  do {
    twspi_send(pgm_read_byte(buf++));
  } while (--len);
}

/*
 * Receives a byte from the bus. Master controls the clock.
 */
uint8_t twspi_get(void)
{
  uint8_t data = 0;

  for (uint8_t i = 0; i < 8; i++) {
    TWSPI_PORT &= ~_BV(TWSPI_CLK);
    _delay_us(1);
    data <<= 1;
    if (TWSPI_PIN & _BV(TWSPI_DATA)) {
      data |= 1;
    }
    TWSPI_PORT |= _BV(TWSPI_CLK);
    _delay_us(1);
  }
  return data;
}

/*
 * Receives a series of bytes from the bus. No time-out condition.
 */
void twspi_get_buf(uint8_t* buf, uint8_t len)
{
  do {
    *buf++ = twspi_get();
  } while (--len);
}
