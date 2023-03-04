#include "bsp_uart.h"
#include "gd32e23x.h"
#include "stdio.h"
#include <string.h>
#include <stdarg.h>

/* 串口0引脚初始化 */
static void com_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
}

/* 串口0初始化 */
void uart0_init(void)
{
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* USART GPIO configure */
    com_usart_init();

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
}

void uart0_send(uint8_t ch)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}

static int __serial_putc (const char  c, void *fil)
{
    uart0_send((uint8_t)c);
    return 1;
}

static int __serial_puts (const char *s, void *fil)
{
    const char *ss;
    char        c;

    for (ss = s; (c = *ss) != '\0'; ss++) {
        (void)__serial_putc(c, fil);
    }

    return (ss - s);
}

int kprintf_do (void       *fil,
                int       (*f_putc) (const char  c, void *fil),
                int       (*f_puts) (const char *s, void *fil),
                const char *fmt,
                va_list     args)
{
    unsigned char   c, f, r;
    unsigned long   val;
    char            s[16];
    int             i, w, res, cc;

    for (cc = res = 0; cc >= 0; res += cc >= 0 ? cc : 0) {
        if ((c = *fmt++) == 0) {
            break;                      /* End of string */
        } else if (c != '%') {          /* Non escape cahracter */
            cc = f_putc(c, fil);
            continue;
        }
        w = f = 0;
        c = *fmt++;
        if (c == '0') {                 /* Flag: '0' padding */
            f = 1;
            c = *fmt++;
        }
        while (c >= '0' && c <= '9') {  /* Precision */
            w = w * 10 + (c - '0');
            c = *fmt++;
        }
        if (c == 'l') {                 /* Prefix: Size is long int */
            f |= 2;
            c  = *fmt++;
        }
        if (c == 's') {                 /* Type is string */
            cc = f_puts(va_arg(args, char *), fil);
            continue;
        }
        if (c == 'c') {                 /* Type is character */
            cc = f_putc(va_arg(args, int), fil);
            continue;
        }
        r = 0;
        if (c == 'd') {
            r = 10;             /* Type is signed decimal */
        } else if (c == 'u') {
            r = 10;             /* Type is unsigned decimal */
        } else if (c == 'X' || c == 'x') {
            r = 16;             /* Type is unsigned hexdecimal */
        } else {
            break;              /* Unknown type */
        }
        if (f & 2) {            /* Get the value */
            val = (unsigned long)va_arg(args, long);
        } else {
            val = (c == 'd')
                ? (unsigned long)(long)va_arg(args, int)
                : (unsigned long)va_arg(args, unsigned int);
        }
        /* Put numeral string */
        if ((c == 'd') && (val & 0x80000000)) {
            val = 0 - val;
            f  |= 4;
        }
        i    = sizeof(s) - 1;
        s[i] = 0;
        do {
            c = (unsigned char)(val % r + '0');
            if (c > '9') {
                c += 7;
            }
            s[--i] = c;
            val   /= r;
        } while (i && val);
        if (i && (f & 4)) {
            s[--i] = '-';
        }
        w = sizeof(s) - 1 - w;
        while (i && i > w) {
            s[--i] = (f & 1) ? '0' : ' ';
        }
        cc = f_puts(&s[i], fil);
    }

    return res;
}

int kprintf (const char *fmt, ...)
{
    int     len;
    va_list args;

    va_start(args, fmt);
    len = kprintf_do(NULL, __serial_putc, __serial_puts, fmt, args);
    va_end(args);

    return len;
}

/* end of file */
