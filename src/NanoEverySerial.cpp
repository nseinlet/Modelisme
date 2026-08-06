/*Copyright (C) 2024  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/

#include "NanoEverySerial.h"

// Pre-calculated for 100000 baud at 16MHz clock: (4 * 16,000,000) / 100,000 = 640
#define BAUD_VAL_100K 640

void NanoEverySerial::begin() {
    // 1. ROUTAGE CHIRURGICAL : On bascule UNIQUEMENT l'USART1 sur ses broches alternatives (PC4/PC5)
    // On efface d'abord les bits d'USART1 sans toucher au reste (l'USB utilise USART3 et reste intact).
    PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~PORTMUX_USART1_gm) | PORTMUX_USART1_ALT1_gc;

    // 2. Configuration électrique des broches (PC4 = TX, PC5 = RX) via les ports virtuels
    VPORTC.DIR |= PIN4_bm;  // Force la broche 1 (TX) en sortie
    VPORTC.DIR &= ~PIN5_bm; // Force la broche 0 (RX) en entrée

    // 3. Configuration du débit à 100 000 bauds
    USART1.BAUD = BAUD_VAL_100K; 

    // Configuration pour le SBUS : 8 bits de données, Parité PAIRE, 2 bits de STOP (8E2)
    USART1.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_2BIT_gc;


    // 5. Activation matérielle de l'émetteur et du récepteur de l'USART1
    USART1.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

bool NanoEverySerial::_available() {
    // Return true if the Receive Complete Interrupt Flag is set
    return (USART1.STATUS & USART_RXCIF_bm);
}

uint8_t NanoEverySerial::_read() {
    // Wait until data is completely received (blocking read)
    uint32_t startMs = millis();
    while (! _available()) {
        if (millis() - startMs > 10) { // 10 millisecond timeout
            _parityError = true;
            return 0x04; // or handle timeout as needed
        }
    }
    uint8_t status = USART1.STATUS;
    uint8_t data = USART1.RXDATAL;
    if (status & USART_PERR_bm) { _parityError = true; };
    if (status & USART_FERR_bm) { _parityError = true; };
    return data;
}

bool NanoEverySerial::read() {
    /**
     * Blocking read of HARDWARE_SERIAL_READ_SIZE bytes from the hardware USART3 into the buffer.
     */
    for (int i = 0; i < HARDWARE_SERIAL_READ_SIZE; i++) {
        buffer[i] = 0xff;
    };
    _parityError = false;
    for (int i = 0; i < HARDWARE_SERIAL_READ_SIZE; i++) {
        buffer[i] = _read(); 
    }
    return not _parityError;
}

// Instantiate the global object
NanoEverySerial NanoEverySerial1;