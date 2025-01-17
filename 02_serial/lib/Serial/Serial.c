#include "Serial.h"

/**
 * @brief initialise l'uart3 
 * @param baudrate Le taux de transmission
 */
void serial_setup(uint32_t baudrate) {

    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    UART3_DeInit();
    UART3_Init(baudrate,
            UART3_WORDLENGTH_8D,
            UART3_STOPBITS_1,
            UART3_PARITY_NO,
            UART3_MODE_TXRX_ENABLE);
}

/**
 * @brief Retargets the C library printf function to the UART3.
 * @param c Character to send
 * @retval char Character sent
 */
int putchar(int c) {
    /* Write a character to the UART3 */
    UART3_SendData8(c);
    /* Loop until the end of transmission */
    while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);

    return (c);
}

/**
 * @brief Retargets the C library scanf function to the USART3.
 * @param None
 * @retval char Character to Read
 */
int getchar(void) {
    int c = 0;


    /* Loop until the Read data register flag is SET */
    while ((UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET)); /* wait until data arrived */
    c = UART3_ReceiveData8();
    return (c);
}

/* @Brief  :   Delay function
 * @Param  :   Time to delay (millis seconds)
 * @Return :   None
 * @Note   :   None
 */
void delay_ms(uint32_t nb) {

    for (uint32_t i = 0; i < nb; i++) {
        // simple wait ~1ms @ 16MHz
        for (uint32_t j = 0; j < 1600L; j++)
            __asm__("nop");
    }
}

/**
 * 
 * @param f
 * @return char *
 */
void print_float(float f) {

    int i = (int) (f * 1000.0f);
    printf("%d.%03d\n", i / 1000, i % 1000); // for positive values

}



/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    (void) file;
    (void) line;
    /* Infinite loop */
    while (1) {
    }
}
#endif

/**
 * @brief This function returns a string of maximum length 
 * @param buf to store string; <len> maximum number of characters to read;
 * @param len   must be 2 or greater.
 * @return Pointer to string containing buffer read from UART.
 */
char* gets(char *buf, uint32_t len) {

    unsigned char temp;
    uint32_t i;
    unsigned char done;

    done = FALSE;
    i = 0;

    while (done == FALSE) {
        temp = getchar();

        if (temp == '\b' || temp == 0x7f) {
            if (i > 0) {  // backspace if possible            
                i--;
                putchar(temp);
            }
        } else {
            if (temp == '\r') { // handle newline
                buf[i] = '\0';  // add null terminator to string
                done = TRUE;
            }
            else {
                if (temp == '\0') { // handle EOF            
                    buf[i] = '\0';  // add null terminator to string
                } else {            // handle new character
                    buf[i] = temp;
                    putchar(temp);  // echo character
                    i++;
                    if (i == (len - 1)) {
                        buf[i] = '\0';
                        done = TRUE;
                    }
                }
            }
        }
    }
    return buf;
}

/**
 * Affiche l'adresse du pointeur
 * et la zone mémoire utilisée
 * @param ptr
 */
void printAddress(void *ptr) {

    uint8_t *_ptr = (uint8_t*) ptr;

    if (_ptr >= 0x8000 && _ptr < 0xFFFF) {
        printf("In Flash programm : ");
    }

    if (_ptr <= 0x17ff) {
        printf("In RAM : ");
    }
    printf(" 0x%06X\r\n", _ptr);
}

/**
 * @brief affiche une zone mémoire en hexa et en ASCII
 * @param data un pointeur générique sur le premier octet à afficher
 * @param len la taille en octets de la zone mémoire 
 */
void hex_dump(void *data, int len) {

    uint8_t *p = (uint8_t*) data;
    int n, i, offset;

    offset = 0;
    while (len > 0) {
        n = len < 16 ? len : 16;
        printf("  %06x: ", p + offset);
        for (i = 0; i < n; i++) {
            printf(" %02x", p[i]);
        }
        for (i = n; i < 16; i++) {
            printf("   ");
        }
        printf("  ");
        for (i = 0; i < n; i++) {
            printf("%c", isprint(p[i]) ? p[i] : '.');
        }
        printf("\n\r");
        p += 16;
        offset += 16;
        len -= 16;
    }
}

/**
 * @brief efface l'écran
 */
void effacer() {
    printf("\x1b[H\x1b[2J");
}