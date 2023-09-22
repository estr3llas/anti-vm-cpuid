#include <stdio.h>
#include <Windows.h>
#include <inttypes.h>


int main(int argc, char* argv[]) {

	BOOL beingVirtualized;
	int maximumFunction = 0;
	int vendor[13];					//every vendor-id is a 12 char string, 13 = \0

	__asm {
		mov eax, 1				//leaf 1 stored in eax
		cpuid

		test ecx, 0x80000000			//if MSB of ECX is equal to 1, then it is being virtualized
		jnz is_virtualized

		mov beingVirtualized, 0			//no virtualization
		jmp done

	is_virtualized:
		mov beingVirtualized, 1			//virtualization detected

		xor eax, eax				//eax equals 0
		mov eax, 0x40000000			//hypervisor brand(?) stored in  eax
		cpuid

		mov maximumFunction, eax		//up to what leaf number we can query
		mov vendor[0], ebx
		mov vendor[4], edx
		mov vendor[8], ecx
		mov byte ptr[vendor + 12], 0		/*each "piece" of vendor id is stored into a register.
							So, we simply store those values onto vendor[] array.
							Next, we manually put a NULL character for printing purposes.
							*/
	done:
	};

	if (beingVirtualized == FALSE) {
		puts("No virtualization detected");
		return EXIT_SUCCESS;
	}

	puts("[+] Virtualization detected!");
	printf("\t[+] Maximum supported CPUID functions: %" PRId32 "\n", maximumFunction);
	printf("\t[+] Vendor id: %s\n", (char*)vendor);

	return EXIT_SUCCESS;
}
