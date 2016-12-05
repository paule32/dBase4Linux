#include <stdio.h>
#include <stdlib.h>
#include <printf.h>

#include "gmp.h"

int
main()
{
	mpf_t want;

	mpf_init(want);
	mpf_init_set_str(want,"12.302222222222222000000",10);
	gmp_printf("%10.20Ff\n",want);
	mpf_clear(want);

	return 0;
}

