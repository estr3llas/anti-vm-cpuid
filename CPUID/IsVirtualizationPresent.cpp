#pragma once

#include <stdio.h>
#include <windows.h>

BOOL IsVirtualizationPresent();

int main(int argc, char* argv[]) {

	puts(IsVirtualizationPresent() ? "[+] Virtualization detected!" : "[-] No virtualization detected.");

	return EXIT_SUCCESS;
}

BOOL IsVirtualizationPresent() {

	BOOL beingVirtualized;
		
	__asm {
		mov eax, 1
		cpuid

		test ecx, 0x80000000
		jnz is_virtualized

		mov beingVirtualized, 0
		jmp done

	
	is_virtualized:
		mov beingVirtualized, 1
		jmp done

	done:
	};

	if (beingVirtualized)
		return TRUE;

	return FALSE;
}