#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#define DE2_PIO_GREENLED9_BASE 0x90e0
#define DE2_PIO_REDLED18_BASE 0x9120
#define D2_PIO_KEYS4_BASE 0x9140
#define DE2_PIO_TOGGLES18_BASE 0x9150



int main(void) {
	alt_u8 keyVal_hex, gled_status, pbVal_hex;

  while (1)
	

	{
	/*This section will Cover interfacing Switches with green LEDs*/

		// /*This is Switch 1
		// *
		// */
	
		// keyVal_hex = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE); //This returns a binary no. with all set bits equal to ones
		// if (keyVal_hex & 0x2) //Use & here because we just check that certain bit and don't care about the others
		// {
		// 	IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,0x8); //This function takes a decimal or hex and sets the corresponding bits
		// 	
		//printf("Switching ON LED 3 via Switch 1\n");

		// }
		// else
		// {
		// 	IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,0x8); //Using Clear_bits you just pass the bits you want to clear
		// 	
		//printf("Switching OFF LED 3 via Switch 1\n");
		// }

		// /*This is Switch 2
		// *
		// */
	
		// keyVal_hex = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE); //This returns a binary no. with all set bits equal to ones
		// if (keyVal_hex & 0x4) //Use & here because we just check that certain bit and don't care about the others
		// {
		// 	IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,16); //This function takes a decimal or hex and sets the corresponding bits
		// 	
		//printf("Switching ON LED 4 via Switch 2\n");

		// }
		// else
		// {
		// 	IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,16); //Using Clear_bits you just pass the bits you want to clear
		// 	
		//printf("Switching OFF LED 4 via Switch 2\n");
		// }

		// /*This is Switch 3
		// *
		// */
		// keyVal_hex = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE); //This returns a binary no. with all set bits equal to ones
		// if (keyVal_hex & 0x8) //Use & here because we just check that certain bit and don't care about the others
		// {
		// 	IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,32); //This function takes a decimal or hex and sets the corresponding bits
		// 	
		//printf("Switching ON LED 5 via Switch 3\n");

		// }
		// else
		// {
		// 	IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,32); //Using Clear_bits you just pass the bits you want to clear
		// 	
		//printf("Switching OFF LED 5 via Switch 3\n");
		// }

	/*This section will Cover interfacing Push Buttons with green LEDs*/

	
		pbVal_hex = ~IORD_ALTERA_AVALON_PIO_DATA(D2_PIO_KEYS4_BASE);
		/*This is Pb 1
		 *
		 */
		if (pbVal_hex & 0x02) //Use & here because we just check that certain bit and don't care about the others
		{
			IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,32); //This function takes a decimal or hex and sets the corresponding bits
			
			//printf("Switching ON LED 5 via Pb 1\n");

		}
		else
		{
			IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,32); //Using Clear_bits you just pass the bits you want to clear
			
			//printf("Switching OFF LED 5 via Pb 1\n");
		}
		/*This is Pb 2
		 *
		 */
		if (pbVal_hex & 0x04) //Use & here because we just check that certain bit and don't care about the others
		{
			IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,64); //This function takes a decimal or hex and sets the corresponding bits
			
			//printf("Switching ON LED 5 via Pb 1\n");

		}
		else
		{
			IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,64); //Using Clear_bits you just pass the bits you want to clear
			
			//printf("Switching OFF LED 5 via Pb 1\n");
		}

		/*This is Pb 3
		 *
		 */

		if (pbVal_hex & 0x08) //Use & here because we just check that certain bit and don't care about the others
		{
			IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,128); //This function takes a decimal or hex and sets the corresponding bits
			
			//printf("Switching ON LED 5 via Pb 1\n");

		}
		else
		{
			IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,128); //Using Clear_bits you just pass the bits you want to clear
			
			//printf("Switching OFF LED 5 via Pb 1\n");
		}



		



	}

  
  return 0;
}
