#include <stdio.h>
#include <Windows.h>
#include <inttypes.h>


int main(int argc, char* argv[]) {

	BOOL isVirtualized;
	int maximumFunction = 0;
	int vendor[13];

	__asm {
		mov eax, 1
		cpuid

		test ecx, 0x80000000
		jnz is_virtualized

		mov isVirtualized, 0
		jmp done

	is_virtualized:
		mov isVirtualized, 1

		xor eax, eax
		mov eax, 0x40000000
		cpuid

		mov maximumFunction, eax
		mov vendor[0], ebx
		mov vendor[4], edx
		mov vendor[8], ecx
		mov byte ptr[vendor + 12], 0

	done:
	};

	if (isVirtualized == FALSE) {
		puts("No virtualization detected");
		return EXIT_SUCCESS;
	}

	puts("[+] Virtualization detected!");
	printf("\t[+] Maximum supported CPUID functions: %" PRId32 "\n", maximumFunction);
	printf("\t[+] Vendor id: %s\n", vendor);

	return EXIT_SUCCESS;
}