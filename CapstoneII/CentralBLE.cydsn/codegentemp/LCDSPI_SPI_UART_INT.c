/***************************************************************************//**
* \file LCDSPI_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCDSPI_PVT.h"
#include "LCDSPI_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (LCDSPI_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: LCDSPI_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(LCDSPI_SPI_UART_ISR)
{
#if (LCDSPI_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (LCDSPI_INTERNAL_RX_SW_BUFFER_CONST) */

#if (LCDSPI_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (LCDSPI_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef LCDSPI_SPI_UART_ISR_ENTRY_CALLBACK
    LCDSPI_SPI_UART_ISR_EntryCallback();
#endif /* LCDSPI_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != LCDSPI_customIntrHandler)
    {
        LCDSPI_customIntrHandler();
    }

    #if(LCDSPI_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        LCDSPI_ClearSpiExtClkInterruptSource(LCDSPI_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (LCDSPI_CHECK_RX_SW_BUFFER)
    {
        if (LCDSPI_CHECK_INTR_RX_MASKED(LCDSPI_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (LCDSPI_rxBufferHead + 1u);

                /* Adjust local head index */
                if (LCDSPI_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == LCDSPI_rxBufferTail)
                {
                    #if (LCDSPI_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        LCDSPI_INTR_RX_MASK_REG &= ~LCDSPI_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) LCDSPI_RX_FIFO_RD_REG;
                        LCDSPI_rxBufferOverflow = (uint8) LCDSPI_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    LCDSPI_PutWordInRxBuffer(locHead, LCDSPI_RX_FIFO_RD_REG);

                    /* Move head index */
                    LCDSPI_rxBufferHead = locHead;
                }
            }
            while(0u != LCDSPI_GET_RX_FIFO_ENTRIES);

            LCDSPI_ClearRxInterruptSource(LCDSPI_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (LCDSPI_CHECK_TX_SW_BUFFER)
    {
        if (LCDSPI_CHECK_INTR_TX_MASKED(LCDSPI_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (LCDSPI_txBufferHead != LCDSPI_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (LCDSPI_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (LCDSPI_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    LCDSPI_TX_FIFO_WR_REG = LCDSPI_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    LCDSPI_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    LCDSPI_DISABLE_INTR_TX(LCDSPI_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (LCDSPI_SPI_UART_FIFO_SIZE != LCDSPI_GET_TX_FIFO_ENTRIES);

            LCDSPI_ClearTxInterruptSource(LCDSPI_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef LCDSPI_SPI_UART_ISR_EXIT_CALLBACK
    LCDSPI_SPI_UART_ISR_ExitCallback();
#endif /* LCDSPI_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (LCDSPI_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */
