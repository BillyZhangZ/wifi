How-to Use Sai_dma_demo example

Start sai_dma_demo example on the target platform, this example will firstly record wav sound via line-in port to
the internal RAM, then playback the sound out to headphone port. A terminal application can be started on your PC
to show the related message.

The sai driver suppport 16bit, 32bit quantization level, default set to 16bit
Supporting sample rate: 8K, 11.025k, 12k, 16k, 22.05k, 24k, 32k, 44.1k, 48k, 
96KHZ, default set to 8k

Example platform: TWR-K22F120M + TWR-AUDIO-SGTL board.

NOTE:
   1) Record input use line in port while playback output use headphone port.
   2) Due to RAM size limitation, the record parameters were configured to be
   8K HZ, 2 channels, one second duration, test of sample rate besides 8k HZ
   is currently not supported in this example.
