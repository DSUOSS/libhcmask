#libhcmask

A wordlist generation library based on hashcat masks


#Authors

- Andrew Kramer
- Alex Gartner


#Example Usage

	#include <stdio.h>
	#include "libhcmask.h"

	int main() {

		// Create a mask_t pointer
		mask_t *maskctx;

		// Initialize the mask_t
		//  - mask = "?2?1?1?d?a"
		//  - custom_charset_1 = "?dabcdef" (all hex characters)
		//  - custom_charset_2 = "?l?u"		(loweralpha + upperalpha)
		// (NOTE: lib hcmask can handle up to 9 custom character sets)
		maskctx = init_mask("?2?1?1?d?a", "?dabcdef", "?l?u");

		// Ensure initilization was successful
		if(!maskctx)
			return 1;

		// Self explanatory
		do {
			printf("%s\n", maskctx->current_string);
		} while(next_mask(maskctx));

		// Cleanup after ourselves
		free_mask(maskctx);

		return 0;

	}

