/*HEADER**********************************************************************
*
* Copyright 2009-2013 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains implementation of "INT/UART output" "eDMA/UART input"  driver provided to other driver 
*
**END************************************************************************/

#include "mqx_inc.h"
#include "mqx.h"
#include "bsp.h"
#include "io_prv.h"
#include "charq.h"
#include "fio_prv.h"
#include "serinprv.h"

/* DMA driver functions */

extern void _kuart_mix_write(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char );
extern uint32_t _kuart_mix_init(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char *);
extern uint32_t _kuart_mix_deinit(KUART_INIT_STRUCT_PTR, KUART_INFO_STRUCT_PTR);
extern uint32_t _kuart_mix_enable(KUART_INFO_STRUCT_PTR);
extern void    _kuart_mix_err_isr(void *);
extern void    _kuart_dma_rx_isr(void *, int tcd_done, uint32_t tcd_seq);
/* Polled functions used */
extern uint32_t _kuart_polled_init(KUART_INIT_STRUCT_PTR, void ** , char *);
extern uint32_t _kuart_polled_deinit(KUART_INIT_STRUCT_PTR, KUART_INFO_STRUCT_PTR);
extern uint32_t _kuart_polled_ioctl(KUART_INFO_STRUCT_PTR, uint32_t, uint32_t *);
static void _kuart_prepare_rx_dma(IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr);
/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_int_putc
* Returned Value   : none
* Comments         :
*   This function is called to write out the first character, when
* the output serial device and output ring buffers are empty.
*
*END*********************************************************************/

static void _kuart_mix_int_putc
   (
      /* [IN] the address of the device specific information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to write out now */
      char                       c
   )
{ /* Body */
   KUART_INFO_STRUCT_PTR                  sci_info_ptr;
   UART_MemMapPtr                         sci_ptr;

   sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   sci_ptr = sci_info_ptr->SCI_PTR;

   while (!(sci_ptr->S1 & UART_S1_TDRE_MASK)) {
      /* Wait while buffer is full */
   } /* Endwhile */

   sci_ptr->D = c;
   sci_info_ptr->TX_CHARS++;

   /* Enable transmit iterrupt */
   sci_ptr->C2 |= UART_C2_TIE_MASK;

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_nextc
* Returned Value   : _mqx_int, the next character to write out.
* Comments         :
*    This function returns the next character to send out, or -1 if
* no more output characters are available
*
*END*********************************************************************/

static _mqx_int _io_serial_mix_nextc
   (
      /* [IN] the interrupt I/O context information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr
   )
{ /* Body */
   unsigned char c;

   if (int_io_dev_ptr->HAVE_STOPPED_OUTPUT
      || (! int_io_dev_ptr->OUTPUT_ENABLED))
   {
      return(-1);
   } /* Endif */

   if (_CHARQ_EMPTY(int_io_dev_ptr->OUT_QUEUE)) {
      /* No output */
      int_io_dev_ptr->OUTPUT_ENABLED = FALSE;
      if (_QUEUE_GET_SIZE(int_io_dev_ptr->OUT_WAITING_TASKS)) {
         _taskq_resume(int_io_dev_ptr->OUT_WAITING_TASKS, TRUE);
      } /* Endif */
      return(-1);
   }/* Endif */

   _CHARQ_DEQUEUE(int_io_dev_ptr->OUT_QUEUE, c);
   return((_mqx_int)c);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_int_tx_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial input interrupts.
*
*************************************************************************/

static void _kuart_mix_int_tx_isr
   (
      /* [IN] the address of the device specific information */
      void   *parameter
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
   KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   UART_MemMapPtr                         sci_ptr = sci_info_ptr->SCI_PTR;
   volatile int32_t                        c;
   uint16_t                                stat = sci_ptr->S1;

   ++sci_info_ptr->INTERRUPTS;

   /* 
   process error flags in case:
   - there is single interrupt line for both data and errors
   - _kuart_int_err_isr has lower priority than _kuart_int_rx_tx_isr
   - error interrupts are not enabled
   */
   if(stat & UART_S1_OR_MASK) {
      ++sci_info_ptr->RX_OVERRUNS;
   }
   if(stat & UART_S1_PF_MASK) {
      ++sci_info_ptr->RX_PARITY_ERRORS;
   }
   if(stat & UART_S1_NF_MASK) {
      ++sci_info_ptr->RX_NOISE_ERRORS;
   }
   if(stat & UART_S1_FE_MASK) {
      ++sci_info_ptr->RX_FRAMING_ERRORS;
   }

   /*
   if "framming error" or "overrun" error occours 
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
   if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   {
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      sci_ptr->D;
      // if 'RFIFO' underflow detected, perform flush to reinitialize 'RFIFO'
      if (sci_ptr->SFIFO & UART_SFIFO_RXUF_MASK)
      {
         sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
         sci_ptr->SFIFO |= UART_SFIFO_RXUF_MASK;
      }
      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
      return;
   }

   /* try if TX buffer is still not full */
   if (stat & UART_S1_TDRE_MASK) {
      c = _io_serial_mix_nextc(int_io_dev_ptr);
      if (c >= 0) {
          sci_ptr->D = c;
      } else {
         /* All data sent, disable transmit interrupt */
         sci_ptr->C2 &= ~UART_C2_TIE_MASK;
      }
      sci_info_ptr->TX_CHARS++;
   }

}  /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_mix_peripheral_enable
* Returned Value   : None
* Comments         :
*    Enables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_mix_peripheral_enable
	 (
	     /* [IN] SCI channel */
	     UART_MemMapPtr sci_ptr
	 )
{
	 /* Enable only receive interrupt, transmit will be enabled during sending first character */
	 //TODO: why we need to enable the TX/RX in such an early step
	 sci_ptr->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK;
	 /* Enable dma RX request */
	 sci_ptr->C5 |=  UART_C5_RDMAS_MASK;
	 /* Enable INT TX request */
	 sci_ptr->C5 &= ~UART_C5_TDMAS_MASK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_mix_peripheral_disable
* Returned Value   : None
* Comments         :
*    Disables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_mix_peripheral_disable
	 (
	     /* [IN] SCI channel */
	     UART_MemMapPtr sci_ptr
	 )
{
	 /* Transmitter and receiver disable */
	 sci_ptr->C2 &= (~ (UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK));
	 //sci_ptr->C5 &= (~(UART_C5_TDMAS_MASK | UART_C5_RDMAS_MASK));
	 
}

/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_open
* Returned Value   : _mqx_int task error code
* Comments         :
*    This routine initializes an interrupt/dma I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    This memory is returned as a 'handle' to be used for all other 
*    interrupt I/O functions.
*
* 
*END**********************************************************************/

static _mqx_int _io_serial_mix_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char              *open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff
      */
      char        *flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       result = MQX_OK;
   _mqx_uint                       ioctl_val;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   
   if (int_io_dev_ptr->COUNT) {
      /* Device is already opened */
      int_io_dev_ptr->COUNT++;
      fd_ptr->FLAGS = int_io_dev_ptr->FLAGS;
      return(result);
   } /* Endif */

   int_io_dev_ptr->IN_WAITING_TASKS  = _taskq_create(MQX_TASK_QUEUE_FIFO);
   int_io_dev_ptr->OUT_WAITING_TASKS = _taskq_create(MQX_TASK_QUEUE_FIFO);

   int_io_dev_ptr->OUT_QUEUE = (void *)_mem_alloc_system(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + int_io_dev_ptr->OQUEUE_SIZE);
   _mem_set_type(int_io_dev_ptr->OUT_QUEUE,MEM_TYPE_IO_SERIAL_OUT_QUEUE);      
   _CHARQ_INIT(int_io_dev_ptr->OUT_QUEUE, int_io_dev_ptr->OQUEUE_SIZE);



   int_io_dev_ptr->FLAGS = (_mqx_uint)flags;
   fd_ptr->FLAGS      = (_mqx_uint)flags;

   result = (*int_io_dev_ptr->DEV_INIT)(int_io_dev_ptr, open_name_ptr);
   
   if (result == MQX_OK) {
       result = (*int_io_dev_ptr->DEV_ENABLE_INTS)(int_io_dev_ptr->DEV_INFO_PTR);
   } /* Endif */
   
   if (result != MQX_OK) {
      _mem_free(int_io_dev_ptr->OUT_QUEUE);
      _taskq_destroy(int_io_dev_ptr->IN_WAITING_TASKS);
      _taskq_destroy(int_io_dev_ptr->OUT_WAITING_TASKS);
   }
   int_io_dev_ptr->COUNT = 1;
   return(result);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_close
* Returned Value   : _mqx_int error code
* Comments         :
*    This routine closes the serial I/O channel.
* 
*END**********************************************************************/

static _mqx_int _io_serial_mix_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_int                        result = MQX_OK;
   _mqx_int                        ioctl_val;

   /* other task cannot break 'close' function */
   _int_disable();

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   /* flush the output buffer before closing */
   (*io_dev_ptr->IO_IOCTL)(fd_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);

   if (--int_io_dev_ptr->COUNT == 0) {

      if (int_io_dev_ptr->DEV_DEINIT) {
          result = (*int_io_dev_ptr->DEV_DEINIT)(int_io_dev_ptr->DEV_INIT_DATA_PTR,
                  int_io_dev_ptr->DEV_INFO_PTR);
      } /* Endif */
      _mem_free(int_io_dev_ptr->OUT_QUEUE);
      int_io_dev_ptr->OUT_QUEUE = NULL;
      _taskq_destroy(int_io_dev_ptr->IN_WAITING_TASKS);
      _taskq_destroy(int_io_dev_ptr->OUT_WAITING_TASKS);
   } /* Endif */

   _int_enable();
   return(result);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_putc_internal
* Returned Value   : void
* Comments         : 
*   This function writes out the character to the device if the queue
* is empty, or it writes it to the device.  If the queue is full, this
* function will suspend the writing task.
*
*END*********************************************************************/

static bool _io_serial_mix_putc_internal
   (
      /* [IN] the interrupt io device information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to print out */
      char                     c, 
      _mqx_uint                flags
   )
{ /* Body */
   volatile CHARQ_STRUCT      *out_queue;

   /* Start CR 388 */
#if (PSP_MEMORY_ADDRESSING_CAPABILITY > 8 )
   c &= 0xFF;
#endif
   /* End CR 388 */

   out_queue = int_io_dev_ptr->OUT_QUEUE;
   _int_disable();
   if(flags & IO_SERIAL_NON_BLOCKING) {
      if (_CHARQ_FULL(out_queue)) {
          _int_enable();
          return FALSE;
      } /* Endif */
   } else {
      if(int_io_dev_ptr->HAVE_STOPPED_OUTPUT) {
          _taskq_suspend(int_io_dev_ptr->OUT_WAITING_TASKS);
      } /* Endif */
      while (_CHARQ_FULL(out_queue)) {
         /* Lets wait */
         _taskq_suspend(int_io_dev_ptr->OUT_WAITING_TASKS);
      } /* Endif */
   } /* Endif */


   if (int_io_dev_ptr->HAVE_STOPPED_OUTPUT ||
       (int_io_dev_ptr->OUTPUT_ENABLED && !(int_io_dev_ptr->TX_DEV_PUTC)))
   {   
      _CHARQ_ENQUEUE(out_queue,c);
   } else {
      int_io_dev_ptr->OUTPUT_ENABLED = TRUE;
      (*int_io_dev_ptr->DEV_PUTC)(int_io_dev_ptr, c);
   } /* Endif */
   _int_enable();
   return TRUE;

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_read
* Returned Value   : _mqx_int number of characters read
* Comments         :
*    This routine reads characters from the input ring buffer,
*    converting carriage return ('\r') characters to newlines,
*    and then echoing the input characters.
*
*END*********************************************************************/

_mqx_int _io_serial_mix_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char              *data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int               num
      
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   KUART_INFO_STRUCT_PTR           sci_info_ptr;

   uint32_t                        dmaUC     = 0, dmaC = 0;
   uint32_t                        queueSize = 0;
   uint8_t                        *dmaBuffer = NULL;
   uint32_t                        dmaOff = 0, readOff = 0;


   io_dev_ptr     =             fd_ptr->DEV_PTR;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   sci_info_ptr   =     int_io_dev_ptr->DEV_INFO_PTR;

   queueSize =  int_io_dev_ptr->IQUEUE_SIZE;
   dmaBuffer = (uint8_t *)sci_info_ptr->RX_BUF;


   _int_disable();
   dma_channel_status(sci_info_ptr->RX_DCH, &sci_info_ptr->RX_DMA_SEQ, &dmaUC);
   dmaC = queueSize - dmaUC;

#if 1
   if((int64_t)(dmaC + sci_info_ptr->dmaC - sci_info_ptr->readC) < num){
       sci_info_ptr->rxwakeupnum = num;
       _int_enable();
       _taskq_suspend(int_io_dev_ptr->IN_WAITING_TASKS);
       _int_disable();
       dma_channel_status(sci_info_ptr->RX_DCH, &sci_info_ptr->RX_DMA_SEQ, &dmaUC);
       dmaC = queueSize - dmaUC;
   }
#endif
#if 0
   while((int64_t)(dmaC + sci_info_ptr->dmaC - sci_info_ptr->readC) < num){
       sci_info_ptr->rxwakeupnum = num;
       _int_enable();
       _taskq_suspend(int_io_dev_ptr->IN_WAITING_TASKS);
       //_sched_yield();
       _int_disable();
       dma_channel_status(sci_info_ptr->RX_DCH, &sci_info_ptr->RX_DMA_SEQ, &dmaUC);
       dmaC = queueSize - dmaUC;
   }
#endif

   if((int64_t)(dmaC + sci_info_ptr->dmaC - sci_info_ptr->readC) > queueSize){
       printf("UART dma buffer full! Reset read header. \n");
       sci_info_ptr->readC = dmaC + sci_info_ptr->dmaC - num;
   }


   dmaOff  = (dmaC + sci_info_ptr->dmaC) % queueSize;
   readOff =  sci_info_ptr->readC % queueSize;

   if(readOff + num > queueSize){
       memcpy(data_ptr,                       dmaBuffer + readOff, queueSize - readOff);
       memcpy(data_ptr + queueSize - readOff, dmaBuffer,           readOff + num - queueSize);

   }else{
       memcpy(data_ptr, dmaBuffer + readOff, num);
   }

   sci_info_ptr->readC += num;
   _int_enable();
   //printf("mix read %d bytes, dmaC %d \n", num, dmaC);

   return num;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_mix_write
* Returned Value   : _mqx_int
* Comments         :
*
*END**********************************************************************/

static _mqx_int _io_serial_mix_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters to print out are */
      char              *data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int               num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       flags;
   _mqx_int                        i = num;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   flags          = fd_ptr->FLAGS;


   while ( i != 0 ) {
      if (_io_serial_mix_putc_internal(int_io_dev_ptr, *data_ptr, flags)){
          data_ptr++;
          i--;
      } else {
         num -= i;
         break;
      } /* Endif */
      
   } /* Endwhile */
   
   return num;
   
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _io_serial_mix_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

static _mqx_int _io_serial_mix_ioctl
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      void                  *param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       result = MQX_OK;
   _mqx_uint_ptr                   uparam_ptr = (_mqx_uint_ptr)param_ptr;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   switch (cmd) {
      case IO_IOCTL_FLUSH_OUTPUT:
         /* Disable interrupts to avoid situation that last TX interrupt comes after successfull !_CHARQ_EMPTY() */
         _int_disable();
         while(!_CHARQ_EMPTY(int_io_dev_ptr->OUT_QUEUE)) {
            /* wait untill all chars are sent from output queue */
            _taskq_suspend(int_io_dev_ptr->OUT_WAITING_TASKS); 
         };
         _int_enable();
         break;
            
      default:
         if (int_io_dev_ptr->DEV_IOCTL != NULL) {
            result = (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR,
               cmd, param_ptr);
         } /* Endif */
      break;
   } /* Endswitch */
   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_mix_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install an interrupt/dma driven serial device.
*
*END*----------------------------------------------------------------------*/

static _mqx_uint _io_serial_mix_install
   (
      /* [IN] A string that identifies the device for fopen */
      char             *identifier,
  
      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(void *, char *),

      /* [IN] The enable interrupts function */
      _mqx_uint (_CODE_PTR_ enable_ints)(void *),

      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(void *, void *),

      /* [IN] The output function */
      void    (_CODE_PTR_  putc)(void *, char),

      /* [IN] The I/O ioctl function */
      _mqx_uint (_CODE_PTR_ ioctl)(void *, _mqx_uint, void *),

      /* [IN] The I/O init data pointer */
      void                *init_data_ptr,
      
      /* [IN] The I/O queue size to use */
      _mqx_uint             oqueue_size,
      _mqx_uint             iqueue_size
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   uint32_t                         result;

   int_io_dev_ptr = _mem_alloc_system_zero(
      (_mem_size)sizeof(IO_SERIAL_INT_DEVICE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (int_io_dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif
   _mem_set_type(int_io_dev_ptr,MEM_TYPE_IO_SERIAL_INT_DEVICE_STRUCT);    

   int_io_dev_ptr->DEV_INIT          = init;
   int_io_dev_ptr->DEV_ENABLE_INTS   = enable_ints;
   int_io_dev_ptr->DEV_DEINIT        = deinit;
   int_io_dev_ptr->DEV_PUTC          = putc;
   int_io_dev_ptr->DEV_IOCTL         = ioctl;
   int_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   int_io_dev_ptr->QUEUE_SIZE        = oqueue_size;
   int_io_dev_ptr->OQUEUE_SIZE       = oqueue_size;
   int_io_dev_ptr->IQUEUE_SIZE       = iqueue_size;
   
   result = _io_dev_install(identifier,
      _io_serial_mix_open, _io_serial_mix_close,
      _io_serial_mix_read, _io_serial_mix_write,
      _io_serial_mix_ioctl,
      (void *)int_io_dev_ptr); 
   
   return result;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_mix_install
* Returned Value   : uint32_t a task error code or MQX_OK
* Comments         :
*    Install an mix driven uart serial device.
*
*END*----------------------------------------------------------------------*/

uint32_t _kuart_mix_install
	(
	   /* [IN] A string that identifies the device for fopen */
	   char * identifier,

	   /* [IN] The I/O init data void * */
	   KUART_INIT_STRUCT_CPTR  init_data_ptr,

	   /* [IN] The I/O queue size to use */
	   uint32_t  oqueue_size,
	   uint32_t  iqueue_size
	)
{ /* Body */

#if PE_LDD_VERSION
	 if (PE_PeripheralUsed((uint32_t)_bsp_get_serial_base_address(init_data_ptr->DEVICE)))
	 {
	     return IO_ERROR;
	 }
#endif

	return _io_serial_mix_install(identifier,
	   (uint32_t (_CODE_PTR_)(void *, char *_))_kuart_mix_init,
	   (uint32_t (_CODE_PTR_)(void *))_kuart_mix_enable,
	   (uint32_t (_CODE_PTR_)(void *,void *))_kuart_mix_deinit,
	   (void    (_CODE_PTR_)(void *, char))_kuart_mix_int_putc,
	   (uint32_t (_CODE_PTR_)(void *, uint32_t, void *))_kuart_polled_ioctl,
	   (void *)init_data_ptr, oqueue_size, iqueue_size);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_period_isr
* Returned Value   : void
* Comments         :
*    Periodic interrupt for mix uart.
*
*END*----------------------------------------------------------------------*/
static void _kuart_period_isr(void * data_ptr) {
    IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = data_ptr;
    KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
    uint32_t dmaUC, dmaC;

    _int_disable();

    dma_channel_status(sci_info_ptr->RX_DCH, &sci_info_ptr->RX_DMA_SEQ, &dmaUC);
    dmaC = int_io_dev_ptr->IQUEUE_SIZE - dmaUC;


    if (_QUEUE_GET_SIZE(int_io_dev_ptr->IN_WAITING_TASKS)) {
        if((int64_t)(dmaC + sci_info_ptr->dmaC - sci_info_ptr->readC) >= sci_info_ptr->rxwakeupnum){
            _taskq_resume(int_io_dev_ptr->IN_WAITING_TASKS, TRUE);
        }
    }

    _int_enable();
}

uint32_t _kuart_mix_init
	(
	   /* [IN] the interrupt I/O initialization information */
	   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

	   /* [IN] the rest of the name of the device opened */
	   char *                     open_name_ptr
	)
{ /* Body */
      KUART_INFO_STRUCT_PTR sci_info_ptr;
      KUART_INIT_STRUCT_PTR sci_init_ptr;
      uint32_t                     result = MQX_OK;

      //Init basic uart setting
      sci_init_ptr = int_io_dev_ptr->DEV_INIT_DATA_PTR;
      result = _kuart_polled_init((void *)sci_init_ptr, &int_io_dev_ptr->DEV_INFO_PTR, open_name_ptr);
      if (result != MQX_OK) {
          return(result);
      }/* Endif */
      sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
      sci_info_ptr->RX_KEEP = 1;

      /* Add a timer config to check the UART RX state */    
      sci_info_ptr->LW_TIMER_PTR = (LWTIMER_PERIOD_STRUCT_PTR)_mem_alloc_system_zero(sizeof(LWTIMER_PERIOD_STRUCT) + sizeof(LWTIMER_STRUCT));
      if (!sci_info_ptr->LW_TIMER_PTR)
      {
          result = MQX_OUT_OF_MEMORY;
          goto error_free_timer;
      }
      sci_info_ptr->LW_TIMER = (LWTIMER_STRUCT_PTR)((uint8_t *)sci_info_ptr->LW_TIMER_PTR + sizeof(LWTIMER_PERIOD_STRUCT));
      _time_init_ticks(&sci_info_ptr->ticks, 1);  
      _lwtimer_create_periodic_queue(sci_info_ptr->LW_TIMER_PTR ,*(uint32_t *)(&sci_info_ptr->ticks), 0);

      _lwtimer_add_timer_to_queue(
              sci_info_ptr->LW_TIMER_PTR, 
              sci_info_ptr->LW_TIMER,
              0,
              _kuart_period_isr,
              (void *)int_io_dev_ptr);

      /* RX DMA setting */
      sci_info_ptr->RX_BUF = _mem_alloc_system_zero_uncached(sci_init_ptr->IQUEUE_SIZE);
      sci_info_ptr->RX_ACTIVE = sci_info_ptr->RX_BUF;
      if (!sci_info_ptr->RX_BUF)
      {
              result = MQX_OUT_OF_MEMORY;
              goto error_free_buf;
      }
      sci_info_ptr->RX_DMA_CHAN = sci_init_ptr->RX_DMA_CHANNEL;
      sci_info_ptr->RX_DMA_HARDWARE_REQUEST = _bsp_get_serial_rx_dma_request(sci_init_ptr->DEVICE);
#if PSP_MQX_CPU_IS_KINETIS
      sci_info_ptr->ERR_INT = _bsp_get_serial_error_int_num(sci_init_ptr->DEVICE);
#endif
    sci_info_ptr->RX_DMA_SEQ = 0;
    result = dma_channel_claim(&sci_info_ptr->RX_DCH, sci_info_ptr->RX_DMA_CHAN);
    if (result != MQX_OK)
    {
            goto error_free_channel;
    }
    result = dma_callback_reg(sci_info_ptr->RX_DCH, _kuart_dma_rx_isr, int_io_dev_ptr);
    dma_channel_setup(sci_info_ptr->RX_DCH, 1, DMA_CHANNEL_FLAG_LOOP_MODE);
    dma_request_source(sci_info_ptr->RX_DCH, sci_info_ptr->RX_DMA_HARDWARE_REQUEST);

    if (result != MQX_OK)
    {
            goto error_free_channel;
    }


    /* Init RX/TX interrupt vector */

    sci_info_ptr->OLD_ISR_TXRX_DATA = _int_get_isr_data(sci_init_ptr->RX_TX_VECTOR);
    sci_info_ptr->OLD_ISR_EXCEPTION_HANDLER = _int_get_exception_handler(sci_init_ptr->RX_TX_VECTOR);

    sci_info_ptr->OLD_ISR_TXRX =
    _int_install_isr(sci_init_ptr->RX_TX_VECTOR, _kuart_mix_int_tx_isr, int_io_dev_ptr);
    _bsp_int_init(sci_init_ptr->RX_TX_VECTOR, sci_init_ptr->RX_TX_PRIORITY, 0, TRUE);


    /* Init mix error vector */
    sci_info_ptr->OLD_ISR_ERR =
    _int_install_isr(sci_info_ptr->ERR_INT, _kuart_mix_err_isr, int_io_dev_ptr);
    _bsp_int_init(sci_info_ptr->ERR_INT, sci_init_ptr->ERR_PRIORITY, 0, TRUE);

    /* config the UART RX channel first */
    _kuart_prepare_rx_dma(int_io_dev_ptr);      
    return(MQX_OK);

error_free_timer:
    if (!sci_info_ptr->LW_TIMER_PTR)
    _mem_free(sci_info_ptr->LW_TIMER_PTR);

error_free_channel:
    dma_channel_release(sci_info_ptr->RX_DCH);
    
error_free_buf:
    if (sci_info_ptr->RX_BUF)
            _mem_free(sci_info_ptr->RX_BUF);

    return result;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_deinit
* Returned Value   : uint32_t a task error code or MQX_OK
* Comments         :
*    This function de-initializes the UART in mix mode.
*
*END*********************************************************************/

uint32_t _kuart_mix_deinit
	(
	   /* [IN] the interrupt I/O initialization information */
	   KUART_INIT_STRUCT_PTR io_init_ptr,

	   /* [IN] the address of the device specific information */
	   KUART_INFO_STRUCT_PTR io_info_ptr
	)
{ /* Body */
    if (!io_info_ptr)
        return MQX_INVALID_PARAMETER;

    if (io_info_ptr->LW_TIMER_PTR) {
        _lwtimer_cancel_period(io_info_ptr->LW_TIMER_PTR);
        _mem_free(io_info_ptr->LW_TIMER_PTR);
    }

    dma_request_disable(io_info_ptr->RX_DCH);
    dma_channel_release(io_info_ptr->RX_DCH);
	
    _kuart_polled_deinit(io_init_ptr, io_info_ptr);

    if (io_info_ptr->RX_BUF)
        _mem_free(io_info_ptr->RX_BUF);

    _int_install_isr(io_info_ptr->ERR_INT, io_info_ptr->OLD_ISR_ERR, io_info_ptr->OLD_ISR_ERR_DATA);
    _int_install_isr(io_init_ptr->RX_TX_VECTOR, io_info_ptr->OLD_ISR_TXRX, io_info_ptr->OLD_ISR_TXRX_DATA);

    return(MQX_OK);

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_enable
* Returned Value   : uint32_t a task error code or MQX_OK
* Comments         :
*    This function enables the UART interrupts mode.
*
*END*********************************************************************/

uint32_t _kuart_mix_enable
	(
	   /* [IN] the address of the device specific information */
	   KUART_INFO_STRUCT_PTR io_info_ptr
	)
{ /* Body */
    uint8_t                 flags = IO_PERIPHERAL_MODULE_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE;
    UART_MemMapPtr          sci_ptr = io_info_ptr->SCI_PTR;

    /* Enable module clocks to be able to write registers */
    _bsp_serial_io_init (io_info_ptr->INIT.DEVICE, IO_PERIPHERAL_CLOCK_ENABLE);

    /* Enable/disable module */
    if (flags & IO_PERIPHERAL_MODULE_ENABLE) {
       _kuart_mix_peripheral_enable (sci_ptr);
    }
    else {
       _kuart_mix_peripheral_disable (sci_ptr);
    }

    /* Disable module clocks if required */
    if (flags & IO_PERIPHERAL_CLOCK_DISABLE) {
       _bsp_serial_io_init (io_info_ptr->INIT.DEVICE, IO_PERIPHERAL_CLOCK_DISABLE);
    }

    /* start the convert receive of RX */
    dma_request_enable(io_info_ptr->RX_DCH);
    
    return MQX_OK;

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_err_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial error interrupts.
*
*************************************************************************/

void _kuart_mix_err_isr
	(
	   /* [IN] the address of the device specific information */
	   void * parameter
	)
{ /* Body */

    IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
    KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
    UART_MemMapPtr                         sci_ptr = sci_info_ptr->SCI_PTR;
    uint16_t                                stat;

    ++sci_info_ptr->INTERRUPTS;
    stat = sci_ptr->S1;

    if(stat & UART_S1_OR_MASK) {
       ++sci_info_ptr->RX_OVERRUNS;
    }
    if(stat & UART_S1_PF_MASK) {
       ++sci_info_ptr->RX_PARITY_ERRORS;
    }
    if(stat & UART_S1_NF_MASK) {
       ++sci_info_ptr->RX_NOISE_ERRORS;
    }
    if(stat & UART_S1_FE_MASK) {
       ++sci_info_ptr->RX_FRAMING_ERRORS;
    }

}  /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_mix_rx_isr
* Returned Value   : none
* Comments         :
*  mix callback for the serial rx.
*
*************************************************************************/
void _kuart_dma_rx_isr
(
   /* [IN] the address of the device specific information */
   void * parameter,
   int tcd_done,
   uint32_t tcd_seq
)
{ /* Body */
    IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
    KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
    uint32_t i;
    int8_t cc;
    uint8_t *rx_buf;	

#if 0
    _int_disable();
    rx_buf = sci_info_ptr->RX_ACTIVE;
    sci_info_ptr->RX_ACTIVE = sci_info_ptr->RX_ACTIVE == sci_info_ptr->RX_BUF ?
            sci_info_ptr->RX_BUF + int_io_dev_ptr->IN_QUEUE->MAX_SIZE / 2 :
	    sci_info_ptr->RX_BUF;	
    ++sci_info_ptr->INTERRUPTS;
    for (i=sci_info_ptr->RX_BYTES; i<int_io_dev_ptr->IN_QUEUE->MAX_SIZE / 2; i++)
    {
        cc = rx_buf[i];
        _io_serial_int_addc(int_io_dev_ptr, cc);
    }
    sci_info_ptr->RX_CHARS += int_io_dev_ptr->IN_QUEUE->MAX_SIZE / 2 - sci_info_ptr->RX_BYTES;
    sci_info_ptr->RX_KEEP = 1;
    sci_info_ptr->RX_BYTES = 0;
    _int_enable();
#endif
    //_int_disable();
    sci_info_ptr->dmaC += int_io_dev_ptr->IQUEUE_SIZE;
    //_int_enable();
    //printf("UART dma rx isr \n");
    //printf("I \n");
}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_prepare_rx_mix
* Returned Value   : void
* Comments         :
*    Configure and enable rx mix tansmit.
*
*END*----------------------------------------------------------------------*/
static void _kuart_prepare_rx_dma(IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr)
{
    DMA_TCD						tcd;
    KUART_INFO_STRUCT_PTR                  sci_info_ptr;
    UART_MemMapPtr                         sci_ptr;

    sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
    sci_ptr = sci_info_ptr->SCI_PTR;
    dma_tcd_reg2mem(&tcd, &(sci_ptr->D) , 1, sci_info_ptr->RX_BUF, int_io_dev_ptr->IQUEUE_SIZE);
    dma_transfer_submit(sci_info_ptr->RX_DCH, &tcd, &sci_info_ptr->RX_DMA_SEQ);
    dma_request_enable(sci_info_ptr->RX_DCH);
}
/* EOF */
