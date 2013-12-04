
#define KA_REGIF_BASE            0xA0000000			//old:REGIF_BASE

#define SYSTEM_BASE              KA_REGIF_BASE
#define SYSTEM_CTRL_REG          SYSTEM_BASE
#define CLK_DIV_REG              SYSTEM_BASE + 0x04

#define SDRAM_BASE               0x00000000 + 0x00200000

#define KA_SCU_BASE              KA_REGIF_BASE                   /* System Control Unit */
#define KA_SSI_BASE              KA_REGIF_BASE + 0x1000
#define KA_PWM_BASE              KA_REGIF_BASE + 0x2000
#define KA_WDT_BASE              KA_REGIF_BASE + 0x3000
#define KA_UART_BASE             KA_REGIF_BASE + 0x4000
#define KA_GPIO0_BASE            KA_REGIF_BASE + 0x5000
#define KA_INTC_BASE             KA_REGIF_BASE + 0x6000
#define KA_DMA_BASE              KA_REGIF_BASE + 0x7000
#define KA_SDRAM_CTRL_BASE       KA_REGIF_BASE + 0x8000
#define KA_SDIO_BASE             KA_REGIF_BASE + 0x9000
#define KA_SD_SWITCH_BASE        KA_REGIF_BASE + 0xa000
#define KA_SD_CTRL_BASE          KA_REGIF_BASE + 0xb000
#define KA_GPIO1_BASE            KA_REGIF_BASE + 0xc000

/* scu */
#define SCU_CLK_SRC_CTL		 KA_SCU_BASE + 0x00
#define SCU_PLL_FREQ_SEL1	 KA_SCU_BASE + 0x04
#define SCU_PLL_FREQ_SEL2	 KA_SCU_BASE + 0x08
#define SCU_SYSTEM_CTL1		 KA_SCU_BASE + 0x0c
#define SCU_SYSTEM_CTL2		 KA_SCU_BASE + 0x10
#define SCU_SYSTEM_CTL3		 KA_SCU_BASE + 0x14

/* ssi */
#define SSI_BASE                 KA_SSI_BASE
#define SSI_PRE                  SSI_BASE + 0x00
#define SSI_CON                  SSI_BASE + 0x04
#define SSI_STA                  SSI_BASE + 0x08
#define SSI_TDAT                 SSI_BASE + 0x0c
#define SSI_RDAT                 SSI_BASE + 0x10

/* sdio */
#define SDIO_BASE                KA_SDIO_BASE
#define SDIO_CARD_BLOCK_SET_REG  SDIO_BASE  + 0x00
#define SDIO_CTRL_REG            SDIO_BASE  + 0x04
#define SDIO_CMD_ARGUMENT_REG    SDIO_BASE  + 0x08
#define SDIO_SPECIAL_COMMAND_REG SDIO_BASE  + 0x0c
#define SDIO_STATUS_REG          SDIO_BASE  + 0x10
#define SDIO_ERROR_ENABLE_REG    SDIO_BASE  + 0x14
#define SDIO_RESPONSE1_REG       SDIO_BASE  + 0x18
#define SDIO_RESPONSE2_REG       SDIO_BASE  + 0x1c
#define SDIO_RESPONSE3_REG       SDIO_BASE  + 0x20
#define SDIO_RESPONSE4_REG       SDIO_BASE  + 0x24
#define SDIO_BUF_TRAN_RESP_REG   SDIO_BASE  + 0x28
#define SDIO_BUF_TRAN_CTRL_REG   SDIO_BASE  + 0x2c
#define SDIO_DMA_SACH0_REG       SDIO_BASE  + 0x30
#define SDIO_DMA_TCCH0_REG       SDIO_BASE  + 0x34
#define SDIO_DMA_CTRCH0_REG      SDIO_BASE  + 0x38
#define SDIO_reserved            SDIO_BASE  + 0x3c
#define SDIO_DMA_DACH1_REG       SDIO_BASE  + 0x40
#define SDIO_DMA_TCCH1_REG       SDIO_BASE  + 0x44
#define SDIO_DMA_CTRCH1_REG      SDIO_BASE  + 0x48
#define SDIO_DMA_INTS_REG        SDIO_BASE  + 0x4c
#define SDIO_DMA_FIFO_STATUS_REG SDIO_BASE  + 0x50

/* pwm */
#define PWM_BASE                 KA_PWM_BASE
#define TCFG0                    PWM_BASE + 0x00
#define TCFG1                    PWM_BASE + 0x04
#define TCON0                    PWM_BASE + 0x08
#define TCON1                    PWM_BASE + 0x0c
#define TCNTB0                   PWM_BASE + 0x10
#define TCMPB0                   PWM_BASE + 0x14
#define TCNTO0                   PWM_BASE + 0x18
#define TCNTB1                   PWM_BASE + 0x40
#define TCNTO1                   PWM_BASE + 0x44
#define TCNTB2                   PWM_BASE + 0x48
#define TCNTO2                   PWM_BASE + 0x4c

/* wdt */
#define WDT_BASE                 KA_WDT_BASE
#define WDT_CON                  WDT_BASE + 0x00
#define WDT_DAT                  WDT_BASE + 0x04
#define WDT_CNT                  WDT_BASE + 0x08
#define WDT_CON_CONTER_EN        0x20

/* gpio0 */
#define GPIO0_BASE               KA_GPIO0_BASE		//old:GPIO_BASE
#define GPIO_OEN                 GPIO0_BASE + 0x00
#define GPIO_INPUT               GPIO0_BASE + 0x04
#define GPIO_OUTPUT              GPIO0_BASE + 0x08
#define GPIO_INT                 GPIO0_BASE + 0x0c
#define GPIO_INT_CLR0            GPIO0_BASE + 0x10
#define GPIO_INT_CLR1            GPIO0_BASE + 0x14
#define GPIO_INT_CLR2            GPIO0_BASE + 0x18
#define GPIO_INT_CLR3            GPIO0_BASE + 0x1c
#define GPIO_INT_CLR4            GPIO0_BASE + 0x20
#define GPIO_INT_CLR5            GPIO0_BASE + 0x24
#define GPIO_INT_CLR6            GPIO0_BASE + 0x28
#define GPIO_INT_CLR7            GPIO0_BASE + 0x2c

/* gpio1 */
#define GPIO1_BASE               KA_GPIO1_BASE
#define GPO_OEN                  GPIO1_BASE + 0x00
#define GPI_INPUT                GPIO1_BASE + 0x04
#define GPO_OUTPUT               GPIO1_BASE + 0x08
#define GPI_INT                  GPIO1_BASE + 0x0c
#define GPI_INT_CLR0             GPIO1_BASE + 0x10
#define GPI_INT_CLR1             GPIO1_BASE + 0x14
#define GPI_INT_CLR2             GPIO1_BASE + 0x18
#define GPI_INT_CLR3             GPIO1_BASE + 0x1c
#define GPI_INT_CLR4             GPIO1_BASE + 0x20
#define GPI_INT_CLR5             GPIO1_BASE + 0x24
#define GPI_INT_CLR6             GPIO1_BASE + 0x28
#define GPI_INT_CLR7             GPIO1_BASE + 0x2c

// sdram base
#define SDRAM_CTRL_BASE          KA_SDRAM_CTRL_BASE

/* uart */
#define UART_BASE                KA_UART_BASE
#define UART_RECV                UART_BASE + 0x00
#define UART_THR                 UART_BASE + 0x00
#define UART_INTR                UART_BASE + 0x04
#define UART_FCR                 UART_BASE + 0x08
#define UART_IIR                 UART_BASE + 0x08
#define UART_LCR                 UART_BASE + 0x0c
#define UART_MCR                 UART_BASE + 0x10
#define UART_LINE                UART_BASE + 0x14
#define UART_MSR                 UART_BASE + 0x18
#define UART_RFST                UART_BASE + 0x1c

/*sd card */
#define SDR_BASE                  KA_SD_CTRL_BASE
#define SDR_Card_BLOCK_SET_REG    SDR_BASE + 0x00
#define SDR_CTRL_REG              SDR_BASE + 0x04
#define SDR_CMD_ARGUMENT_REG      SDR_BASE + 0x08
#define SDR_SPECIAL_CTRL_REG      SDR_BASE + 0x0C	//old:SDR_ADDRESS_REG
#define SDR_STATUS_REG            SDR_BASE + 0x10
#define SDR_Error_Enable_REG      SDR_BASE + 0x14
#define SDR_RESPONSE1_REG         SDR_BASE + 0x18
#define SDR_RESPONSE2_REG         SDR_BASE + 0x1C
#define SDR_RESPONSE3_REG         SDR_BASE + 0x20
#define SDR_RESPONSE4_REG         SDR_BASE + 0x24
#define SDR_DMA_TRAN_RESP_REG     SDR_BASE + 0x28
#define SDR_BUF_TRAN_CTRL_REG     SDR_BASE + 0x2C

#define SDR_DMA_SACH0_REG         SDR_BASE + 0x30
#define SDR_DMA_TCCH0_REG         SDR_BASE + 0x34
#define SDR_DMA_CTRCH0_REG        SDR_BASE + 0x38
#define SDR_DMA_DACH1_REG         SDR_BASE + 0x40
#define SDR_DMA_TCCH1_REG         SDR_BASE + 0x44
#define SDR_DMA_CTRCH1_REG        SDR_BASE + 0x48
#define SDR_DMA_INTS_REG          SDR_BASE + 0x4C
#define SDR_DMA_FIFO_STATUS_REG   SDR_BASE + 0x50

/* interrupt controller */
#define INTC_BASE                 KA_INTC_BASE
#define INTC_INTSRC1_ADDR         INTC_BASE + 0x00
#define INTC_INTSRC2_ADDR         INTC_BASE + 0x04
#define INTC_INTMOD1_ADDR         INTC_BASE + 0x08
#define INTC_INTMOD2_ADDR         INTC_BASE + 0x0c
#define INTC_INTMSK1_ADDR         INTC_BASE + 0x10
#define INTC_INTMSK2_ADDR         INTC_BASE + 0x14
#define INTC_INTPND1_ADDR         INTC_BASE + 0x18
#define INTC_INTPND2_ADDR         INTC_BASE + 0x1c
#define INTC_INTPRT1_ADDR         INTC_BASE + 0x20
#define INTC_INTPRT2_ADDR         INTC_BASE + 0x24
#define INTC_INTPRO1_ADDR         INTC_BASE + 0x28
#define INTC_INTPRO2_ADDR         INTC_BASE + 0x2c
#define INTC_INTCLR_ADDR          INTC_BASE + 0x30
#define INTC_INTOFS_ADDR          INTC_BASE + 0x34

